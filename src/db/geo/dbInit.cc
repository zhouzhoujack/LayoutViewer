#include "dbInit.h"
#include "dbPlugin.h"
#include "tlException.h"
#include "tlLog.h"
#include "tlString.h"
#include "tlFileUtils.h"
#include "tlGlobPattern.h"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

#include <set>
#include <cstdio>

namespace db
{
// -----------------------------------------------------------------------------
//  Init implementation


static std::list<db::PluginDescriptor> s_plugins;

const std::list<db::PluginDescriptor> &plugins ()
{
  return s_plugins;
}

static PluginDescriptor load_plugin (const std::string &pp)
{
  PluginDescriptor desc;
  desc.path = pp;

  LOGI << tl::sprintf (tl::to_string (tr ("Loading plugin: %s")), pp);

  dbp_init_func_t init_func = 0;
  static const char *init_func_name = "dbp_init";

#if defined(_WIN32)

  //  there is no "dlopen" on mingw, so we need to emulate it.
  HINSTANCE handle = LoadLibraryW (tl::to_wstring (pp).c_str ());
  if (! handle) {
    throw tl::Exception (tl::to_string (tr ("Unable to load plugin: %s with error message: %s ")), pp, GetLastError ());
    return desc;
  }
  init_func = reinterpret_cast<dbp_init_func_t> (GetProcAddress (handle, init_func_name));

#else

  void *handle;
  handle = dlopen (tl::string_to_system (pp).c_str (), RTLD_LAZY);
  if (! handle) {
    throw tl::Exception (tl::to_string (tr ("Unable to load plugin: %s")), pp);
  }
  init_func = reinterpret_cast<dbp_init_func_t> (dlsym (handle, init_func_name));

#endif

  //  If present, call the initialization function to fetch some details from the plugin
  if (init_func) {
    const char *version = 0;
    const char *description = 0;
    (*init_func) (&version, &description);
    if (version) {
      desc.version = version;
    }
    if (description) {
      desc.description = description;
    }
  }

  return desc;
}

void init (const std::vector<std::string> &_paths)
{
  std::vector<std::string> paths = _paths;

  //  add the module path so we also look beside the "db" library
  std::string module_path = tl::get_module_path ((void *) &init);
  if (! module_path.empty ()) {
    paths.push_back (tl::absolute_path (module_path));
  }

  if (paths.empty ()) {
    //  nothing to do
    LOGI << tl::to_string (tr ("No db_plugins loaded - no path given"));
    return;
  }

  std::set<std::string> modules;

  for (std::vector<std::string>::const_iterator p = paths.begin (); p != paths.end (); ++p) {

    //  look next to the db library, but in "db_plugins" directory
    const char *db_plugin_dir = "db";
    std::string pp = tl::combine_path (*p, db_plugin_dir);

    if (toolkit::logkit::verbosity () >= 20) {
      LOGI << "Scanning for db plugins: " << pp;
    }

    std::vector<std::string> ee = tl::dir_entries (pp, true, false);

    tl::GlobPattern pattern;
#if defined(_WIN32)
    pattern.set_case_sensitive (false);
    pattern = std::string ("*.dll");
#elif defined(__APPLE__)
    pattern = std::string ("*.dylib");
#else
    pattern = std::string ("*.so");
#endif

    std::vector<std::string> inst_modules;
    for (std::vector<std::string>::const_iterator e = ee.begin (); e != ee.end (); ++e) {
      if (pattern.match (*e)) {
        inst_modules.push_back (*e);
          LOGI << "find plugin .so: " << *e;
      }
    }

    std::sort (inst_modules.begin (), inst_modules.end ());

    for (std::vector<std::string>::const_iterator im = inst_modules.begin (); im != inst_modules.end (); ++im) {

      std::string imp = tl::combine_path (pp, *im);
      if (modules.find (*im) == modules.end ()) {
        try {
          s_plugins.push_back (load_plugin (imp));
          modules.insert (*im);
        } catch (tl::Exception &ex) {
          LOGE << ex.msg ();
        }
      }

    }

  }
}

}
