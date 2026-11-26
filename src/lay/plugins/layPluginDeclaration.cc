#include "layPluginDeclaration.h"
#include "tlClassRegistry.h"
#include "detail/utilsString.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"

#include "layDispatcher.h"
#include "layGtf.h"
#include "layMenuEntry.h"

namespace lay{

// ----------------------------------------------------------------
//  PluginDeclaration implementation

static int s_next_id = 0;

PluginDeclaration::PluginDeclaration (int id)
    : m_id (id), m_editable_enabled (true)
{
    // .. nothing yet ..
}

PluginDeclaration::~PluginDeclaration ()
{
    if (Dispatcher::instance ()) {
        Dispatcher::instance ()->plugin_removed (this);
    }
}

void
PluginDeclaration::toggle_editable_enabled ()
{
    // BEGIN_PROTECTED
    //     set_editable_enabled (! editable_enabled ());
    // END_PROTECTED
}

std::vector<std::string>
PluginDeclaration::menu_symbols ()
{
    std::vector<std::string> symbols;

    for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {

        std::vector<lay::MenuEntry> menu_entries;
        cls->get_menu_entries (menu_entries);

        for (std::vector<lay::MenuEntry>::const_iterator m = menu_entries.begin (); m != menu_entries.end (); ++m) {
            if (! m->symbol.empty ()) {
                symbols.push_back (m->symbol);
            }
        }

    }

    std::sort (symbols.begin (), symbols.end ());
    symbols.erase (std::unique (symbols.begin (), symbols.end ()), symbols.end ());

    return symbols;
}

void
PluginDeclaration::init_menu (lay::Dispatcher *dispatcher)
{
    // lay::AbstractMenu &menu = *dispatcher->menu ();

    // mp_editable_mode_action.reset ((Action *) 0);
    // mp_mouse_mode_action.reset ((Action *) 0);

    // std::string title;

    //  make all plugins that return true on "implements_editable" into menu entries and
    //  set up the actions accordingly

    // if (implements_editable (title) && menu.is_valid ("edit_menu.select_menu")) {

    //     //  extract first part, which is the name, separated by a tab from the title.
    //     std::string name = geo::sprintf ("pi_enable_%d", id ());
    //     std::string t (title);
    //     const char *tab = strchr (t.c_str (), '\t');
    //     if (tab) {
    //         name = std::string (t, 0, tab - t.c_str ());
    //         title = tab + 1;
    //     }

    //     mp_editable_mode_action.reset (new Action (title));
    //     gtf::action_connect (mp_editable_mode_action->qaction (), SIGNAL (triggered ()), this, SLOT (toggle_editable_enabled ()));
    //     mp_editable_mode_action->set_checkable (true);
    //     mp_editable_mode_action->set_checked (m_editable_enabled);

    //     menu.insert_item ("edit_menu.select_menu.end", name, mp_editable_mode_action.get ());
    // }

    //  add all the custom menus from the plugins


    //  Fill the mode menu file items from the mouse modes

    std::vector<std::pair<std::string, std::pair<std::string, int> > > modes;

    // title = std::string ();
    // if (implements_mouse_mode (title)) {
    //     modes.push_back (std::make_pair (title, std::make_pair ("edit_menu.mode_menu.end;@toolbar.end_modes", id ())));
    // }

    //  the primary mouse modes (special for LayoutView)
    // implements_primary_mouse_modes (modes);

    // for (std::vector<std::pair<std::string, std::pair<std::string, int> > >::const_iterator m = modes.begin (); m != modes.end (); ++m) {

    //     //  extract first part, which is the name, separated by a tab from the title.
    //     std::string name;
    //     if (m->second.second <= 0) {
    //         name = geo::sprintf ("mode_i%d", 1 - m->second.second);
    //     } else {
    //         name = geo::sprintf ("mode_%d", m->second.second);
    //     }
    //     std::string title = m->first;

    //     const char *tab = strchr (title.c_str (), '\t');
    //     if (tab) {
    //         name = std::string (title, 0, tab - title.c_str ());
    //         title = std::string (tab + 1);
    //     }

    //     mp_mouse_mode_action.reset (new ModeAction (dispatcher, title, m->second.second));
    //     mp_mouse_mode_action->add_to_exclusive_group (&menu, "mouse_mode_exclusive_group");
    //     mp_mouse_mode_action->set_checkable (true);

    //     menu.insert_item (m->second.first, name + ":mode_group", mp_mouse_mode_action.get ());

    // }
}

void
PluginDeclaration::remove_menu_items (Dispatcher *dispatcher)
{
    // lay::AbstractMenu *menu = dispatcher->menu ();
    // menu->delete_items (mp_editable_mode_action.get ());
    // menu->delete_items (mp_mouse_mode_action.get ());

    // std::vector<lay::Action *> actions;
    // for (geo::weak_collection <lay::Action>::iterator a = m_menu_actions.begin (); a != m_menu_actions.end (); ++a) {
    //     if (a.operator-> ()) {
    //         actions.push_back (a.operator-> ());
    //     }
    // }
    // for (std::vector<lay::Action *>::const_iterator a = actions.begin (); a != actions.end (); ++a) {
    //     menu->delete_items (*a);
    // }
    // m_menu_actions.clear ();
}

void
PluginDeclaration::set_editable_enabled (bool f)
{
    // if (f != m_editable_enabled) {
    //     m_editable_enabled = f;
    //     if (mp_editable_mode_action.get ()) {
    //         mp_editable_mode_action->set_checked (f);
    //     }
    //     editable_enabled_changed_event ();
    // }
}

void
PluginDeclaration::register_plugin ()
{
    if (Dispatcher::instance ()) {
        Dispatcher::instance ()->plugin_registered (this);
        initialize (Dispatcher::instance ());
    }
}

}
