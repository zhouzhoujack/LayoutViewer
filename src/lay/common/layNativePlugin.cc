#include "layNativePlugin.h"

#include "gsiDecl.h"

// -----------------------------------------------------------------------------
//  NativePlugin implementation

const klp_class_t *klp_class_by_name (const char *name)
{
  return reinterpret_cast<const klp_class_t *> (gsi::class_by_name (name));
}

void *klp_create (const klp_class_t *cls)
{
  return reinterpret_cast<const gsi::ClassBase *> (cls)->create ();
}

void klp_destroy (const klp_class_t *cls, void *obj)
{
  reinterpret_cast<const gsi::ClassBase *> (cls)->destroy (obj);
}

void *klp_clone (const klp_class_t *cls, const void *source)
{
  return reinterpret_cast<const gsi::ClassBase *> (cls)->clone (source);
}

void klp_assign (const klp_class_t *cls, void *target, const void *source)
{
  reinterpret_cast<const gsi::ClassBase *> (cls)->assign (target, source);
}

void klp_require_api_version (const char * /*version*/)
{
  //  TODO: implement
}
