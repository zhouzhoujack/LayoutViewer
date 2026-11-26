#include "layGotoPluginDecl.h"
#include "detail/utilsString.h"

#include "myRegistrar.h"
#include "layAbstractMenu.h"
#include "layDispatcher.h"
#include "layGotoPlugin.h"
#include "layMainWindow.h"
#include "layMenuEntry.h"
#include "layPlugin.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QShortcut>
#include <QToolButton>

namespace lay
{

static GotoPluginDeclaration* sp_instance = 0;

// -----------------------------------------------------------------------------
//  GotoPluginDecl implementation

GotoPluginDeclaration::GotoPluginDeclaration(int id) : PluginDeclaration(id) { sp_instance = this; }

GotoPluginDeclaration::~GotoPluginDeclaration() { sp_instance = 0; }

GotoPluginDeclaration* GotoPluginDeclaration::instance() { return sp_instance; }

void GotoPluginDeclaration::set_pick_mode_checked(bool on)
{
    if (mp_pick_action)
    {
        mp_pick_action->setChecked(on);
    }
}

void GotoPluginDeclaration::create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup* actiongroup)
{
    //  the "pick position" mode action (member of the exclusive mode button group,
    //  so activating it unchecks the other tools and vice versa)
    QAction* pick_action = new QAction(QIcon(":/find.png"), QObject::tr("Pick Position on Canvas"), toolbar);
    mp_pick_action = pick_action;
    pick_action->setCheckable(true);
    pick_action->setToolTip(QObject::tr("Click on the layout view to jump to the clicked position"));
    connect(pick_action, &QAction::triggered, [dispatcher, this, pick_action]() {
        //  checked -> enter the goto pick mode, unchecked -> go back to select
        dispatcher->select_mode(pick_action->isChecked() ? id() : 0);
    });
    if (actiongroup)
    {
        actiongroup->addAction(pick_action);
    }

    //  the "Go To Position..." dialog action
    QAction* dialog_action = new QAction(QIcon(":/find.png"), QObject::tr("Go To Position..."), toolbar);
    dialog_action->setToolTip(QObject::tr("Enter the position in a dialog (Ctrl+G)"));
    connect(dialog_action, &QAction::triggered, [dispatcher]() {
        dispatcher->menu_activated("goto::goto_position");
    });

    //  the dropdown menu of the toolbar button
    QMenu* menu = new QMenu(toolbar);
    menu->setObjectName(QString::fromUtf8("goto_menu"));
    menu->addAction(dialog_action);
    menu->addAction(pick_action);

    //  the toolbar button: clicking it shows the dropdown menu above
    QToolButton* btn = new QToolButton(toolbar);
    btn->setObjectName(QString::fromUtf8("goto_tool_button"));
    btn->setIcon(QIcon(":/find.png"));
    btn->setText(QObject::tr("Goto"));
    btn->setCheckable(true);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setToolTip(QObject::tr("Go To Position (Ctrl+G)"));
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setMenu(menu);
    toolbar->addWidget(btn);

    //  keep the button's active state in sync with the pick mode action
    connect(pick_action, &QAction::toggled, btn, [btn](bool on) {
        btn->setChecked(on);
    });

    //  Ctrl+G opens the position dialog directly
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+G"), toolbar);
    shortcut->setObjectName(QString::fromUtf8("goto_shortcut"));
    connect(shortcut, &QShortcut::activated, [dispatcher]() {
        dispatcher->menu_activated("goto::goto_position");
    });
}

lay::Plugin* GotoPluginDeclaration::create_plugin(geo::Manager*, lay::Dispatcher*, lay::LayoutView* view,
                                                  lay::ResourceTree* tree) const
{
    return new GotoPlugin(view, tree);
}

bool GotoPluginDeclaration::implements_mouse_mode(std::string& title) const
{
    title = "goto:goto_mode_group\t" + geo::to_string(QObject::tr("Goto{Move the view to the clicked position}")) +
            "<:find.png>";
    return true;
}

void GotoPluginDeclaration::get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const
{
    //  "Go To Position" 已改为工具栏按钮（见 create_toobar_action），不再创建菜单项。
    (void)menu_entries;
}

bool GotoPluginDeclaration::menu_activated(const std::string& symbol) const
{
    if (symbol == "goto::goto_position" || symbol == "goto::clear_marker")
    {
        lay::MainWindow* mw = lay::MainWindow::instance();
        if (mw && mw->current_view())
        {
            lay::GotoPlugin* goto_plugin = mw->current_view()->get_plugin<lay::GotoPlugin>();
            if (goto_plugin)
            {
                if (symbol == "goto::goto_position")
                {
                    goto_plugin->goto_position_dialog(mw);
                }
                else
                {
                    goto_plugin->remove_all_gotos();
                }
            }
        }
        return true;
    }
    
    return false;
}

} // namespace lay

// static my::Registrar<lay::PluginDeclaration> goto_plugin_decl(new lay::GotoPluginDeclaration(2), 150, "goto");
