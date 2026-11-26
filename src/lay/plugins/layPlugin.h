#ifndef HDR_layPlugin
#define HDR_layPlugin


#include <QtWidgets/QFrame>
#include "tlTypeTraits.h"
#include "detail/utilsString.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"
#include "detail/utilsDeferredExecution.h"
#include <QObject>

#include "tlString.h"
#include "tlClassRegistry.h"
#include "tlDeferredExecution.h"
#include "gsiObject.h"

#include <map>
#include <vector>
#include <string>

namespace geo
{
  class Manager;
}

namespace lay
{

class Plugin;
class Dispatcher;
class LayoutView;
class Browser;
class ViewService;
class Editable;
class Drawing;
class TechnologyComponentProvider;
class EditorOptionsPage;
class PluginDeclaration;


/**
 *  @brief The plugin interface
 *
 *  Each configurable object must be derived from this interface.
 *  An configurable object can have a parent. This way, a hierarchy
 *  of configurable objects is created. The root object not having a
 *  parent acts as the main entry point: it will try to dispatch 
 *  configuration requests to the children.
 *  A node may have a local configuration - it will override any
 *  parent configurations.
 */

class Plugin
  : public gsi::ObjectBase,
    virtual public geo::RefCounted
{
public:
  typedef std::map <std::string, std::string>::const_iterator iterator;

  /**
   *  @brief The constructor
   *  
   *  See above for a explanation of the parent-child relationship.
   */
  Plugin (Plugin *parent, bool standalone = false);

  /**
   *  @brief The destructor
   */
  virtual ~Plugin ();

  /**
   *  @brief Gets a value indicating whether this plugin is a standlone plugin
   *
   *  Standalone-Plugins don't receive common events such as menu requests or
   *  mode changes.
   */
  bool is_standalone () const
  {
    return m_standalone;
  }

  /**
   *  @brief Setup
   *
   *  Calling this method will make the root configurable object dump
   *  it's current configuration to this object. The setup method can 
   *  be called in the derived object's constructor for example.
   */
  void config_setup ();
  
  /**
   *  @brief Clear (reset to default) the configuration
   *
   *  If called on a child, local configurations are cleared.
   *  If called on the root, the default configuration is restored.
   */
  void clear_config ();

  /**
   *  @brief Set a certain configuration parameter
   *
   *  This method will promote the configuration to the first child
   *  that accepts it and store the value in the repository for the
   *  root element.
   */
  void config_set (const std::string &name, const std::string &value);

  /**
   *  @brief Set a certain configuration parameter
   *
   *  This method will promote the configuration to the first child
   *  that accepts it and store the value in the repository for the
   *  root element.
   */
  void config_set (const std::string &name, const char *value);

  /**
   *  @brief Set a certain configuration parameter with a given type
   *
   *  This method will promote the configuration to the first child
   *  that accepts it and store the value in the repository for the
   *  root element. It will use geo::to_string to convert the type into
   *  a string.
   */
  template <class T> 
  void config_set (const std::string &name, const T &value)
  {
    config_set (name, geo::to_string (value));
  }

  /**
   *  @brief Set a certain configuration parameter with a given type using a custom string converter
   *
   *  This method will promote the configuration to the first child
   *  that accepts it and store the value in the repository for the
   *  root element. It will use conv.to_string to convert the type into
   *  a string.
   */
  template <class T, class C> 
  void config_set (const std::string &name, const T &value, C conv)
  {
    config_set (name, conv.to_string (value));
  }

  /**
   *  @brief Terminate a sequence of configuration setups
   *
   *  In order to make configuration changes effective, this method
   *  must be called. It calls config_finalize recursively on the 
   *  children. In GUI-enabled applications this step is optional
   *  and is performed automatically through a timer.
   */
  void config_end ();

  /**
   *  @brief Get a certain configuration parameter
   *
   *  This method will fetch the parameter from the repository.
   *  It can only be called on the root element.
   *  If no parameter with the given name is known, this method will 
   *  return false and set "value" to empty.
   */
  bool config_get (const std::string &name, std::string &value) const;

  /**
   *  @brief Get a certain configuration parameter
   *
   *  This is a convenience method which behaves similar. It will return the
   *  configuration value for the given configuration option name. However, if the
   *  configuration option with the given name does not exist, simply an 
   *  empty string is returned.
   */
  std::string config_get (const std::string &name) const
  {
    std::string v;
    config_get (name, v);
    return v;
  }

  /**
   *  @brief Get a certain configuration parameter
   *
   *  This method will fetch the parameter from the repository
   *  and try to convert it into the value requested. To convert
   *  it, geo::from_string is used. If an exception is thrown during
   *  the conversion or the value is not present, false is returned.
   */
  template <class T>
  bool config_get (const std::string &name, T &value) const
  {
    T t;
    std::string s;
    if (! config_get (name, s)) {
      return false;
    }
    try {
      geo::from_string (s, t);
      value = t;
      return true;
    } catch (...) {
      return false;
    }
  }

  /**
   *  @brief Get a certain configuration parameter using a custom converter
   *
   *  This method will fetch the parameter from the repository
   *  and try to convert it into the value requested. To convert
   *  it, conv.from_string is used. If an exception is thrown during
   *  the conversion or the value is not present, false is returned.
   */
  template <class T, class C>
  bool config_get (const std::string &name, T &value, C conv) const
  {
    T t;
    std::string s;
    if (! config_get (name, s)) {
      return false;
    }
    try {
      conv.from_string (s, t);
      value = t;
      return true;
    } catch (...) {
      return false;
    }
  }

  /**
   *  @brief Iterator for the key/value pairs: begin iterator
   */
  iterator begin () const
  {
    return m_repository.begin ();
  }

  /**
   *  @brief Iterator for the key/value pairs: end iterator
   */
  iterator end () const
  {
    return m_repository.end ();
  }

  /**
   *  @brief Get the names of all configuration options available
   */
  void get_config_names (std::vector<std::string> &names) const;

  /**
   *  @brief Gets the dispatcher (the top level end of the plugin chain)
   *  The returned pointer is guaranteed to be non-zero.
   */
  Dispatcher *dispatcher ();

  /**
   *  @brief Gets the dispatcher (the top level end of the plugin chain)
   *  This version may return null, if the plugin is instantiated without a
   *  root.
   */
  Dispatcher *dispatcher_maybe_null ();

  /**
   *  @brief Menu command handler
   *
   *  This method is called if a menu entry registered in the 
   *  plugin declaration is activated. The string passed is the 
   *  symbol of the menu entry.
   */
  virtual void menu_activated (const std::string & /*symbol*/) 
  {
    // .. this implementation does nothing ..
  }

  /**
   *  @brief Return the lay::Browser interface if this object has one
   *
   *  If the object implements the lay::Browser interface, return the pointer to this.
   *  Otherwise the implementation is supposed to return 0.
   */
  virtual lay::Browser *browser_interface ()
  {
    return 0;
  }

  /**
   *  @brief Return the lay::ViewService interface if this object has one
   *
   *  If the object implements the lay::ViewService interface, return the pointer to this.
   *  Otherwise the implementation is supposed to return 0.
   */
  virtual lay::ViewService *view_service_interface ()
  {
    return 0;
  }

  /**
   *  @brief Return the lay::Drawing interface if this object has one
   *
   *  If the object implements the lay::Drawing interface, return the pointer to this.
   *  Otherwise the implementation is supposed to return 0.
   */
  virtual lay::Drawing *drawing_interface ()
  {
    return 0;
  }

  /**
   *  @brief Return the lay::Editable interface if this object has one
   *
   *  If the object implements the lay::Editable interface, return the pointer to this.
   *  Otherwise the implementation is supposed to return 0.
   */
  virtual lay::Editable *editable_interface ()
  {
    return 0;
  }

  /**
   *  @brief Gets a value indicating whether the plugin will accept a dropped file with the given URL or path
   */
  virtual bool accepts_drop (const std::string & /*path_or_url*/) const
  {
    return false;
  }

  /**
   *  @brief Gets called when a file or URL is dropped on the plugin
   */
  virtual void drop_url (const std::string & /*path_or_url*/)
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Associate a plugin with the plugin declaration for that service (getter)
   */
  const PluginDeclaration *plugin_declaration () const
  {
    return mp_plugin_declaration;
  }

  /**
   *  @brief Associate a service with the plugin declaration for that service (setter)
   *
   *  The association is done when the plugin is created. It should not be changed.
   */
  void set_plugin_declaration (const PluginDeclaration *pd) 
  {
    mp_plugin_declaration = pd;
  }

protected:
  /**
   *  @brief Configure method
   *
   *  This method must be supplied by derived classes. If the class
   *  knows about the configuration option it must consume the value
   *  and return 'true'.
   *  No selection is made for "matching" configuration options for
   *  this class.
   */
  virtual bool configure (const std::string & /*name*/, const std::string & /*value*/)
  {
    return false;
  }

  /**
   *  @brief Configuration finalization
   *
   *  This method is called after all configuration changes have been applied.
   *  A derived class might implement this method in order to do some post-
   *  processing of the configuration.
   */
  virtual void config_finalize ()
  {
    //  .. the default implementation does nothing ..
  }

private:
  Plugin (const Plugin &);
  Plugin &operator= (const Plugin &);

  /**
   *  @brief Do the actual setup or pass to the parent if not the root
   */
  void do_config_setup (Plugin *target);

  /**
   *  @brief Do the actual set or pass to the children if not taken 
   */
  bool do_config_set (const std::string &name, const std::string &value, bool for_child);

  /**
   *  @brief Recursively call config_finalize
   */
  void do_config_end ();

  Plugin *mp_parent;
  const PluginDeclaration *mp_plugin_declaration;
  geo::weak_collection<Plugin> m_children;
  std::map <std::string, std::string> m_repository;
  geo::DeferredMethod<lay::Plugin> dm_finalize_config;
  bool m_standalone;
};

/**
 *  @brief A handy function for implementing the configure method
 *
 *  This template compares two values and overwrites the target
 *  with the source if both are different. It returns true, if 
 *  the target was updated indicating that something needs to be
 *  updated.
 */
template <class T>
inline bool test_and_set (T &target, const T &source)
{
  if (target != source) {
    target = source;
    return true;
  } else {
    return false;
  }
}

}

// namespace tl
// {
//   //  disable copy ctor for Plugin
//   template <> struct type_traits<lay::Plugin> : public type_traits<void> {
//     typedef tl::false_tag has_copy_constructor;
//   };
// }

#endif


