#ifndef HDR_layAbstractMenu
#define HDR_layAbstractMenu

#include <QKeySequence>
#include "detail/utilsObject.h"
#include "detail/utilsString.h"
#include <QtWidgets/QShortcut>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QObject>

#include <string>
#include <set>
#include <map>
#include <vector>
#include <list>

#include "gsi.h"
#include "gsiObject.h"
#include "tlStableVector.h"
#include "tlObject.h"
#include "layDispatcher.h"

#include "tlTypeTraits.h"
#include "tlObject.h"
#include "layPlugin.h"
#include "layAbstractMenu.h"
#include "layPluginDeclaration.h"


class QFrame;
class QMenuBar;
class QToolBar;
class QMenu;

namespace lay
{

class AbstractMenu;
class Dispatcher;

/**
 *  @brief A utility function to convert the packed key binding in the cfg_key_bindings string to a vector
 */
std::vector<std::pair<std::string, std::string> > unpack_key_binding (const std::string &packed);

/**
 *  @brief A utility function to convert the key binding (as path/shortcut pair vector) to a packed string for cfg_key_bindings
 */
std::string pack_key_binding (const std::vector<std::pair<std::string, std::string> > &unpacked);

/**
 *  @brief A utility function to convert the packed hidden flags in the cfg_menu_items_hidden string to a vector
 */
std::vector<std::pair<std::string, bool> > unpack_menu_items_hidden (const std::string &packed);

/**
 *  @brief A utility function to convert the hidden flags (as path/bool pair vector) to a packed string for cfg_menu_items_hidden
 */
std::string pack_menu_items_hidden (const std::vector<std::pair<std::string, bool> > &unpacked);

/**
 *  @brief The basic Action object
 *
 *  An "action" is the target of a menu action. It allows reimplementing the "triggered" method
 *  to implement a specific action. The action encapsulates a QAction object.
 *
 *  Use this object in "insert_item" of AbstractMenu.
 *
 *  This object is typically owned by the AbstractMenu and cannot be copied or assigned.
 */
class Action
  : public QObject,
    public geo::RefCounted,
    public gsi::ObjectBase
{
Q_OBJECT

public:
  /**
   *  @brief The main constructor
   */
  Action ();

  /**
   *  @brief Creates an action from the given QAction
   *  If "owned" is true, the QAction will become owned by the Action object.
   */
  Action (QAction *action, bool owned = true);

  /**
   *  @brief Creates an action from the given QMenu
   *  If "owned" is true, the QAction will become owned by the Action object.
   */
  Action (QMenu *menu, bool owned = true);

  /**
   *  @brief Creates an action with the given title (icon, keyboard shortcut)
   *
   *  The title will optionally encode the shortcut and/or icon resource.
   *  The format of the title string is: <text>["("shortcut")"]["<"icon-resource">"]
   *
   *  @param title The title string encoding icon and keyboard shortcut if applicable.
   */
  Action (const std::string &title);

  /**
   *  @brief Destructor
   */
  ~Action ();

  /**
   *  @brief Set the title
   */
  void set_title (const std::string &t);

  /**
   *  @brief Get the title
   */
  std::string get_title () const;

  /**
   *  @brief Gets the shortcut string for "no shortcut present"
   */
  static const std::string &no_shortcut ();

  /**
   *  @brief Sets the keyboard shortcut
   *  If no shortcut is set (empty string), the default shortcut will be taken.
   *  If the shortcut string is "no_shortcut()", no shortcut will be assigned to the item.
   */
  void set_shortcut (const std::string &s);

  /**
   *  @brief Gets the keyboard shortcut
   *  To get the effective shortcut (combination of default shortcut and shortcut),
   *  use "get_effective_shortcut".
   */
  std::string get_shortcut () const;

  /**
   *  @brief Sets the default keyboard shortcut
   *  This shortcut is used when no specific shortcut is set.
   */
  void set_default_shortcut (const std::string &s);

  /**
   *  @brief Gets the default keyboard shortcut
   *  To get the effective shortcut (combination of default shortcut and shortcut),
   *  use "get_effective_shortcut".
   */
  std::string get_default_shortcut () const;

  /**
   *  @brief Gets the effective shortcut
   */
  std::string get_effective_shortcut () const;

  /**
   *  @brief Gets the effective shortcut for a given key sequence string
   */
  std::string get_effective_shortcut_for (const std::string &sc) const;

  /**
   *  @brief "is_checkable" attribute
   */
  bool is_checkable () const;

  /**
   *  @brief "is_checked" attribute
   */
  bool is_checked () const;

  /**
   *  @brief "is_enabled" attribute
   */
  bool is_enabled () const;

  /**
   *  @brief "is_visible" attribute
   */
  bool is_visible () const;

  /**
   *  @brief Gets a value indicating whether the action is intentionally hidden
   *  This flag combines with the visibility. "is_effective_visible" is false
   *  if hidden is true. This feature allows implementation of the menu configuration
   *  feature where users can deliberately switch off and on menu items.
   */
  bool is_hidden () const;

  /**
   *  @brief Gets the effective visibility
   *  See "is_hidden" for details.
   */
  bool is_effective_visible () const;

  /**
   *  @brief "is_separator" attribute
   */
  bool is_separator () const;

  /**
   *  @brief Enable or disable the action
   */
  void set_enabled (bool b);

  /**
   *  @brief Show or hide
   */
  void set_visible (bool v);

  /**
   *  @brief Sets a value indicating whether the menu item is hidden
   *  See "is_hidden" for details.
   */
  void set_hidden (bool h);

  /**
   *  @brief Make checkable or not
   */
  void set_checkable (bool c);

  /**
   *  @brief Check or uncheck
   */
  void set_checked (bool c);

  /**
   *  @brief Make a separator (or not)
   */
  void set_separator (bool s);

  /**
   *  @brief Set the tool tip text
   *
   *  @param text The text to display in the tool tip
   */
  void set_tool_tip (const std::string &text);

  /**
   *  @brief Set the icon to the given picture
   *
   *  @param file The file to load for the icon
   *
   *  Passing an empty string will reset the icon
   */
  void set_icon (const std::string &filename);

  /**
   *  @brief Set the icon's text
   *
   *  If an icon text is set, this will be used for the text below the icon.
   *  If no icon text is set, the normal text will be used for the icon.
   *  Passing an empty string will reset the icon's text.
   */
  void set_icon_text (const std::string &icon_text);

  /**
   *  @brief Set the action's QObject name (for testing for example)
   */
  void set_object_name (const std::string &name);

  /**
   *  @brief Makes this action part of the exclusive group with the given name
   */
  void add_to_exclusive_group (lay::AbstractMenu *menu, const std::string &group_name);

  /**
   *  @brief Get the tool tip text
   */
  std::string get_tool_tip () const;

  /**
   *  @brief Get the icon's text
   */
  std::string get_icon_text () const;

  /**
   *  @brief Trigger the action
   */
  void trigger ();

  /**
   *  @brief Reimplement this method to implement a trigger handler
   */
  virtual void triggered ();

  /**
   *  @brief Returns true, if the action is associated with a specific mode ID
   */
  virtual bool is_for_mode (int /*mode_id*/) const
  {
    return false;
  }

  /**
   *  @brief Get the underlying QAction object
   *
   *  Hint: do not use for changing the shortcut!
   */
  QAction *qaction () const;

  /**
   *  @brief Gets the QMenu object if the action is a menu action
   */
  QMenu *menu () const;

  /**
   *  @brief Gets the dispatcher object this action is connected to
   */
  Dispatcher *dispatcher () const
  {
    return mp_dispatcher;
  }

protected slots:
  void destroyed (QObject *obj);
  void qaction_triggered ();

private:
  friend struct AbstractMenuItem;

  QMenu *mp_menu;
  QAction *mp_action;
  lay::Dispatcher *mp_dispatcher;
  bool m_owned;
  bool m_visible;
  bool m_hidden;
  std::string m_default_shortcut;
  QKeySequence m_default_key_sequence;
  std::string m_shortcut;
  std::string m_symbol;
  QKeySequence m_key_sequence;
  bool m_no_key_sequence;

  void set_dispatcher (Dispatcher *dispatcher);
  QKeySequence get_key_sequence () const;
  QKeySequence get_key_sequence_for (const std::string &sc) const;

  //  no copying
  Action (const Action &);
  Action &operator= (const Action &);
};


class ConfigureAction
    : public Action
{
public:
    enum type { setter_type = 0, boolean_type = 1, choice_type = 2 };

    /**
   *  @brief The default constructor
   */
    ConfigureAction ();

    /**
   *  @brief Create an configure action with the given name and value
   *
   *  @param cname The name of the configuration parameter to set
   *  @param cvalue The value to set "cname" to
   *
   *  The value can be "?" in which case the configuration action describes
   *  a boolean parameter which is mapped to a checkable action.
   */
    ConfigureAction (const std::string &cname, const std::string &value);

    /**
   *  @brief Create an configure action with the given title (icon, keyboard shortcut), name and value
   *
   *  The format of the title string is: <text>["("shortcut")"]["<"icon-resource">"]
   *
   *  @param title The title string encoding icon and keyboard shortcut if applicable.
   *  @param cname The name of the configuration parameter to set
   *  @param cvalue The value to set "cname" to
   *
   *  The value can be "?" in which case the configuration action describes
   *  a boolean parameter which is mapped to a checkable action.
   */
    ConfigureAction (const std::string &title, const std::string &cname, const std::string &value);

    /**
   *  @brief Destructor
   */
    ~ConfigureAction ();

    /**
   *  @brief Configuration parameter name setter
   */
    void set_cname (const std::string &cname)
    {
        m_cname = cname;
    }

    /**
   *  @brief Configuration parameter name getter
   */
    const std::string &get_cname () const
    {
        return m_cname;
    }

    /**
   *  @brief Configuration parameter value setter
   */
    void set_cvalue (const std::string &cvalue)
    {
        m_cvalue = cvalue;
    }

    /**
   *  @brief Configuration parameter value getter
   */
    const std::string &get_cvalue () const
    {
        return m_cvalue;
    }

    /**
   *  @brief Transfer the value into this action
   *
   *  This method is called by the main window when the respective configuration changes.
   */
    void configure (const std::string &value);

protected:
    virtual void triggered ();

private:
    ConfigureAction (const ConfigureAction &action);
    ConfigureAction &operator= (const ConfigureAction &action);

    std::string m_cname, m_cvalue;
    type m_type;
};


/**
 *  @brief One item in the abstract menu
 */
struct AbstractMenuItem
{
  AbstractMenuItem (lay::Dispatcher *dispatcher);

