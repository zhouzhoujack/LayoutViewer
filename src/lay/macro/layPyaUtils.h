#ifndef _HDR_pyaUtils
#define _HDR_pyaUtils

#include "tlScriptError.h"
#include "tlException.h"

namespace pya
{

/**
 *  Some helper macros that translate C++ exceptions into Python errors
 */

#define PYA_TRY \
  { \
    try {

#define PYA_CATCH(where) \
    } catch (tl::ExitException &ex) { \
      PyErr_SetObject (PyExc_SystemExit, PyLong_FromLong (ex.status ())); \
    } catch (std::exception &ex) { \
      std::string msg = std::string(ex.what ()) + tl::to_string (tr (" in ")) + (where); \
      PyErr_SetString (PyExc_RuntimeError, msg.c_str ()); \
    } catch (tl::Exception &ex) { \
      std::string msg; \
      msg = ex.msg () + tl::to_string (tr (" in ")) + (where); \
      PyErr_SetString (PyExc_RuntimeError, msg.c_str ()); \
    } catch (...) { \
      std::string msg = tl::to_string (tr ("Unspecific exception in ")) + (where); \
      PyErr_SetString (PyExc_RuntimeError, msg.c_str ()); \
    } \
  }

#define PYA_CATCH_ANYWHERE \
    } catch (tl::ExitException &ex) { \
      PyErr_SetObject (PyExc_SystemExit, PyLong_FromLong (ex.status ())); \
    } catch (std::exception &ex) { \
      PyErr_SetString (PyExc_RuntimeError, ex.what ()); \
    } catch (tl::Exception &ex) { \
      PyErr_SetString (PyExc_RuntimeError, ex.msg ().c_str ()); \
    } catch (...) { \
      PyErr_SetString (PyExc_RuntimeError, tl::to_string (tr ("Unspecific exception in ")).c_str ()); \
    } \
  }

/**
 *  @brief Turn Python errors into C++ exceptions
 */
void check_error ();

}

#endif

