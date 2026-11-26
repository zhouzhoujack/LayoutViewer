#ifndef _HDR_pyaStatusChangedListener
#define _HDR_pyaStatusChangedListener

#include <Python.h>
#include "detail/utilsObject.h"

#include "layPyaRefs.h"

#include "gsiSignals.h"
#include "gsiObject.h"

namespace pya
{

class PYAObjectBase;

/**
 *  @brief A helper object to forward status changed events to a Python object
 *  This object is used to connect the events to the Python object. Unfortunately,
 *  PYAObjectBase cannot be derived from tl::Object directly since in that case,
 *  tl::Object will be placed before PyObject in the memory layout.
 */
class StatusChangedListener
  : public tl::Object
{
public:
  StatusChangedListener (PYAObjectBase *pya_object);

  void object_status_changed (gsi::ObjectBase::StatusEventType type);

  PYAObjectBase *pya_object () const
  {
    return mp_pya_object;
  }

private:
  PYAObjectBase *mp_pya_object;
};

}

#endif