  //  No copy constructor semantics because we don't need it (we use list's) and Action does not provide one.
  AbstractMenuItem (const AbstractMenuItem &);

  /**
   *  @brief Internal method used to set up the item
   */
  void setup_item (const std::string &pn, const std::string &n, Action *a);

  Dispatcher *dispatcher () const
  {
    return mp_dispatcher;
  }

  const std::string &name () const 
  {
    return m_name;    
  }

  const std::set<std::string> &groups () const
  {
    return m_groups;
  }

  void set_action (Action *a, bool copy_properties);

  void set_action_title (const std::string &t);

  Action *action ()
  {
    return mp_action.get ();
  }

  const Action *action () const
  {
    return mp_action.get ();
  }

  QMenu *menu ()
  {
    return mp_action->menu ();
  }

  void set_has_submenu ();

  bool has_submenu () const
  {
    return m_has_submenu;
  }

  void set_remove_on_empty ();

  bool remove_on_empty () const
  {
    return m_remove_on_empty;
  }

  std::list <AbstractMenuItem> children;

private:
  geo::shared_ptr<Action> mp_action;
  Dispatcher *mp_dispatcher;
  bool m_has_submenu;
  bool m_remove_on_empty;
  std::string m_name;
  std::string m_basename;
  std::set<std::string> m_groups;
};

/**
 *  @brief The abstract menu class
 *
 *  The abstract menu is a class that stores a main menu and several popup menus
 *  in a generic form such that they can be manipulated and converted into QMenu's or
 *  QMenuBar setups.
 *
 *  Each item can be associated with a Action, which delivers a title, enabled/disable state etc.
 *  The Action is either provided when new entries are inserted or created upon initialisation.
 *
 *  The abstract menu class provides methods to manipulate the menu structure (the state of the
 *  menu items, their title and shortcut key is provided and manipulated through the Action object). 
 *
 *  Menu items and submenus are referred to by a "path". The path is a string with this interpretation:
 *
 *    ""                 is the root 
 *    "[<path>.]<name>"  is an element of the submenu given by <path>. If <path> is omitted, this refers to 
 *                       an element in the root 
 *    "[<path>.]end      refers to the item past the last item of the submenu given by <path> or root
 *    "[<path>.]begin    refers to the first item of the submenu given by <path> or root
 *    "[<path>.]#<n>     refers to the nth item of the submenu given by <path> or root (n is an integer number)
 *
 *  The abstract menu provides methods to obtain QMenu and Action objects associated with a certain
 *  item by specifying a path.
 *
 *  Menu items can be put into groups. The path strings of each group can be obtained with the 
 *  "group" method. An item is put into a group by appending ":<group-name>" to the item's name.
 *  This specification can be used several times.
 *
 *  Detached menus (i.e. for use in context menus) can be created as virtual top-level submenus
 *  with a name of the form "@<name>". Such menu's are created as detached QMenu object's and not
 *  inserted into the main menu. A special detached menu is "@toolbar" which describes all elements
 *  placed into the toolbar passed to build (). Detached menu objects can be retrieved with the
 *  detached_menu () method, passing the desired name (without the "@"). 
 *  A detached (separate menu bar) can be created as well from detached menu bars. Such menu
 *  bars are specified by submenus starting with "@@". Detached menu bars can be filled using the 
 *  build_detached () method.
 *
 *  The abstract menu class issues a signal ("changed") if the structure has changed. To recreate the
 *  QMenu objects and fill the QMenuBar, the "build" method needs to be called. This will delete all
 *  QMenu's created so far (except the detached menus), clear the QMenuBar, recreate the QMenu objects (note, that the 
 *  addresses will change this way!) and refill the QToolBar and QMenuBar.
 */
class AbstractMenu
  : public QObject,
    public gsi::ObjectBase
{
Q_OBJECT

public:
  /**
   *  @brief Create the abstract menu object attached to the given main window
   */
  AbstractMenu (Dispatcher *dispatcher=0);
  
  /** 
   *  @brief Destroy the abstract menu object
   */
  ~AbstractMenu ();

  /**
   *  @brief Rebuild the QMenu's and refill the QMenuBar object
   */
  void build (QMenuBar *mbar, QToolBar *tbar);

  /**
   *  @brief Rebuild a menu into a 
   *
   *  This will fill the given menu bar with a menu from
   *  a "@.." top level entry. The menu bar is cleared and rebuilt.
   *
   *  @param name The name of the detached menu, without the "@"
   *  @param mbar The menu bar into which to build the menu
   */
  void build_detached (const std::string &name, QFrame *mbar);

  /**
   *  @brief Get the reference to a QMenu object
   *
   *  @param path The path to the submenu
   *  @return A reference to a QMenu object or 0 if the path is not valid or does not refer to a submenu
   */
  QMenu *menu (const std::string &path);

  /**
   *  @brief Get the Action object for a given item
   *
   *  The Action object returned is basically a pointer to the underlying QAction object,
   *  which is potentially shared between multiple menu items.
   *
   *  @param path The path to the item. This must be a valid path.
   *  @return The action object
   */
  Action *action (const std::string &path);

  /**
   *  @brief Get the Action object for a given item (const version)
   */
  const Action *action(const std::string &path) const;

  /**
   *  @brief Get the Action object of a menu entry inserted directly into a QMenu
   *
   *  Unlike "action(path)" (which searches the AbstractMenuItem tree), this
   *  resolves entries that were inserted with insert_menu(dispatcher, ...) and
   *  are tracked by their menu path (e.g. "view_menu.command").
   *
   *  @param name The menu path of the entry
   *  @return The action object or 0 if the entry does not exist
   */
  Action *menu_action(const std::string &name);

  /**
   *  @brief Get the subitems for a given submenu
   *
   *  @param path The path to the submenu
   *  @return A vector or path strings for the child items or an empty vector if the path is not valid or the item does not have children
   */
  std::vector<std::string> items (const std::string &path) const;

  /**
   *  @brief Query if an item is a menu
   *
   *  @param path The path to the item
   *  @return false if the path is not valid or is not a menu
   */
  bool is_menu (const std::string &path) const;

  /**
   *  @brief Query if an item is a separator
   *
   *  @param path The path to the item
   *  @return false if the path is not valid or is not a separator
   */
  bool is_separator (const std::string &path) const;

  /**
   *  @brief Query if a path is a valid one
   *
   *  @param path The path to check
   *  @return false if the path is not a valid path to an item
   */
  bool is_valid (const std::string &path) const;

  /**
   *  @brief Insert a new item before the one given by the path
   *
   *  The action passed to this method is copied as the item's action. The item will refer to 
   *  this action and issue triggered events on this.
   *
   *  @param path The path to the item before which to insert the new item
   *  @param name The name of the item to insert 
   *  @param action The action associated with the item
   *
   *  NOTE: the abstract menu will take ownership of the Action object.
   */
  void insert_item (const std::string &path, const std::string &name, Action *action);

  /**
   *  @brief Insert a new separator before the one item by the path
   *
   *  @param path The path to the item before which to insert the separator
   *  @param name The name of the separator to insert 
   */
  void insert_separator (const std::string &path, const std::string &name); 

  /**
   *  @brief Insert a new submenu before the item given by the path
   *
   *  The action passed to this method must be provided and owned by the caller
   *  unless "owned" is true.
   *
   *  @param path The path to the item before which to insert the new submenu
   *  @param name The name of the submenu to insert 
   *  @param action The action associated with the submenu
   */
  void insert_menu (const std::string &path, const std::string &name, Action *action);

  /**
   *  @brief Insert a new submenu before the item given by the path
   *
   *  The title string optionally encodes the key shortcut and icon resource
   *  in the form <text>["("shortcut")"]["<"icon-resource">"].
   *
   *  @param path The path to the item before which to insert the submenu
   *  @param name The name of the submenu to insert 
   *  @param title The title of the submenu to insert
   */
  void insert_menu (const std::string &path, const std::string &name, const std::string &title);


  void insert_menu (const std::string& name, const std::string &path, const std::string &, const std::string &title);
  void insert_separator (const std::string &path);
  void insert_menu (QMenuBar *, const std::string &name, const std::string &title);
  void insert_menu (Dispatcher* d, const std::string &name, const std::string &symbol, const std::string &title);
  void insert_item (const std::string &path, Action *ac);
  void clear_menu (const std::string &path);

  /**
   *  @brief Delete the item given by the path
   *
   *  @param path The path to the item to delete
   */
  void delete_item (const std::string &path);

  /**
   *  @brief Delete the items referring to the given action
   */
  void delete_items (Action *action);

  /**
   *  @brief Get the group members
   *
   *  @param group The group name
   *  @param A vector of all members (by path) of the group
   */
  std::vector<std::string> group (const std::string &name) const;

  /**
   *  @brief Get the group members as Action objects
   *
   *  @param group The group name
   *  @param A vector of all members (as actions) of the group
   */
  std::vector<Action *> group_actions(const std::string &name);

  /**
   *  @brief Get the configure actions for a given configuration name
   *
   *  @param The configuration actions for this given configuration name
   */
  std::vector<lay::ConfigureAction *> configure_actions (const std::string &name);

  /**
   *  @brief Get the detached menu
   *
   *  This will return a QMenu pointer to a detached menu that is created
   *  with a "@.." top level entry. In any case, a valid QMenu object is returned
   *  which never changes, even if the menu hierarchy is rebuilt. The QMenu returned
   *  may be empty.
   *
   *  @param name The name of the detached menu, without the "@"
   *  @return a valid QMenu object
   */
  QMenu *detached_menu (const std::string &name);

  /**
   *  @brief Creates a new exclusive action group
   *
   *  If a group with that name already exists, this method does nothing.
   *
   *  @return The QActionGroup object of that group
   */
  QActionGroup *make_exclusive_group (const std::string &name);

  /**
   *  @brief Gets the keyboard shortcuts
   *  @param with_defaults Returns the default shortcuts if true. Otherwise returns the effective shortcut.
   *  @return a hash with menu paths for keys and key binding for values
   */
  std::map<std::string, std::string> get_shortcuts (bool with_defaults)
  {
    std::map<std::string, std::string> b;
    get_shortcuts (std::string (), b, with_defaults);
    return b;
  }

  /**
   *  @brief Gets the root node of the menu
   */
  const AbstractMenuItem &root () const
  {
    return m_root;
  }

signals:
  /**
   *  @brief this signal is emitted whenever something changes on the menu
   */
  void changed ();

private:
  friend class Action;

  std::vector<std::pair<AbstractMenuItem *, std::list<AbstractMenuItem>::iterator> > find_item (geo::Extractor &extr);
  const AbstractMenuItem *find_item_exact (const std::string &path) const;
  AbstractMenuItem *find_item_exact (const std::string &path);
  void build (QMenu *menu, std::list<AbstractMenuItem> &items);
  void build (QToolBar *tbar, std::list<AbstractMenuItem> &items);
  void collect_group (std::vector<std::string> &grp, const std::string &name, const AbstractMenuItem &item) const;
  void collect_configure_actions (std::vector<ConfigureAction *> &ca, AbstractMenuItem &item);
  void emit_changed ();
  void get_shortcuts (const std::string &root, std::map<std::string, std::string> &bindings, bool with_defaults);

  std::map<std::string, QMenu *> m_top_menu;
  std::map<std::string, Action*> m_sub_menu;

  Dispatcher *mp_dispatcher;
  AbstractMenuItem m_root;
  tl::stable_vector<QMenu> m_helper_menu_items;

  std::map<std::string, QActionGroup *> m_action_groups;
  std::map<std::string, std::vector<ConfigureAction *> > m_config_action_by_name;
  bool m_config_actions_valid;
};

/**
 *  @brief A delegate by which the dispatcher can submit notification events
 */
class DispatcherDelegate
{
public:
  /**
   *  @brief Notifies the plugin root that a new plugin class has been registered
   *
   *  This method is called when a plugin is loaded dynamically during runtime.
   */
  virtual void plugin_registered (lay::PluginDeclaration * /*cls*/)
  {
    // .. this implementation does nothing ..
  }

