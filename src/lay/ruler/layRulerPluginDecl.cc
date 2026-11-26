#include "layRulerPluginDecl.h"
#include "detail/utilsString.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"

#include "layAbstractMenu.h"
#include "layConfigurationDialog.h"
#include "layConverters.h"
#include "layDispatcher.h"
#include "layPlugin.h"
#include "layRulerConfig.h"
#include "layRulerConfigPage.h"
#include "layMainWindow.h"
#include "layMenuEntry.h"
#include "layRulerPlugin.h"
#include "layLayoutView.h"

#include "myRegistrar.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QToolButton>

namespace lay
{

static RulerPluginDeclaration* sp_instance = 0;

// -----------------------------------------------------------------------------
//  RulerPluginDecl implementation

RulerPluginDeclaration::RulerPluginDeclaration(int id)
    : PluginDeclaration(id), m_current_template(0), m_ruler_mode(0), m_current_template_updated(true),
      m_templates_updated(true)
{
    sp_instance = this;
}

RulerPluginDeclaration::~RulerPluginDeclaration()
{
    sp_instance = 0;
}

RulerPluginDeclaration* RulerPluginDeclaration::instance()
{
    return sp_instance;
}

void RulerPluginDeclaration::get_options(
    std::vector<std::pair<std::string, std::string>>& options) const
{
    // options.push_back(std::pair<std::string, std::string>(cfg_max_number_of_rulers, "-1"));
    // options.push_back(std::pair<std::string, std::string>(cfg_ruler_snap_range, "8"));
    // options.push_back(std::pair<std::string, std::string>(
    //     cfg_ruler_color, lay::ColorConverter().to_string(QColor())));
    // options.push_back(std::pair<std::string, std::string>(cfg_ruler_halo, "true"));
    // options.push_back(std::pair<std::string, std::string>(cfg_ruler_snap_mode,
    //                                                       ACConverter().to_string(lay::AC_Any)));
    // options.push_back(std::pair<std::string, std::string>(cfg_ruler_obj_snap,
    // geo::to_string(true))); options.push_back(
    //     std::pair<std::string, std::string>(cfg_ruler_grid_snap, geo::to_string(false)));
    // options.push_back(std::pair<std::string, std::string>(cfg_ruler_templates, ""));
    // options.push_back(std::pair<std::string, std::string>(cfg_current_ruler_template, "0"));
    //  grid-micron is not configured here since some other entity is supposed to do this.
}

std::vector<std::pair<std::string, lay::ConfigPage*>>
RulerPluginDeclaration::config_pages(QWidget* parent) const
{
    std::vector<std::pair<std::string, lay::ConfigPage*>> pages;
    pages.push_back (std::make_pair ("Rulers And Annotations|Snapping", new RulerConfigPage (parent)));
    pages.push_back (std::make_pair("Rulers And Annotations|Appearance", new RulerConfigPage2(parent)));
    pages.push_back (std::make_pair ("Rulers And Annotations|Angle", new RulerConfigPage3 (parent)));
    pages.push_back (std::make_pair("Rulers And Annotations|Templates", new RulerConfigPage4(parent)));
    return pages;
}

void RulerPluginDeclaration::get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const
{
    // lay::RulerPluginDeclaration::get_menu_entries(menu_entries);
    // menu_entries.push_back(lay::separator("rulers_group", "edit_menu.end"));
    // menu_entries.push_back(
    //     lay::menu_item("ant::clear_all_rulers", "clear_all_rulers:edit", "edit_menu.end",
    //                    geo::to_string(QObject::tr("Clear All Rulers And Annotations(Ctrl+K)"))));
    // menu_entries.push_back(
    //     lay::menu_item("ant::configure", "configure_rulers", "edit_menu.end",
    //                    geo::to_string(QObject::tr("Ruler And Annotation Setup"))));
}

lay::Plugin* RulerPluginDeclaration::create_plugin(geo::Manager* manager, lay::Dispatcher* ,
                                                   lay::LayoutView* view, lay::ResourceTree* tree) const
{
    return new RulerPlugin(manager, view, tree);
}

bool RulerPluginDeclaration::menu_activated(const std::string& symbol) const
{
    // if (symbol == "ant::configure")
    // {

    //     lay::ConfigurationDialog config_dialog(QApplication::activeWindow(),
    //                                            lay::Dispatcher::instance(), "ant::Plugin");
    //     config_dialog.exec();

    //     return true;
    // }
    // else
    // {
    //     return lay::RulerPluginDeclaration::menu_activated(symbol);
    // }

    return false;
}

bool RulerPluginDeclaration::implements_editable(std::string& title) const
{
    // title = geo::to_string(QObject::tr("Rulers And Annotations"));
    return true;
}

bool RulerPluginDeclaration::implements_mouse_mode(std::string& title) const
{
    // title = "ruler:ruler_mode_group:ruler_templates_group\t" +
    //         geo::to_string(QObject::tr("Ruler{Add rulers and annotations}")) + "<:ruler.png>";
    // return true;

    return false;
}

bool RulerPluginDeclaration::configure(const std::string& name, const std::string& value)
{
    // if (name == cfg_ruler_templates)
    // {

    //     m_templates = ant::Template::from_string(value);
    //     m_templates_updated = true;
    // }
    // else if (name == cfg_current_ruler_template)
    // {

    //     int n = 0;
    //     geo::from_string(value, n);

    //     if (n != m_current_template)
    //     {
    //         m_current_template = n;
    //         m_current_template_updated = true;
    //     }
    // }
    // return false;

    return false;
}

void RulerPluginDeclaration::config_finalize()
{
    // if (m_templates_updated)
    // {

    //     update_menu();
    //     m_templates_updated = false;
    //     m_current_template_updated = false;
    // }
    // else if (m_current_template_updated)
    // {

    //     update_current_template();
    //     m_current_template_updated = false;
    // }
}

void RulerPluginDeclaration::initialized(lay::Dispatcher* root)
{
    //  Check if we already have templates (initial setup)
    // bool any_templates = false;
    // for (std::vector<ant::Template>::iterator i = m_templates.begin();
    //      !any_templates && i != m_templates.end(); ++i)
    // {
    //     any_templates = !i->category().empty();
    // }

    // if (!any_templates)
    // {

    //     //  This is the migration path from <= 0.24 to 0.25: clear all templates unless we
    //     //  have categorized ones there. Those can't be deleted, so we know we have a 0.25
    //     //  setup if there are some
    //     m_templates.clear();

    //     //  Set up the templates we want to see (plus some non-categorized templates)

    //     m_templates.push_back(ant::Template(geo::to_string(QObject::tr("Ruler")), "$X", "$Y",
    //     "$D",
    //                                         ant::Object::STY_ruler, ant::Object::OL_diag, true,
    //                                         lay::AC_Global, "_ruler"));

    //     m_templates.push_back(ant::Template(geo::to_string(QObject::tr("Cross")), "", "", "$U,$V",
    //                                         ant::Object::STY_cross_both, ant::Object::OL_diag,
    //                                         true, lay::AC_Global, "_cross"));
    //     m_templates.back().set_mode(ant::Template::RulerSingleClick);

    //     m_templates.push_back(ant::Template(geo::to_string(QObject::tr("Measure")), "$X", "$Y",
    //     "$D",
    //                                         ant::Object::STY_ruler, ant::Object::OL_diag, true,
    //                                         lay::AC_Global, "_measure"));
    //     m_templates.back().set_mode(ant::Template::RulerAutoMetric);

    //     m_templates.push_back(ant::Template(
    //         geo::to_string(QObject::tr("Ellipse")), "W=$(abs(X))", "H=$(abs(Y))", "",
    //         ant::Object::STY_line, ant::Object::OL_ellipse, true, lay::AC_Global,
    //         std::string()));

    //     m_templates.push_back(ant::Template(
    //         geo::to_string(QObject::tr("Box")), "W=$(abs(X))", "H=$(abs(Y))", "",
    //         ant::Object::STY_line, ant::Object::OL_box, true, lay::AC_Global, std::string()));

    //     root->config_set(cfg_ruler_templates, ant::TemplatesConverter().to_string(m_templates));
    //     root->config_end();
    // }
}

void RulerPluginDeclaration::uninitialize(lay::Dispatcher*)
{
    // m_actions.clear();
}

void RulerPluginDeclaration::update_current_template()
{
    // lay::Dispatcher* mp = lay::Dispatcher::instance();
    // if (!mp || !mp->has_ui())
    // {
    //     return;
    // }

    // if (m_current_template >= 0 && m_current_template < int(m_templates.size()))
    // {

    //     std::vector<std::string> menu_entries = mp->menu()->group("ruler_mode_group");
    //     for (std::vector<std::string>::const_iterator m = menu_entries.begin();
    //          m != menu_entries.end(); ++m)
    //     {
    //         lay::Action* action = mp->menu()->action(*m);
    //         action->set_title(m_templates[m_current_template].title());
    //     }

    //     if (m_templates.size() > 1)
    //     {

    //         geo::weak_collection<lay::ConfigureAction>::iterator it = m_actions.begin();
    //         int index = 0;
    //         for (std::vector<Template>::const_iterator tt = m_templates.begin();
    //              tt != m_templates.end() && it != m_actions.end(); ++tt, ++it, ++index)
    //         {
    //             if (it.operator->())
    //             {
    //                 it->set_checked(index == m_current_template);
    //             }
    //         }
    //     }
    // }
}

void RulerPluginDeclaration::update_menu()
{
    // lay::Dispatcher* mp = lay::Dispatcher::instance();
    // if (!mp || !mp->has_ui())
    // {
    //     return;
    // }

    // if (m_current_template < 0 || m_current_template >= int(m_templates.size()))
    // {
    //     m_current_template = 0;
    // }

    // if (m_current_template >= 0 && m_current_template < int(m_templates.size()))
    // {
    //     std::vector<std::string> menu_entries = mp->menu()->group("ruler_mode_group");
    //     for (std::vector<std::string>::const_iterator m = menu_entries.begin();
    //          m != menu_entries.end(); ++m)
    //     {
    //         lay::Action* action = mp->menu()->action(*m);
    //         action->set_title(m_templates[m_current_template].title());
    //     }
    // }

    // std::vector<std::string> tmpl_group = mp->menu()->group("ruler_templates_group");
    // for (std::vector<std::string>::const_iterator t = tmpl_group.begin(); t != tmpl_group.end();
    //      ++t)
    // {
    //     std::vector<std::string> items = mp->menu()->items(*t);
    //     for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
    //     {
    //         mp->menu()->delete_item(*i);
    //     }
    // }

    // m_actions.clear();

    // if (m_templates.size() > 1)
    // {
    //     int it = 0;
    //     for (std::vector<Template>::const_iterator tt = m_templates.begin();
    //          tt != m_templates.end(); ++tt, ++it)
    //     {
    //         lay::ConfigureAction* action = new lay::ConfigureAction(
    //             tt->title(), cfg_current_ruler_template, geo::to_string(it));
    //         m_actions.push_back(action);
    //         action->set_checkable(true);
    //         action->set_checked(it == m_current_template);
    //         for (std::vector<std::string>::const_iterator t = tmpl_group.begin();
    //              t != tmpl_group.end(); ++t)
    //         {
    //             mp->menu()->insert_item(*t + ".end", "ruler_template_" + geo::to_string(it),
    //             action);
    //         }
    //     }
    // }
}

void RulerPluginDeclaration::register_annotation_template(const lay::Template& t)
{
    // if (t.category().empty())
    // {
    //     return;
    // }

    // for (std::vector<ant::Template>::iterator i = m_templates.begin(); i != m_templates.end();
    // ++i)
    // {
    //     if (i->category() == t.category())
    //     {
    //         return;
    //     }
    // }

    // m_templates.push_back(t);
    // lay::Dispatcher::instance()->config_set(cfg_ruler_templates,
    //                                         ant::TemplatesConverter().to_string(m_templates));
    // lay::Dispatcher::instance()->config_end();
}

void RulerPluginDeclaration::create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup* actiongroup)
{
    Action* ac = new ModeAction(dispatcher, "Ruler", id());
    ac->set_icon(":/ruler.png");
    ac->set_checkable(true);

    toolbar->addAction(ac->qaction());
    actiongroup->addAction(ac->qaction());

    //  ruler creation mode dropdown, attached directly onto the tool button
    QMenu* mode_menu = new QMenu(toolbar);
    mode_menu->setObjectName(QString::fromUtf8("ruler_mode_menu"));

    QAction* common_action = mode_menu->addAction(QObject::tr("Common Ruler"));
    common_action->setCheckable(true);
    common_action->setChecked(m_ruler_mode == 0);
    common_action->setToolTip(QObject::tr("Drag on the layout to create a ruler"));

    QAction* measure_action = mode_menu->addAction(QObject::tr("Measure"));
    measure_action->setCheckable(true);
    measure_action->setChecked(m_ruler_mode == 1);
    measure_action->setToolTip(QObject::tr("Double-click on two points to auto-measure"));

    auto apply_mode = [this, ac, common_action, measure_action](int mode) {
        set_ruler_mode(mode);
        common_action->setChecked(mode == 0);
        measure_action->setChecked(mode == 1);
        //  activate the ruler tool (checks the button in the exclusive group)
        ac->qaction()->trigger();
    };

    connect(common_action, &QAction::triggered, [apply_mode]() { apply_mode(0); });
    connect(measure_action, &QAction::triggered, [apply_mode]() { apply_mode(1); });

    //  the dropdown is attached onto the tool button
    ac->qaction()->setMenu(mode_menu);
    ac->qaction()->setToolTip(QObject::tr("Ruler - click to choose mode (Common Ruler: drag to create / Measure: double-click to auto-measure)"));

    //  clicking the whole button opens the dropdown (InstantPopup); the mode is then
    //  picked from the menu, which also activates the ruler tool
    if (QToolButton* tb = qobject_cast<QToolButton*>(toolbar->widgetForAction(ac->qaction())))
    {
        tb->setPopupMode(QToolButton::InstantPopup);
    }
}

void RulerPluginDeclaration::set_ruler_mode(int mode)
{
    m_ruler_mode = mode;

    //  apply the mode to all currently open views
    lay::MainWindow* mw = lay::MainWindow::instance();
    if (mw)
    {
        for (unsigned int i = 0; i < mw->views(); ++i)
        {
            lay::LayoutView* v = mw->view(int(i));
            if (v)
            {
                lay::RulerPlugin* rp = v->get_plugin<lay::RulerPlugin>();
                if (rp)
                {
                    rp->set_mode(mode == 0 ? lay::RulerPlugin::CommonMode : lay::RulerPlugin::MeasureMode);
                }
            }
        }
    }
}

} // namespace lay

static my::Registrar<lay::PluginDeclaration> ruler_plugin(new lay::RulerPluginDeclaration(1), 200, "ruler");
