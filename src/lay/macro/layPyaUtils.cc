#include <Python.h>
#include "tlException.h"
#include <frameobject.h>   //  Python - for traceback

#include "tlString.h"

#include "layPyaUtils.h"
#include "layPyaConvert.h"
#include "layPya.h"

#include <string>
#include <vector>
#include <algorithm>

namespace pya
{
// -----------------------------------------------------------------------------
//  PyaUtils implementation


/**
 *  @brief Turn Python errors into C++ exceptions
 */
void check_error ()
{
  PyObject *py_exc_type = NULL, *py_exc_value = NULL, *py_exc_traceback = NULL;
  PyErr_Fetch (&py_exc_type, &py_exc_value, &py_exc_traceback);
  PythonRef exc_type (py_exc_type);
  PythonRef exc_value (py_exc_value);
  PythonRef exc_traceback (py_exc_traceback);

  std::string exc_cls ("unknown");
  const char *c = ((PyTypeObject *) exc_type.get ())->tp_name;
  if (c) {
    exc_cls = c;
  }

  if (exc_type) {

    //  fetch traceback
    //  TODO: really decref the stack trace? how about the other objects in the stack trace?
    std::vector <tl::BacktraceElement> backtrace;
    if (exc_traceback) {
      PyTracebackObject *traceback = (PyTracebackObject*) exc_traceback.get ();
      for (PyTracebackObject *t = traceback; t; t = t->tb_next) {
        // backtrace.push_back (tl::BacktraceElement (python2c<std::string> (t->tb_frame->f_code->co_filename), t->tb_lineno));
      }
      std::reverse (backtrace.begin (), backtrace.end ());
    }

    if (PyErr_GivenExceptionMatches (exc_type.get (), PyExc_SyntaxError) && PyTuple_Check (exc_value.get ()) && PyTuple_Size (exc_value.get ()) >= 2) {

      const char *sourcefile = 0;
      int line = 0;
      std::string msg;

      const char *msg_arg = 0, *sourcefile_arg = 0, *text_arg = 0; 
      int line_arg = 0, column_arg = 0;
      if (exc_value && PyArg_ParseTuple (exc_value.get (), "s(siis)", &msg_arg, &sourcefile_arg, &line_arg, &column_arg, &text_arg)) {

        //  build a Ruby-like message
        msg = sourcefile_arg;
        msg += ":";
        msg += tl::to_string (line_arg);
        msg += ": ";
        msg += msg_arg;

        sourcefile = sourcefile_arg;
        line = line_arg;

      } else {
        msg = "syntax error";
      }

      if (! backtrace.empty () && ! sourcefile) {
        sourcefile = backtrace.front ().file.c_str ();
        line = backtrace.front ().line;
      }

      throw PythonError (msg.empty () ? exc_cls.c_str () : msg.c_str (), sourcefile ? sourcefile : "unknown", line, exc_cls.c_str (), backtrace);

    } else if (PyErr_GivenExceptionMatches (exc_type.get (), PyExc_SystemExit)) {

      int status = 0;
      if (exc_value && test_type<int> (exc_value.get (), true)) {
        status = python2c<int> (exc_value.get ());
      }

      throw tl::ExitException (status);

    } else {

      std::string msg;
      if (exc_value) {
        PythonRef msg_str (PyObject_Str (exc_value.get ()));
        if (msg_str && test_type<std::string> (msg_str.get (), true)) {
          msg = python2c<std::string> (msg_str.get ());
        }
      }

      const char *sourcefile = 0;
      int line = 0;

      if (! backtrace.empty ()) {
        sourcefile = backtrace.front ().file.c_str ();
        line = backtrace.front ().line;
      }

      throw PythonError (msg.empty () ? exc_cls.c_str () : msg.c_str (), sourcefile ? sourcefile : "unknown", line, exc_cls.c_str (), backtrace);

    }

  }
}

}

