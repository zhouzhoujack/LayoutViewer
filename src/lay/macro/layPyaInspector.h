#ifndef _HDR_pyaInspector
#define _HDR_pyaInspector

#include "layPyaRefs.h"

#include "gsiInspector.h"

#include <list>
#include <string>

namespace pya
{

/**
 *  @brief Creates an inspector object from a Python dict, tuple or list
 *
 *  The returned object must be deleted by the caller.
 *  If symbolic is true, the dict is interpreted as a variable list.
 */
gsi::Inspector *
create_inspector (PyObject *container, bool symbolic = false);

}

#endif

