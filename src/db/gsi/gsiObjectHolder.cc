#include "gsiObjectHolder.h"
#include "gsiDecl.h"

namespace gsi
{

// -----------------------------------------------------------------------------
//  ObjectHolder implementation

ObjectHolder::ObjectHolder (const gsi::ClassBase *cls, void *obj)
  : mp_cls (cls), mp_obj (obj)
{
  //  .. nothing yet ..
}

ObjectHolder::~ObjectHolder ()
{
  reset (0, 0);
}

void ObjectHolder::reset (const gsi::ClassBase *cls, void *obj)
{
  if (mp_cls == cls && mp_obj == obj) {
    return;
  }

  if (mp_cls) {
    if (mp_obj) {
      mp_cls->destroy (mp_obj);
      mp_obj = 0;
    }
    mp_cls = 0;
  }

  if (cls) {
    mp_cls = cls;
    mp_obj = obj;
  }
}

void *ObjectHolder::release ()
{
  void *obj = mp_obj;
  mp_obj = 0;
  mp_cls = 0;
  return obj;
}

}

