#include "laySelectPluginDecl.h"
#include "detail/utilsString.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"

#include "myRegistrar.h"
#include "layAbstractMenu.h"
#include "layConfigurationDialog.h"
#include "layConverters.h"
#include "layDispatcher.h"
#include "layMainWindow.h"
#include "layPlugin.h"
#include "laySelectPlugin.h"
#include "laySelectionConfigPage.h"
#include "laySelectConfig.h"

namespace lay
{

static SelectPluginDeclaration* sp_instance = 0;

// -----------------------------------------------------------------------------
//  SelectPluginDecl implementation

SelectPluginDeclaration::SelectPluginDeclaration(int id)
    : PluginDeclaration(id), m_current_template(0), m_current_template_updated(true),
      m_templates_updated(true)
{
    sp_instance = this;
}

SelectPluginDeclaration::~SelectPluginDeclaration()
{
    sp_instance = 0;
}

void SelectPluginDeclaration::create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup* actiongroup)
{
    Action* ac = new ModeAction(dispatcher, "Select", id());
    ac->set_icon(":/select.png");
    ac->set_checkable(true);

    toolbar->addAction(ac->qaction());
    actiongroup->addAction(ac->qaction());
}

SelectPluginDeclaration* SelectPluginDeclaration::instance()
{
    return sp_instance;
}

void SelectPluginDeclaration::get_options(
    std::vector<std::pair<std::string, std::string>>& options) const
{
    lay::ColorConverter cc;
    options.push_back (std::pair<std::string, std::string> (cfg_sel_color, cc.to_string (QColor ())));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_line_width, "1"));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_vertex_size, "3"));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_dither_pattern, "1"));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_halo, "true"));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_transient_mode, "true"));
    options.push_back (std::pair<std::string, std::string> (cfg_sel_inside_pcells_mode, "false"));
    options.push_back (std::pair<std::string, std::string> (cfg_search_range, "5"));
    options.push_back (std::pair<std::string, std::string> (cfg_search_range_box, "0"));
}

std::vector<std::pair<std::string, lay::ConfigPage*>>
SelectPluginDeclaration::config_pages(QWidget* parent) const
{
    std::vector<std::pair<std::string, lay::ConfigPage*>> pages;
    pages.push_back(std::make_pair(geo::to_string(QObject::tr("Application|Selection")),
                                   new LayoutViewConfigPage2c(parent)));
    return pages;
}

// void
// SelectPluginDeclaration::get_menu_entries (std::vector<lay::MenuEntry> &menu_entries) const
// {
// lay::SelectPluginDeclaration::get_menu_entries (menu_entries);
// menu_entries.push_back (lay::separator ("rulers_group", "edit_menu.end"));
// menu_entries.push_back (lay::menu_item ("ant::clear_all_rulers", "clear_all_rulers:edit",
// "edit_menu.end", geo::to_string (QObject::tr ("Clear All Rulers And Annotations(Ctrl+K)"))));
// menu_entries.push_back (lay::menu_item ("ant::configure", "configure_rulers", "edit_menu.end",
// geo::to_string (QObject::tr ("Ruler And Annotation Setup"))));
// }

lay::Plugin* SelectPluginDeclaration::create_plugin(geo::Manager*, lay::Dispatcher*,
                                                    lay::LayoutView* view, lay::ResourceTree*) const
{
    return new SelectionService(view);
}

// bool
// SelectPluginDeclaration::menu_activated (const std::string &symbol) const
// {
// if (symbol == "ant::configure") {

//   lay::ConfigurationDialog config_dialog (QApplication::activeWindow (),
//   lay::Dispatcher::instance (), "ant::Plugin"); config_dialog.exec ();

//   return true;

// } else {
//   return lay::SelectPluginDeclaration::menu_activated (symbol);
// }
// }

// bool
// SelectPluginDeclaration::implements_editable (std::string &title) const
// {
//   title = geo::to_string (QObject::tr ("Rulers And Annotations"));
//   return true;
// }

// bool
// SelectPluginDeclaration::implements_mouse_mode (std::string &title) const
// {
//   title = "ruler:ruler_mode_group:ruler_templates_group\t" + geo::to_string (QObject::tr
//   ("Ruler{Add rulers and annotations}")) + "<:ruler.png>"; return true;
// }

// bool
// SelectPluginDeclaration::configure (const std::string &name, const std::string &value)
// {
// if (name == cfg_ruler_templates) {

//   m_templates = ant::Template::from_string (value);
//   m_templates_updated = true;

// } else if (name == cfg_current_ruler_template) {

//   int n = 0;
//   geo::from_string (value, n);

//   if (n != m_current_template) {
//     m_current_template = n;
//     m_current_template_updated = true;
//   }

// }
// return false;
// }

// void
// SelectPluginDeclaration::config_finalize ()
// {
//   if (m_templates_updated) {

//     update_menu ();
//     m_templates_updated = false;
//     m_current_template_updated = false;

//   } else if (m_current_template_updated) {

//     update_current_template ();
//     m_current_template_updated = false;

//   }
// }

// void
// SelectPluginDeclaration::initialized (lay::Dispatcher *root)
// {
//  Check if we already have templates (initial setup)
// bool any_templates = false;
// for (std::vector<ant::Template>::iterator i = m_templates.begin (); ! any_templates && i !=
// m_templates.end (); ++i) {
//   any_templates = ! i->category ().empty ();
// }

// if (! any_templates) {

//   //  This is the migration path from <= 0.24 to 0.25: clear all templates unless we
//   //  have categorized ones there. Those can't be deleted, so we know we have a 0.25
//   //  setup if there are some
//   m_templates.clear ();

