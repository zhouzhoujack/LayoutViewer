#include "layPyaStatusChangedListener.h"
#include "layPyaObject.h"

namespace pya
{
// --------------------------------------------------------------------------
//  Implementation of StatusChangedListener

StatusChangedListener::StatusChangedListener (PYAObjectBase *pya_object)
  : mp_pya_object (pya_object)
{
  //  .. nothing yet ..
}

void
StatusChangedListener::object_status_changed (gsi::ObjectBase::StatusEventType type)
{
  if (type == gsi::ObjectBase::ObjectDestroyed) {
    mp_pya_object->object_destroyed ();
  } else if (type == gsi::ObjectBase::ObjectKeep) {
    mp_pya_object->keep_internal ();
  } else if (type == gsi::ObjectBase::ObjectRelease) {
    mp_pya_object->release ();
  }
}

}
