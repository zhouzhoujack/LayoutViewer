#include "layMainWindow.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "detail/utilsProgress.h"
#include "utilsStream.h"

#include "layLayoutView.h"
#include "layResourceTree.h"
#include "layAbstractMenu.h"
#include "layApplication.h"
#include "layConfig.h"
#include "layControlWidgetStack.h"
#include "layFileDialog.h"
#include "layLayerToolbox.h"
#include "layMenuEntry.h"
#include "layPluginDeclaration.h"
#include "layProgressWidget.h"
#include "layProgressDialog.h"
#include "layRulerPlugin.h"
#include "laySettingsForm.h"
#include "layViewWidgetStack.h"

#include "geoLoadLayoutOptions.h"

#include "myRegistrar.h"
#include "tlLog.h"

namespace lay
{

// ---------------------------------------------------------------------------------------------
//  OpenRecentAction implementation

class OpenRecentAction : public lay::Action
{
public:
    OpenRecentAction(lay::MainWindow* mw, size_t n) : lay::Action(), mp_mw(mw), m_n(n) {}

    void triggered()
    {
        mp_mw->open_recent(m_n);
    }

private:
    lay::MainWindow* mp_mw;
    size_t m_n;
};

// ---------------------------------------------------------------------------------------------
//  MainWindow implementation

static MainWindow* mw_instance = 0;

const size_t max_mru = 16;

MainWindow* MainWindow::instance()
{
    return mw_instance;
}

MainWindow::MainWindow(QApplication* app, const char* name, bool undo_enabled)
    : QMainWindow(0), lay::DispatcherDelegate(), dm_do_update_mru_menus(this, &MainWindow::do_update_mru_menus),
      m_dispatcher(this), m_text_progress(this, 10)
{
    m_dispatcher.set_menu_parent_widget(this);

    init_menu();
    setup_dock();
    setup_tool_bar();
    setup_central_widget();
    setup_status_bar();
    setup_resource_tree();

    mp_file_dlg = new lay::FileDialog(this, "Layout File", {});
    mp_setup_form = new SettingsForm(0, dispatcher(), "setup_form");

    mw_instance = this;
}

void MainWindow::setup_central_widget()
{
    mp_pr = new lay::ProgressReporter();
    mp_progress_dialog = new lay::ProgressDialog(mp_pr, this);
    mp_pr->set_progress_bar(mp_progress_dialog);

    mp_main_stack_widget = new QStackedWidget(this);
    mp_main_stack_widget->setObjectName(QString::fromUtf8("main_stack"));
    setCentralWidget(mp_main_stack_widget);

    mp_main_frame = new QFrame(mp_main_stack_widget);
    mp_main_frame->setObjectName(QString::fromUtf8("main_frame"));
    mp_main_stack_widget->addWidget(mp_main_frame);

    mp_progress_widget = new lay::ProgressWidget(mp_pr, mp_main_stack_widget);
    mp_progress_widget->setObjectName(QString::fromUtf8("progress"));
    mp_main_stack_widget->addWidget(mp_progress_widget);

    mp_main_stack_widget->setCurrentIndex(0);

    QVBoxLayout* vbl = new QVBoxLayout(mp_main_frame);
    vbl->setMargin(0);
    vbl->setSpacing(0);

    mp_tab_bar = new QTabBar(mp_main_frame);
    vbl->addWidget(mp_tab_bar);
    connect(mp_tab_bar, &QTabBar::currentChanged, this, &MainWindow::view_selected);
    mp_tab_bar->setTabsClosable(true);
    connect(mp_tab_bar, &QTabBar::tabCloseRequested, this, &MainWindow::tab_close_requested);

    mp_view_stack = new ViewWidgetStack(mp_main_frame);
    mp_view_stack->setObjectName(QString::fromUtf8("view_stack"));
    vbl->addWidget(mp_view_stack);
}

void MainWindow::view_selected(int index)
{
    if (index >= 0 && index < int(views()))
    {
        mp_tab_bar->setFocus();
        select_view(index);
    }
}

void MainWindow::cm_setup()
{
    mp_setup_form->show();
    mp_setup_form->setup();
}

void MainWindow::tab_close_requested(int index)
{
    interactive_close_view(index, true);
}

void MainWindow::setup_status_bar()
{
    mp_status_bar = statusBar();
    mp_status_bar->setObjectName(QString::fromUtf8("status_bar"));
    mp_status_bar->setStyleSheet(QString::fromUtf8("QStatusBar { border-top: 1px solid rgb(223, 222, 222); }"));

    // --- DBU section ---
    mp_dbu_frame = new QFrame(mp_status_bar);
    mp_dbu_frame->setObjectName(QString::fromUtf8("status_bar_section"));
    QHBoxLayout* dbu_ly = new QHBoxLayout(mp_dbu_frame);
    dbu_ly->setContentsMargins(8, 0, 8, 0);
    dbu_ly->setSpacing(4);
    QLabel* dbu_title = new QLabel(QString::fromUtf8("DBU(um):"), mp_dbu_frame);
    dbu_title->setObjectName(QString::fromUtf8("status_bar_title"));
    mp_grid_size_label = new QLabel(mp_dbu_frame);
    dbu_ly->addWidget(dbu_title);
    dbu_ly->addWidget(mp_grid_size_label);
    dbu_ly->addStretch();

    // --- Cell section ---
    mp_cell_frame = new QFrame(mp_status_bar);
    mp_cell_frame->setObjectName(QString::fromUtf8("status_bar_section"));
    QHBoxLayout* cell_ly = new QHBoxLayout(mp_cell_frame);
    cell_ly->setContentsMargins(8, 0, 8, 0);
    cell_ly->setSpacing(4);
    QLabel* cell_title = new QLabel(QString::fromUtf8("Cell:"), mp_cell_frame);
    cell_title->setObjectName(QString::fromUtf8("status_bar_title"));
    mp_cur_cell_label = new QLabel(mp_cell_frame);
    cell_ly->addWidget(cell_title);
    cell_ly->addWidget(mp_cur_cell_label);
    cell_ly->addStretch();

    // --- Coord section ---
    mp_coord_frame = new QFrame(mp_status_bar);
    mp_coord_frame->setObjectName(QString::fromUtf8("status_bar_section"));
    QHBoxLayout* coord_ly = new QHBoxLayout(mp_coord_frame);
    coord_ly->setContentsMargins(8, 0, 8, 0);
    coord_ly->setSpacing(4);
    QLabel* coord_title = new QLabel(QString::fromUtf8("Coord(um):"), mp_coord_frame);
    coord_title->setObjectName(QString::fromUtf8("status_bar_title"));
    mp_cpx_label = new QLabel(mp_coord_frame);
    mp_cpx_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    mp_cpx_label->setMinimumSize(90, 0);
    mp_cpy_label = new QLabel(mp_coord_frame);
    mp_cpy_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    mp_cpy_label->setMinimumSize(90, 0);
    coord_ly->addWidget(coord_title);
    coord_ly->addWidget(mp_cpx_label);
    coord_ly->addWidget(mp_cpy_label);
    coord_ly->addStretch();

    // DBU stays at the far left, Cell is centered, Coord stays at the far right.
    // The two equal-stretch spacers balance the space on both sides of Cell.
    auto make_spacer = [this]() {
        QWidget* spacer = new QWidget(mp_status_bar);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return spacer;
    };

    mp_status_bar->addPermanentWidget(mp_dbu_frame, 0);
    mp_status_bar->addPermanentWidget(make_spacer(), 1);
    mp_status_bar->addPermanentWidget(mp_cell_frame, 0);
    mp_status_bar->addPermanentWidget(make_spacer(), 1);
    mp_status_bar->addPermanentWidget(mp_coord_frame, 0);

    mp_dbu_frame->hide();
    mp_cell_frame->hide();
    mp_coord_frame->hide();
}

void MainWindow::update_status_bar()
{
    //  determine the currently selected view
    lay::LayoutView* v = 0;
    int index = mp_tab_bar->currentIndex();
    if (index >= 0 && index < int(mp_views.size()))
    {
        v = mp_views[index];
    }

    //  no view / no file open -> hide the text only, keep the status bar visible
    if (!v || v->cellviews() == 0)
    {
        mp_dbu_frame->hide();
        mp_cell_frame->hide();
        mp_coord_frame->hide();
        return;
    }

    mp_dbu_frame->show();
    mp_cell_frame->show();
    mp_coord_frame->show();

    lay::CellViewRef cv = v->active_cellview_ref();
    if (cv.is_valid())
    {
        //  DBU value (trailing zeros stripped)
        std::string dbu_str = geo::micron_to_string(cv->layout().dbu());
        size_t dot = dbu_str.find('.');
        if (dot != std::string::npos)
        {
            size_t last = dbu_str.find_last_not_of('0');
            if (last > dot)
            {
                dbu_str.erase(last + 1);
            }
            else if (last == dot)
            {
                dbu_str.erase(dot);
            }
        }
        mp_grid_size_label->setText(geo::to_qstring(dbu_str));
        //  current cell name
        const char* cell_name = cv->layout().cell_name(cv.cell_index());
        mp_cur_cell_label->setText(cell_name ? geo::to_qstring(cell_name) : QString());
    }
    else
    {
        mp_grid_size_label->clear();
        mp_cur_cell_label->clear();
    }
}

void MainWindow::on_cellview_changed(int)
{
    //  refresh the status bar when the current cellview/cell changes
    update_status_bar();
}

void MainWindow::interactive_close_view(int index, bool all_cellviews)
{
    close_view(index);
    mp_layer_toolbox->set_view(0);
}

bool MainWindow::check_exist_file(std::string& f)
{
    return view_index_for_file(f) >= 0;
}

int MainWindow::view_index_for_file(const std::string& fn) const
{
    for (unsigned int i = 0; i < mp_views.size(); ++i)
    {
        lay::LayoutView* v = mp_views[i];
        if (!v)
        {
            continue;
        }
        for (unsigned int cv = 0; cv < v->cellviews(); ++cv)
        {
            if (v->cellview(cv)->filename() == fn)
            {
                return int(i);
            }
        }
    }
    return -1;
}

std::string MainWindow::view_filename(int index) const
{
    if (index < 0 || index >= int(mp_views.size()))
    {
        return std::string();
    }
    lay::LayoutView* v = mp_views[index];
    if (v && v->cellviews() > 0)
    {
        return v->cellview(0)->filename();
    }
    return std::string();
}

void MainWindow::close_view(int index)
{
    //  echo the close operation into the command panel (unless the command
    //  originated from the panel itself)
    if (index >= 0 && index < int(mp_views.size()))
    {
        std::string fn = view_filename(index);
        if (!fn.empty())
        {
            echo_command("close " + CommandPanel::quote(fn));
        }
    }

    mp_tab_bar->removeTab(index);
    mp_view_stack->remove_widget(index);

    view_closed_event(int(index));

    delete view(index);
    mp_views.erase(mp_views.begin() + index, mp_views.begin() + index + 1);
    mp_cell_views.erase(index);

    mp_lp_stack->remove_widget(index);
    mp_hp_stack->remove_widget(index);

    //  remove the corresponding resource tree entry (the tree is driven by the
    //  window, no item signal involved)
    m_tree->remove_layout_item(index);

    if (index >= int(mp_views.size()))
    {
        --index;
    }

    select_view(index);
}

void MainWindow::close_all()
{
    //   cancel ();
    mp_layer_toolbox->set_view(0);

    //  echo the operation into the command panel (unless the command originated
    //  from the panel itself)
    echo_command("close all");

    //  try a smooth shutdown of the current view
    lay::LayoutView::set_current(0);

    //   current_view_changed ();

    for (unsigned int i = 0; i < mp_views.size(); ++i)
    {
        mp_views[i]->stop();
    }

    m_manager.clear();

    //  Clear the tab bar
    //   bool f = m_disable_tab_selected;
    //   m_disable_tab_selected = true;
    while (mp_tab_bar->count() > 0)
    {
        mp_tab_bar->removeTab(mp_tab_bar->count() - 1);
    }
    //   m_disable_tab_selected = f;

    while (mp_views.size() > 0)
    {

        view_closed_event(int(mp_views.size() - 1));

        lay::LayoutView* view = mp_views.back();
        mp_views.pop_back();
        mp_lp_stack->remove_widget(mp_views.size());
        mp_hp_stack->remove_widget(mp_views.size());
        mp_view_stack->remove_widget(mp_views.size());

        delete view;
    }

    //   update_dock_widget_state ();

    //  clear the resource tree (driven by the window, no item signal involved)
    m_tree->remove_all_layout_items();

    //  all views closed -> hide the status bar
    update_status_bar();
}

void MainWindow::setup_dock()
{
    mp_hp_dock_widget = new QDockWidget(QObject::tr("Cells"), this);

    mp_hp_dock_widget->setObjectName(QString::fromUtf8("hp_dock_widget"));
    mp_hp_dock_widget->setStyleSheet(QString::fromUtf8("QDockWidget { border-left: 1px solid rgb(223, 222, 222); }"));
    mp_hp_stack = new ControlWidgetStack(mp_hp_dock_widget, "hp_stack");
    mp_hp_dock_widget->setWidget(mp_hp_stack);
    mp_hp_dock_widget->setFocusProxy(mp_hp_stack);

    mp_lp_dock_widget = new QDockWidget(QObject::tr("Layers"), this);

    mp_lp_dock_widget->setObjectName(QString::fromUtf8("lp_dock_widget"));
    mp_lp_dock_widget->setStyleSheet(QString::fromUtf8("QDockWidget { border-left: 1px solid rgb(223, 222, 222); }"));
    mp_lp_stack = new ControlWidgetStack(mp_lp_dock_widget, "lp_stack");
    mp_layer_toolbox = new LayerToolbox(mp_lp_dock_widget, "layer_toolbox");

    QSplitter* splitter = new QSplitter(Qt::Vertical);

    splitter->addWidget(mp_lp_stack);
    splitter->addWidget(mp_layer_toolbox);

    splitter->setMinimumWidth(300);

    mp_lp_dock_widget->setWidget(splitter);

    addDockWidget(Qt::RightDockWidgetArea, mp_lp_dock_widget);
    addDockWidget(Qt::RightDockWidgetArea, mp_hp_dock_widget);
    tabifyDockWidget(mp_lp_dock_widget, mp_hp_dock_widget);
    mp_lp_dock_widget->raise();

    m_tree_dock = new QDockWidget("Resource Tree", this);
    m_tree_dock->setStyleSheet(QString::fromUtf8("QDockWidget { border-right: 1px solid rgb(223, 222, 222); }"
                                                 "QDockWidget::title { font-size: 9pt; }"));
    addDockWidget(Qt::LeftDockWidgetArea, m_tree_dock);

    mp_command_dock = new QDockWidget("Command", this);
    addDockWidget(Qt::BottomDockWidgetArea, mp_command_dock);
    mp_command_panel = new CommandPanel(mp_command_dock, this);
    mp_command_dock->setWidget(mp_command_panel);
    mp_command_dock->hide();

    setup_panel_menus();

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

void MainWindow::setup_panel_menus()
{
    //  make the View menu items checkable toggles and keep their check state in
    //  sync with the corresponding dock widget visibility
    setup_dock_toggle("view_menu.command", mp_command_dock);
    setup_dock_toggle("view_menu.layers", mp_lp_dock_widget);
    setup_dock_toggle("view_menu.cells", mp_hp_dock_widget);
    setup_dock_toggle("view_menu.tree", m_tree_dock);
}

void MainWindow::setup_dock_toggle(const std::string& menu_path, QDockWidget* dock)
{
    lay::Action* ac = m_menu.menu_action(menu_path);
    if (ac && dock)
    {
        ac->set_checkable(true);
        ac->set_checked(dock->isVisible());

        connect(dock, &QDockWidget::visibilityChanged, this, [ac](bool visible) {
            ac->set_checked(visible);
        });
    }
}

void MainWindow::build_menu_item(const std::vector<lay::MenuEntry>& menu_entries)
{
    for (auto m = menu_entries.begin(); m != menu_entries.end(); ++m)
    {
        if (m->separator)
        {
            m_menu.insert_separator(m->insert_pos);
        }
        else if (!m->sub_menu)
        {
            m_menu.insert_menu(menuBar(), m->menu_name, m->title);
        }
        else
        {
            if (!m->cname.empty())
            {
                m_menu.insert_menu(m->menu_name, m->title, m->cname, m->cvalue);
            }
            else
            {
                m_menu.insert_menu(dispatcher(), m->menu_name, m->symbol, m->title);
            }
        }
    }
}

void MainWindow::init_menu()
{
    for (auto cls = my::Registrar<lay::PluginDeclaration>::begin(); cls != my::Registrar<lay::PluginDeclaration>::end(); ++cls)
    {
        std::vector<lay::MenuEntry> menu_entries;
        cls->get_menu_entries(menu_entries);
        build_menu_item(menu_entries);
    }
}

void MainWindow::setup_tool_bar()
{
    mp_tool_bar = new QToolBar(this);
    mp_tool_bar->setWindowTitle(QObject::tr("Toolbar"));
    mp_tool_bar->setObjectName(QString::fromUtf8("toolbar"));
    mp_tool_bar->setMovable(false);
    mp_tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::TopToolBarArea, mp_tool_bar);

    QActionGroup* group = new QActionGroup(mp_tool_bar);
    group->setExclusive(true);

    for (auto cls = my::Registrar<lay::PluginDeclaration>::begin(); cls != my::Registrar<lay::PluginDeclaration>::end(); ++cls)
    {
        cls->create_toobar_action(dispatcher(), mp_tool_bar, group);
    }
}

void MainWindow::setup_resource_tree()
{
    m_tree = new ResourceTree(this);
    connect(this, &MainWindow::file_open_signal, m_tree, &ResourceTree::addLayoutItem);
    // connect(this, &MainWindow::file_close_signal, m_tree, &ResourceTree::deleteLayoutItem);

    m_tree_dock->setWidget(m_tree);
}

void MainWindow::progress_remove_widget()
{
    // if (mp_progress_dialog) {
    //     mp_progress_dialog->remove_widget ();
    // } else if (mp_progress_widget) {
    //     mp_progress_widget->remove_widget ();
    // }
}

void MainWindow::cm_open()
{
    static std::vector<std::string> files;
    mp_file_dlg->get_open(files, "", "Open Layout Files");

    geo::LoadLayoutOptions options;

    std::vector<std::string> opened;

    for (auto fn = files.begin(); fn != files.end(); ++fn)
    {
        if (check_exist_file(*fn))
        {
            continue;
        }

        lay::CellViewRef cell_ref = load_layout(*fn, options, m_initial_technology, 1);
        mp_cell_views.emplace(views() - 1, cell_ref);
        add_mru(*fn, m_initial_technology);
        opened.push_back(*fn);
    }

    if (!opened.empty() && mp_command_panel)
    {
        std::string cmd = "open";
        for (auto f = opened.begin(); f != opened.end(); ++f)
        {
            cmd += " ";
            cmd += CommandPanel::quote(*f);
        }
        mp_command_panel->echo(cmd);
    }
}

void MainWindow::select_view(int index)
{
    if (index >= 0)
    {
        view(index)->set_current();
        mp_tab_bar->setCurrentIndex(index);
        mp_view_stack->raise_widget(index);
        //  switch the layer / hierarchy control panels together with the view
        mp_lp_stack->raise_widget(index);
        mp_hp_stack->raise_widget(index);
        mp_layer_toolbox->set_view(current_view());
    }

    update_window_title();
    update_status_bar();
}

void MainWindow::select_mode(int m)
{
    if (m_mode != m)
    {
        m_mode = m;
        LOGI << "mode: " << m;
        for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
        {
            (*vp)->mode(m);
        }

        // std::vector<std::string> items = menu ()->items ("@toolbar");
        // for (std::vector<std::string>::const_iterator i = items.begin (); i != items.end (); ++i)
        // {
        //     Action *a = menu ()->action (*i);
        //     if (a->is_checkable() && a->is_for_mode (m_mode)) {
        //         a->set_checked (true);
        //         break;
        //     }
        // }

        //  if the current mode supports editing, show the editor options panel
        // const lay::PluginDeclaration *pd_sel = 0;
        // for (my::Registrar<lay::PluginDeclaration>::iterator cls =
        // my::Registrar<lay::PluginDeclaration>::begin (); cls !=
        // my::Registrar<lay::PluginDeclaration>::end (); ++cls) {
        //     const lay::PluginDeclaration *pd = cls.operator-> ();
        //     if (pd->id () == m_mode) {
        //         pd_sel = pd;
        //     }
        // }

        // bool eo_visible = false;
        // if (mp_eo_stack && pd_sel) {
        //     eo_visible = pd_sel->editable_enabled ();
        // }
        // if (current_view () && eo_visible) {
        //     lay::EditorOptionsPages *eo_pages = current_view ()->editor_options_pages ();
        //     if (! eo_pages || ! eo_pages->has_content ()) {
        //         eo_visible = false;
        //     }
        // }

        // if (eo_visible != m_eo_visible) {
        //     m_eo_visible = eo_visible;
        //     show_dock_widget (mp_eo_dock_widget, m_eo_visible);
        // }
    }
}

void MainWindow::set_synchronous(bool sync_mode)
{
    m_synchronous = sync_mode;
    for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
    {
        (*vp)->set_synchronous(sync_mode);
    }
}

void MainWindow::update_window_title()
{
    if (current_view())
    {
        std::string sep = " - ";
        // if (current_view()->is_dirty())
        // {
        //     sep += "[+] ";
        // }
        setWindowTitle(geo::to_qstring(lay::ApplicationBase::version() + sep + current_view()->title()));
    }
    else
    {
        setWindowTitle(geo::to_qstring(lay::ApplicationBase::version()));
    }
}

void MainWindow::add_mru(const std::string& fn_rel)
{
    add_mru(fn_rel, m_initial_technology);
}

void MainWindow::add_mru(const std::string& fn_rel, const std::string& tech)
{
    // std::vector<std::pair<std::string, std::string>> new_mru(m_mru);
    std::string fn(geo::InputStream::absolute_path(fn_rel));

    for (auto mru = m_mru.begin(); mru != m_mru.end(); ++mru)
    {
        if (mru->first == fn)
        {
            m_mru.erase(mru);
            break;
        }
    }

    m_mru.push_back(std::make_pair(fn, tech));

    if (m_mru.size() > max_mru)
    {
        m_mru.erase(m_mru.begin());
    }

    // std::string config_str;
    // for (std::vector<std::pair<std::string, std::string>>::const_iterator mru = new_mru.begin();
    //      mru != new_mru.end(); ++mru)
    // {
    //     if (!config_str.empty())
    //     {
    //         config_str += " ";
    //     }
    //     config_str += geo::to_quoted_string(mru->first);
    //     if (!mru->second.empty())
    //     {
    //         config_str += "@";
    //         config_str += geo::to_quoted_string(mru->second);
    //     }
    // }

    // dispatcher()->config_set(cfg_mru, config_str);
    dm_do_update_mru_menus();
}

lay::LayoutView* MainWindow::current_view() const
{
    return lay::LayoutView::current();
}

int MainWindow::index_of(const lay::LayoutView* view) const
{
    for (int i = 0; i < int(mp_views.size()); ++i)
    {
        if (mp_views[i] == view)
        {
            return i;
        }
    }

    return -1;
}

void MainWindow::dock_widget_visibility_changed(bool visible)
{
    // if (sender () == mp_lp_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_layer_panel, geo::to_string
    //     (!mp_lp_dock_widget->isHidden ()));
    // } else if (sender () == mp_hp_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_hierarchy_panel, geo::to_string
    //     (!mp_hp_dock_widget->isHidden ()));
    // } else if (sender () == mp_libs_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_libraries_view, geo::to_string
    //     (!mp_libs_dock_widget->isHidden ()));
    // } else if (sender () == mp_bm_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_bookmarks_view, geo::to_string
    //     (!mp_bm_dock_widget->isHidden ()));
    // } else if (sender () == mp_navigator_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_navigator, geo::to_string
    //     (!mp_navigator_dock_widget->isHidden ()));
    // } else if (sender () == mp_layer_toolbox_dock_widget) {
    //     dispatcher ()->config_set (cfg_show_layer_toolbox, geo::to_string
    //     (!mp_layer_toolbox_dock_widget->isHidden ()));
    // } else if (sender () == mp_eo_dock_widget) {
    //     m_eo_visible = visible;
    // }
}

bool MainWindow::configure(const std::string& name, const std::string& value)
{

    // if (name == cfg_show_layer_panel) {
    //     bool flag;
    //     geo::from_string (value, flag);
    //     if (flag) {
    //         mp_lp_dock_widget->show ();
    //     } else {
    //         mp_lp_dock_widget->hide ();
    //     }
    //     return true;
    // }

    return false;
}

int MainWindow::create_view()
{
    //  create a new view
    // int view_index = do_create_view ();

    // //  add a new tab and make the new view the current one
    // mp_views.back ()->set_current ();

    // mp_layer_toolbox->set_view (current_view ());

    // mp_view_stack->add_widget (mp_views.back ());
    // mp_lp_stack->add_widget (mp_views.back ()->layer_control_frame ());
    // mp_hp_stack->add_widget (mp_views.back ()->hierarchy_control_frame ());
    // mp_libs_stack->add_widget (mp_views.back ()->libraries_frame ());
    // mp_eo_stack->add_widget (mp_views.back ()->editor_options_frame ());
    // mp_bm_stack->add_widget (mp_views.back ()->bookmarks_frame ());

    // bool f = m_disable_tab_selected;
    // m_disable_tab_selected = true;
    // int index = mp_tab_bar->insertTab (-1, geo::to_qstring (current_view ()->title ()));
    // m_disable_tab_selected = f;

    // view_created_event (index);
    // select_view (index);

    // update_dock_widget_state ();

    // return view_index;

    return -1;
}

void MainWindow::load_layer_properties(const std::string& fn, int cv_index, bool all_views, bool add_default)
{
    if (all_views)
    {
        for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
        {
            (*vp)->load_layer_props(fn, cv_index, add_default);
        }
    }
    else if (current_view())
    {
        current_view()->load_layer_props(fn, cv_index, add_default);
    }
}

void MainWindow::load_layer_properties(const std::string& fn, bool all_views, bool add_default)
{
    if (all_views)
    {
        for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
        {
            (*vp)->load_layer_props(fn, add_default);
        }
    }
    else if (current_view())
    {
        current_view()->load_layer_props(fn, add_default);
    }
}

lay::CellViewRef MainWindow::create_or_load_layout(const std::string* filename, const geo::LoadLayoutOptions* options,
                                                   const std::string& technology, int mode)
{
    lay::LayoutView* vw = 0;

    vw = view(do_create_view());

    unsigned int cv_index = 0;
    cv_index = vw->load_layout(*filename, *options, technology, mode == 2);

    mp_views.back()->set_current();
    mp_view_stack->add_widget(mp_views.back());

    mp_lp_stack->add_widget(mp_views.back()->layer_control_frame());
    mp_hp_stack->add_widget(mp_views.back()->hierarchy_control_frame());

    int index = mp_tab_bar->insertTab(-1, QString());
    update_tab_title(index);

    select_view(index);
    select_mode(vw->default_mode());

    emit file_open_signal(index);

    return vw->cellview_ref(cv_index);
}

lay::CellViewRef MainWindow::load_layout(const std::string& filename, const geo::LoadLayoutOptions& options,
                                         const std::string& technology, int mode)
{
    return create_or_load_layout(&filename, &options, technology, mode);
}

int MainWindow::do_create_view()
{
    //  create a new view
    // auto is_edited = lay::ApplicationBase::instance ()->is_editable ();
    bool is_edited{true};
    lay::LayoutView* view = new lay::LayoutView(&m_manager, is_edited, dispatcher(), mp_view_stack);
    add_view(view);

    //  set initial attributes
    // view->set_synchronous (synchronous ());

    int tl = 0;
    // dispatcher ()->config_get (cfg_initial_hier_depth, tl);
    // view->set_hier_levels (std::make_pair (0, tl));

    //  select the current mode and select the enabled editables
    // view->mode(m_mode);

    //  initialize the state stack
    view->clear_states();
    view->store_state();

    return int(mp_views.size() - 1);
}

void MainWindow::add_view(lay::LayoutView* view)
{
    // connect (view, &lay::LayoutView::title_changed, this, &MainWindow::update_window_title);
    // connect (view, SIGNAL (dirty_changed ()), this, SLOT (view_title_changed ()));
    // connect (view, SIGNAL (edits_enabled_changed ()), this, SLOT (edits_enabled_changed ()));
    // connect (view, SIGNAL (menu_needs_update ()), this, SLOT (menu_needs_update ()));
    // connect (view, SIGNAL (show_message (const std::string &, int)), this, SLOT (message (const
    // std::string &, int)));
    connect(view, &lay::LayoutView::current_pos_changed, this, &MainWindow::current_pos);
    view->cellview_changed_event.add(this, &MainWindow::on_cellview_changed);
    // connect (view, SIGNAL (clear_current_pos
    // ()), this, SLOT (clear_current_pos ()));
    // connect (view, SIGNAL (mode_change (int)), this, SLOT (select_mode (int)));

    // connect(view, &lay::LayoutView::file_open_signal, m_tree, [this](){
    // m_tree->addLayoutItem(mp_views.size()); }); view->file_open_event.add(m_tree, );
    mp_views.push_back(view);

    view->setGeometry(0, 0, mp_view_stack->width(), mp_view_stack->height());
    view->show();
}

lay::LayoutView* MainWindow::view(int index)
{
    if (index >= 0 && index < int(mp_views.size()))
    {
        return mp_views[index];
    }
    else
    {
        return 0;
    }
}

void MainWindow::update_tab_title(int i)
{
    std::string title;

    lay::LayoutView* v = view(i);
    if (v)
    {
        if (v->is_dirty())
        {
            title += "[+] ";
        }
        title += v->title();
    }

    if (geo::to_string(mp_tab_bar->tabText(i)) != title)
    {
        mp_tab_bar->setTabText(i, geo::to_qstring(title));
    }

    if (v)
    {
        std::string files;
        for (unsigned int cv = 0; cv < v->cellviews(); ++cv)
        {
            if (!files.empty())
            {
                files += "\n";
            }
            if (!v->cellview(cv)->filename().empty())
            {
                files += v->cellview(cv)->filename();
            }
            else
            {
                files += geo::to_string(tr("(not saved)"));
            }
        }
        if (geo::to_string(mp_tab_bar->tabToolTip(i)) != files)
        {
            mp_tab_bar->setTabToolTip(i, geo::to_qstring(files));
        }
    }
}

void MainWindow::restore_session(const std::string& fn)
{
    // m_current_session = fn;
    // lay::Session session;
    // session.load (fn);
    // session.restore (*this);
    // read_dock_widget_state ();
}

void MainWindow::do_update_mru_menus()
{
    std::string mru_menu = "file.open_recent";

    if (m_mru.size() > 0)
    {

        m_menu.clear_menu(mru_menu);

        for (auto mru = m_mru.end(); mru != m_mru.begin();)
        {
            --mru;
            size_t i = std::distance(m_mru.begin(), mru);
            Action* action = new OpenRecentAction(this, i);
            action->set_title(mru->first);
            m_menu.insert_item(mru_menu, action);
        }
    }
}

void MainWindow::about_to_exec()
{
}

bool MainWindow::is_single_cv_layer_properties_file(const std::string& fn)
{
    return false;
}

void MainWindow::tech_message(const std::string& s)
{
    // mp_tech_status_label->setText(geo::to_qstring (s));
}

bool MainWindow::update_progress(geo::Progress* progress)
{
    // if (mp_progress_dialog) {

    //     mp_progress_dialog->set_progress (progress);
    //     return true;

    // } else if (isVisible () && mp_progress_widget) {

    //     mp_progress_widget->set_progress (progress);
    //     return true;

    // } else {
    //     return false;
    // }

    return false;
}

bool MainWindow::show_progress_bar(bool show)
{
    // if (!isVisible ()) {

    //     mp_progress_dialog.reset (0);

    //     if (show) {
    //         QWidget *tl = QApplication::activeWindow ();
    //         if (tl && tl->isVisible ()) {
    //             mp_progress_dialog.reset (new ProgressDialog (tl, mp_pr));
    //             mp_progress_dialog->show ();
    //         }
    //         return true;
    //     } else {
    //         return false;
    //     }

    // } else {

    //     mp_main_stack_widget->setCurrentIndex (show ? 1 : 0);
    //     if (show) {
    //         clear_current_pos ();
    //     }
    //     return true;

    // }

    return false;
}

bool MainWindow::progress_wants_widget() const
{
    return true;
}

void MainWindow::progress_add_widget(QWidget* widget)
{
    // if (mp_progress_dialog) {
    //     mp_progress_dialog->add_widget (widget);
    // } else if (mp_progress_widget) {
    //     mp_progress_widget->add_widget (widget);
    // }
}

QWidget* MainWindow::progress_get_widget() const
{
    // if (mp_progress_dialog) {
    //     return mp_progress_dialog->get_widget ();
    // } else if ( mp_progress_widget) {
    //     return mp_progress_widget->get_widget ();
    // } else {
    //     return 0;
    // }

    return 0;
}

void MainWindow::current_pos(double x, double y, bool dbu_units)
{
    mp_cpx_label->setText(geo::to_qstring(dbu_units ? geo::db_to_string(x) : geo::micron_to_string(x)));
    mp_cpy_label->setText(geo::to_qstring(dbu_units ? geo::db_to_string(y) : geo::micron_to_string(y)));
}

void MainWindow::open_recent(size_t i)
{
    if (i >= m_mru.size())
    {
        return;
    }
    std::string fn(m_mru[i].first);
    std::string tech(m_mru[i].second);

    LOGI << "recent file: " << fn;

    if (check_exist_file(fn))
    {
        return;
    }

    load_layout(fn, tech, 1);
    add_mru(fn, tech);

    if (mp_command_panel)
    {
        mp_command_panel->echo("open " + CommandPanel::quote(fn));
    }
}

void MainWindow::menu_activated(const std::string& symbol)
{
    //  echo external operations into the command panel (unless the command
    //  originated from the panel itself)
    if (m_command_echo && mp_command_panel)
    {
        std::string cmd = CommandPanel::command_for_symbol(symbol);
        if (!cmd.empty())
        {
            mp_command_panel->echo(cmd);
        }
    }

    if (symbol == "cm_open")
    {
        cm_open();
    }
    else if (symbol == "cm_setup")
    {
        cm_setup();
    }
    else if (symbol == "cm_undo")
    {
        cm_undo();
    }
    else if (symbol == "cm_redo")
    {
        cm_redo();
    }
    else if (symbol == "cm_command_save_script")
    {
        if (mp_command_panel)
        {
            mp_command_panel->save_script_dialog();
        }
    }
    else if (symbol == "cm_command_load_script")
    {
        if (mp_command_panel)
        {
            mp_command_panel->load_script_dialog();
        }
    }
    else if (symbol == "cm_command_help")
    {
        if (mp_command_panel)
        {
            mp_command_panel->show_help();
        }
    }
    else if (symbol == "cm_command_visible")
    {
        if (mp_command_dock)
        {
            mp_command_dock->setVisible(!mp_command_dock->isVisible());
        }
    }
    else if (symbol == "cm_layers_visible")
    {
        if (mp_lp_dock_widget)
        {
            mp_lp_dock_widget->setVisible(!mp_lp_dock_widget->isVisible());
        }
    }
    else if (symbol == "cm_tree_visible")
    {
        if (m_tree_dock)
        {
            m_tree_dock->setVisible(!m_tree_dock->isVisible());
        }
    }
    else
    {
        for (auto cls = my::Registrar<lay::PluginDeclaration>::begin();cls != my::Registrar<lay::PluginDeclaration>::end(); ++cls)
        {
            if (cls->menu_activated(symbol))
            {
                return;
            }
        }
    }
}

void MainWindow::cm_undo()
{
    if (current_view() && m_manager.available_undo().first)
    {
        for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
        {
            (*vp)->clear_selection();
            (*vp)->cancel();
        }
        m_manager.undo();
    }
}

void MainWindow::cm_redo()
{
    if (current_view() && m_manager.available_redo().first)
    {
        for (std::vector<lay::LayoutView*>::iterator vp = mp_views.begin(); vp != mp_views.end(); ++vp)
        {
            (*vp)->clear_selection();
            (*vp)->cancel();
        }
        m_manager.redo();
    }
}

} // namespace lay