//   //  Set up the templates we want to see (plus some non-categorized templates)

//   m_templates.push_back (ant::Template (geo::to_string (QObject::tr ("Ruler")), "$X", "$Y", "$D",
//   ant::Object::STY_ruler, ant::Object::OL_diag, true, lay::AC_Global, "_ruler"));

//   m_templates.push_back (ant::Template (geo::to_string (QObject::tr ("Cross")), "", "", "$U,$V",
//   ant::Object::STY_cross_both, ant::Object::OL_diag, true, lay::AC_Global, "_cross"));
//   m_templates.back ().set_mode (ant::Template::RulerSingleClick);

//   m_templates.push_back (ant::Template (geo::to_string (QObject::tr ("Measure")), "$X", "$Y",
//   "$D", ant::Object::STY_ruler, ant::Object::OL_diag, true, lay::AC_Global, "_measure"));
//   m_templates.back ().set_mode (ant::Template::RulerAutoMetric);

//   m_templates.push_back (ant::Template (geo::to_string (QObject::tr ("Ellipse")), "W=$(abs(X))",
//   "H=$(abs(Y))", "", ant::Object::STY_line, ant::Object::OL_ellipse, true, lay::AC_Global,
//   std::string ()));

//   m_templates.push_back (ant::Template (geo::to_string (QObject::tr ("Box")), "W=$(abs(X))",
//   "H=$(abs(Y))", "", ant::Object::STY_line, ant::Object::OL_box, true, lay::AC_Global,
//   std::string ()));

//   root->config_set (cfg_ruler_templates, ant::TemplatesConverter ().to_string (m_templates));
//   root->config_end ();

// }
// }

// void
// SelectPluginDeclaration::uninitialize (lay::Dispatcher *)
// {
//   m_actions.clear ();
// }

// void
// SelectPluginDeclaration::update_current_template ()
// {
// lay::Dispatcher *mp = lay::Dispatcher::instance ();
// if (! mp || ! mp->has_ui ()) {
//   return;
// }

// if (m_current_template >= 0 && m_current_template < int (m_templates.size ())) {

//   std::vector<std::string> menu_entries = mp->menu ()->group ("ruler_mode_group");
//   for (std::vector<std::string>::const_iterator m = menu_entries.begin (); m != menu_entries.end
//   (); ++m) {
//     lay::Action *action = mp->menu ()->action (*m);
//     action->set_title (m_templates [m_current_template].title ());
//   }

//   if (m_templates.size () > 1) {

//     geo::weak_collection<lay::ConfigureAction>::iterator it = m_actions.begin ();
//     int index = 0;
//     for (std::vector<Template>::const_iterator tt = m_templates.begin (); tt != m_templates.end
//     () && it != m_actions.end (); ++tt, ++it, ++index) {
//       if (it.operator -> ()) {
//         it->set_checked (index == m_current_template);
//       }
//     }
//   }

// }
// }

// void
// SelectPluginDeclaration::update_menu ()
// {
// lay::Dispatcher *mp = lay::Dispatcher::instance ();
// if (! mp || ! mp->has_ui ()) {
//   return;
// }

// if (m_current_template < 0 || m_current_template >= int (m_templates.size ())) {
//   m_current_template = 0;
// }

// if (m_current_template >= 0 && m_current_template < int (m_templates.size ())) {
//   std::vector<std::string> menu_entries = mp->menu ()->group ("ruler_mode_group");
//   for (std::vector<std::string>::const_iterator m = menu_entries.begin (); m != menu_entries.end
//   (); ++m) {
//     lay::Action *action = mp->menu ()->action (*m);
//     action->set_title (m_templates [m_current_template].title ());
//   }
// }

// std::vector<std::string> tmpl_group = mp->menu ()->group ("ruler_templates_group");
// for (std::vector<std::string>::const_iterator t = tmpl_group.begin (); t != tmpl_group.end ();
// ++t) {
//   std::vector<std::string> items = mp->menu ()->items (*t);
//   for (std::vector<std::string>::const_iterator i = items.begin (); i != items.end (); ++i) {
//     mp->menu ()->delete_item (*i);
//   }
// }

// m_actions.clear ();

// if (m_templates.size () > 1) {
//   int it = 0;
//   for (std::vector<Template>::const_iterator tt = m_templates.begin (); tt != m_templates.end ();
//   ++tt, ++it) {
//     lay::ConfigureAction *action = new lay::ConfigureAction (tt->title (),
//     cfg_current_ruler_template, geo::to_string (it)); m_actions.push_back (action);
//     action->set_checkable (true);
//     action->set_checked (it == m_current_template);
//     for (std::vector<std::string>::const_iterator t = tmpl_group.begin (); t != tmpl_group.end
//     (); ++t) {
//       mp->menu ()->insert_item (*t + ".end", "ruler_template_" + geo::to_string (it), action);
//     }
//   }
// }

// }

// void
// SelectPluginDeclaration::register_annotation_template (/*const ant::Template &t*/)
// {
// if (t.category ().empty ()) {
//   return;
// }

// for (std::vector<ant::Template>::iterator i = m_templates.begin (); i != m_templates.end (); ++i)
// {
//   if (i->category () == t.category ()) {
//     return;
//   }
// }

// m_templates.push_back (t);
// lay::Dispatcher::instance ()->config_set (cfg_ruler_templates, ant::TemplatesConverter
// ().to_string (m_templates)); lay::Dispatcher::instance ()->config_end ();
// }

} // namespace lay

static my::Registrar<lay::PluginDeclaration> select_plugin(new lay::SelectPluginDeclaration(0), 100, "select");
