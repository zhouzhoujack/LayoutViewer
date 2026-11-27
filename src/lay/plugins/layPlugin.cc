
#include "tlException.h"
#include "detail/utilsString.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"
#include "tlAssert.h"
#include "tlXMLParser.h"
#include "tlLog.h"

#include "layPlugin.h"
#include "layDispatcher.h"
#include "tlExceptions.h"
#include "layPluginDeclaration.h"
#include "tlClassRegistry.h"

#include "layGtf.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <QObject>

namespace lay
{

// ----------------------------------------------------------------
//  Plugin implementation

Plugin::Plugin (Plugin *parent, bool standalone)
  : mp_parent (parent), mp_plugin_declaration (0), dm_finalize_config (this, &lay::Plugin::config_end), m_standalone (standalone)
{
  if (! parent) {
    if (! standalone) {
      //  load the root with the default configuration
      for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
        std::vector< std::pair<std::string, std::string> > pairs;
        cls->get_options (pairs);
        m_repository.insert (pairs.begin (), pairs.end ());
      }
    }
  } else {
    mp_parent->m_children.push_back (this);
  }
}

Plugin::~Plugin ()
{
  // .. nothing yet ..
}

void 
Plugin::config_setup ()
{
  do_config_setup (this);
  do_config_end ();
}
  
void 
Plugin::config_set (const std::string &name, const char *value)
{
  config_set (name, std::string (value));
}

void 
Plugin::config_set (const std::string &name, const std::string &value)
{
  // std::map <std::string, std::string>::iterator m = m_repository.find (name);
  // if (m != m_repository.end ()) {
  //   //  if the value did not change, do nothing
  //   if (m->second == value) {
  //     return;
  //   }
  //   m->second = value;
  // } else {
  //   //  install the value in the repository
  //   m_repository.insert (std::make_pair (name, value));
  // }

  // //  look for plugins that receive that configuration statically if the root is addressed
  // if (! mp_parent && ! m_standalone) {
  //   for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
  //     try {
  //       if ((const_cast<lay::PluginDeclaration *> (&*cls))->configure (name, value)) {
  //         return;
  //       }
  //     } catch (tl::Exception &ex) {
  //       LOGE << geo::to_string (QObject::tr ("Error on configure")) << " " << name << "='" << value << "': " << ex.msg ();
  //     }
  //   }
  // }

  // do_config_set (name, value, false);

  // //  schedule a configuration finalization call (once for all config_set calls)
  // dm_finalize_config ();
}

void 
Plugin::config_end ()
{
  //  finish static configurations for plugins if the root is addressed
  if (! mp_parent && ! m_standalone) {
    for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
      (const_cast<lay::PluginDeclaration *> (&*cls))->config_finalize ();
    }
  }

  do_config_end ();
}

void
Plugin::clear_config ()
{
  m_repository.clear ();
  if (! mp_parent && ! m_standalone) {
    //  load the root with the default configuration
    for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
      std::vector< std::pair<std::string, std::string> > pairs;
      cls->get_options (pairs);
      m_repository.insert (pairs.begin (), pairs.end ());
    }
  }
  config_setup ();
}

bool 
Plugin::config_get (const std::string &name, std::string &value) const
{
  std::map<std::string, std::string>::const_iterator p = m_repository.find (name);
  if (p != m_repository.end ()) {
    value = p->second;
    return true;
  } else if (mp_parent) {
    return mp_parent->config_get (name, value);
  } else {
    value = "";
    return false;
  }
}

void 
Plugin::get_config_names (std::vector<std::string> &names) const
{
  names.reserve (m_repository.size ());

  //  retrieve the configuration names
  for (std::map<std::string, std::string>::const_iterator p = m_repository.begin (); p != m_repository.end (); ++p) {
    names.push_back (p->first);
  }
}

Dispatcher *
Plugin::dispatcher ()
{
  Plugin *p = this;
  while (p->mp_parent) {
    p = p->mp_parent;
  }

  return dynamic_cast<Dispatcher *> (p);
}

Dispatcher *
Plugin::dispatcher_maybe_null ()
{
  Plugin *p = this;
  while (p->mp_parent) {
    p = p->mp_parent;
  }

  return dynamic_cast<Dispatcher *> (p);
}

void 
Plugin::do_config_setup (Plugin *target)
{
  if (mp_parent) {
    mp_parent->do_config_setup (target);
  } 
  //  local configurations override the parent's configuration, i.e. are applied after the parents settings
  for (std::map<std::string, std::string>::const_iterator p = m_repository.begin (); p != m_repository.end (); ++p) {
    target->do_config_set (p->first, p->second, false);
  }
}

void 
Plugin::do_config_end ()
{
  config_finalize ();
  for (geo::weak_collection<Plugin>::iterator c = m_children.begin (); c != m_children.end (); ++c) {
    c->do_config_end ();
  }
}

bool 
Plugin::do_config_set (const std::string &name, const std::string &value, bool for_child)
{
  if (for_child) {
    //  this is the case when we impose a configuration from the parent: in this case we
    //  have to remove it from the repository of local parameters.
    m_repository.erase (name);
  }

  try {
    if (configure (name, value)) {
      //  taken by us - don't propagate to the children
      return true;
    }
  } catch (tl::Exception &ex) {
    LOGE << geo::to_string (QObject::tr ("Error on configure")) << " " << name << "='" << value << "': " << ex.msg ();
  }

  //  propagate to all children (not only the first that takes it!)
  for (geo::weak_collection<Plugin>::iterator c = m_children.begin (); c != m_children.end (); ++c) {
    c->do_config_set (name, value, true);
  }

  return false;
}




}
