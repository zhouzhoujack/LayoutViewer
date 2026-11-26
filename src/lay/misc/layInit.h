#ifndef HDR_layInit
#define HDR_layInit

#include "layNativePlugin.h"

#include <string>
#include <vector>
#include <list>

namespace lay
{

/**
 *  @brief A tiny struct describing a native plugin
 */
struct PluginDescriptor
{
  typedef void (*runner_func_t) ();

  runner_func_t autorun;
  runner_func_t autorun_early;
  std::string version;
  std::string path;
  std::string description;

  PluginDescriptor ()
    : autorun (0), autorun_early (0)
  { }
};

/**
 *  @brief The main initialization function for the "db" module
 *  This function needs to be called initially by all code using the db
 *  module. It will load the plugins and perform initialization of all
 *  of them.
 */
void init (const std::vector<std::string> &paths = std::vector<std::string> ());

/**
 *  @brief Explicitly load a specific plugin
 */
void load_plugin (const std::string &pp);

/**
 *  @brief Gets a list of all plugins registered
 */
const std::list<lay::PluginDescriptor> &plugins ();

}

#endif
