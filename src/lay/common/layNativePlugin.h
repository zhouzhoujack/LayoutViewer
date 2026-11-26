#ifndef HDR_layNativePlugin
#define HDR_layNativePlugin


/**
 *  @brief A struct to hold the data of the plugin
 *
 *  Use it like this:
 *
 *  @code
 *  static NativePlugin plugin_desc = {
 *    0,     //  (void (*)()) pointer to autorun function or 0 if not present
 *    0,     //  (void (*)()) pointer to early autorun function or 0 if not present
 *    "1.0", //  (const char *) version information - should be given at least
 *    0      //  (const char *) description or 0/empty if no description is given
 *  };
 *  DECLARE_NATIVE_PLUGIN (plugin_desc);
 *  @endcode
 */
struct NativePlugin {
  void (*autorun) ();
  void (*autorun_early) ();
  const char *version;
  const char *description;
};

/**
 *  @brief A typedef for the initialization function a plugin is supposed to expose.
 */
typedef void (*klp_init_func_t) (void (**autorun) (), void (**autorun_early) (), const char **version, const char **description);

# if defined _WIN32 || defined __CYGWIN__
#   define KLP_PUBLIC __declspec(dllexport)
# else
#   if __GNUC__ >= 4 || defined(__clang__)
#     define KLP_PUBLIC __attribute__ ((visibility ("default")))
#   else
#     define KLP_PUBLIC
#   endif

# endif

#define DECLARE_NATIVE_PLUGIN(desc) \
  extern "C" { \
    KLP_PUBLIC void klp_init (void (**autorun) (), void (**autorun_early) (), const char **version, const char **description) { \
      *autorun = desc.autorun; \
      *autorun_early = desc.autorun_early; \
      *version = desc.version; \
      *description = desc.description; \
    } \
  }

/**
 *  @brief Some (opaque) types for representing some gsi classes in the native API
 */

struct klp_class_t { };
struct klp_method_t { };

/**
 *  @brief The gsi API functions wrapped for the native API
 */
extern "C" {
  const klp_class_t *klp_class_by_name (const char *name);
  void *klp_create (const klp_class_t *cls);
  void klp_destroy (const klp_class_t *cls, void *obj);
  void *klp_clone (const klp_class_t *cls, const void *source);
  void klp_assign (const klp_class_t *cls, void *target, const void *source);
  void klp_require_api_version (const char *version);
}


#endif