  /**
   *  @brief Notifies the plugin root that a plugin class is about to be removed
   */
  virtual void plugin_removed (lay::PluginDeclaration * /*cls*/)
  {
    // .. this implementation does nothing ..
  }

  /**
   *  @brief Selects the given mode
   *
   *  The implementation is supposed to select the given mode on all related plugins.
   */
  virtual void select_mode (int /*mode*/)
  {
    // .. this implementation does nothing ..
  }

  /**
   *  @brief Menu command handler
   */
  virtual void menu_activated (const std::string & /*symbol*/)
  {
    // .. this implementation does nothing ..
  }

  /**
   *  @brief Receives configuration events
   */
  virtual bool configure (const std::string & /*name*/, const std::string & /*value*/)
  {
    return false;
  }

  /**
   *  @brief Configuration finalization
   */
  virtual void config_finalize ()
  {
    //  .. the default implementation does nothing ..
  }
};

/**
 *  @brief The central menu event and configuration dispatcher class
 *
 *  This class acts as the top level dispatcher for plugin events and the menu configuration.
 */
class Dispatcher
  : public Plugin
{
public:
  /**
   *  @brief The constructor
   *
   *  @param parent Usually 0, but a dispatcher may have parents. In this case, the dispatcher is not the actual dispatcher, but the real plugin chain's root is.
   *  @param standalone The standalone flag passed to the plugin constructor.
   */
  Dispatcher (Plugin *parent = 0, bool standalone = false);

  /**
   *  @brief The root constructor
   *
   *  @param delegate The notification receiver for dispatcher events
   */
  Dispatcher (DispatcherDelegate *delegate, Plugin *parent = 0, bool standalone = false);

  /**
   *  @brief Destructor
   */
  ~Dispatcher ();

  /**
   *  @brief Write configuration to a file
   *
   *  If the configuration file cannot be written, false
   *  is returned but no exception is thrown.
   *
   *  @return false, if an error occurred.
   */
  bool write_config (const std::string &config_file);

  /**
   *  @brief Read the configuration from a file
   *
   *  This method silently does nothing, if the config file does not
   *  exist. If it does and an error occurred, the error message is printed
   *  on stderr. In both cases, false is returned.
   *
   *  @return false, if an error occurred.
   */
  bool read_config (const std::string &config_file);

  /**
   *  @brief The singleton instance of the plugin root
   */
  static Dispatcher *instance ();

  /**
   *  @brief Notifies the plugin root that a new plugin class has been registered
   *
   *  This method is called when a plugin is loaded dynamically during runtime.
   */
  virtual void plugin_registered (lay::PluginDeclaration *cls)
  {
    if (mp_delegate) {
      mp_delegate->plugin_registered (cls);
    }
  }

  /**
   *  @brief Notifies the plugin root that a plugin class is about to be removed
   */
  virtual void plugin_removed (lay::PluginDeclaration *cls)
  {
    if (mp_delegate) {
      mp_delegate->plugin_registered (cls);
    }
  }

  /**
   *  @brief Selects the given mode
   *
   *  The implementation is supposed to select the given mode on all related plugins.
   */
  virtual void select_mode (int mode)
  {
    if (mp_delegate) {
      mp_delegate->select_mode (mode);
    }
  }

  /**
   *  @brief Called, when a menu item is selected
   */
  virtual void menu_activated (const std::string &symbol)
  {
    if (mp_delegate) {
      mp_delegate->menu_activated (symbol);
    }
  }

  /**
   *  @brief Gets the parent widget
   */
  QWidget *menu_parent_widget ()
  {
    return mp_menu_parent_widget;
  }

  /**
   *  @brief Gets the parent widget
   */
  void set_menu_parent_widget (QWidget *w)
  {
    mp_menu_parent_widget = w;
  }

  /**
   *  @brief Returns true, if the dispatcher supplies a user interface
   */
  bool has_ui () { return menu_parent_widget () != 0; }

  /**
   *  @brief Gets the AbstractMenu object
   *
   *  This will deliver the actual menu - the one that is the root dispatcher's menu
   */
  AbstractMenu *menu ()
  {
    return (dispatcher () == this) ? &m_menu : dispatcher ()->menu ();
  }

protected:
  //  capture the configuration events so we can change the value of the configuration actions
  virtual bool configure (const std::string &name, const std::string &value);
  virtual void config_finalize ();

private:
  Dispatcher (const Dispatcher &);
  Dispatcher &operator= (const Dispatcher &);

  lay::AbstractMenu m_menu;
  QWidget *mp_menu_parent_widget;
  DispatcherDelegate *mp_delegate;
};


class ModeAction
    : public Action
{
public:
    ModeAction (Dispatcher *dispatcher, const std::string &title, int mode)
        : Action (title), mp_dispatcher (dispatcher), m_mode (mode)
    { }

    void triggered ()
    {
        if (mp_dispatcher) {
            mp_dispatcher->select_mode (m_mode);
            set_checked (true);
        }
    }

    bool is_for_mode (int mode_id) const
    {
        return mode_id == m_mode;
    }

private:
    Dispatcher *mp_dispatcher;
    int m_mode;
};

class GenericMenuAction
    : public Action
{
public:
    GenericMenuAction (Dispatcher *dispatcher, const std::string &title, const std::string &symbol)
        : Action (title), mp_dispatcher (dispatcher), m_symbol (symbol)
    { }

    void triggered ()
    {
        if (mp_dispatcher) {
            mp_dispatcher->menu_activated (m_symbol);
        }
    }

private:
    Dispatcher *mp_dispatcher;
    std::string m_symbol;
};

}   // namespace lay

namespace tl
{
  template <> struct type_traits<lay::AbstractMenu> : public type_traits<void> {
    typedef tl::false_tag has_copy_constructor;
    typedef tl::false_tag has_default_constructor;
  };

  template <> struct type_traits<lay::AbstractMenuItem> : public type_traits<void> {
    typedef tl::false_tag has_copy_constructor;
    typedef tl::false_tag has_default_constructor;
  };

  template <> struct type_traits<lay::Action> : public type_traits<void> {
    typedef tl::false_tag has_copy_constructor;
  };
}

#endif


