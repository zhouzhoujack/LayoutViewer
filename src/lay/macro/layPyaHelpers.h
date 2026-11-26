#ifndef _HDR_pyaHelpers
#define _HDR_pyaHelpers

#include <Python.h>

#include "gsiDecl.h"
#include "gsiInterpreter.h"
#include "layPyaRefs.h"

namespace pya
{

class SignalHandler;

// -------------------------------------------------------------------
//  Helper objects: PYAChannelObject

/**
 *  @brief The channel object that replaces stdout/stderr when the console is set
 */
struct PYAChannelObject
  : public PyObject
{
  static void make_class (PyObject *module);
  static PYAChannelObject *create (gsi::Console::output_stream chn);

  gsi::Console::output_stream channel;
  static PyTypeObject *cls;
};

// -------------------------------------------------------------------
//  Helper objects: PYAStaticAttributeDescriptorObject

/**
 *  @brief The static attribute descriptor object
 */
struct PYAStaticAttributeDescriptorObject
  : public PyObject
{
  typedef PyObject *((*func_t) (PyObject *self, PyObject *args));

  static void make_class (PyObject *module);
  static PYAStaticAttributeDescriptorObject *create (const char *n);

  func_t getter, setter;
  const char *name;
  PyTypeObject *type;
  static PyTypeObject *cls;
};

// -------------------------------------------------------------------
//  Helper objects: PYAAmbiguousMethodDispatcher

/**
 *  @brief The non-static/static dispatcher object
 */
struct PYAAmbiguousMethodDispatcher
  : public PyObject
{
  static void make_class (PyObject *module);
  static PYAAmbiguousMethodDispatcher *create (PyObject *ai, PyObject *ac);

  PyObject *attr_inst, *attr_class;
  static PyTypeObject *cls;
};

// -------------------------------------------------------------------
//  Helper objects: PYAIteratorObject

/**
 *  @brief The iterator object delivered by iterator "return" values
 *  This object will turn a gsi::IterAdaptorAbstractBase class into a Python iterator
 */
struct PYAIteratorObject
  : public PyObject
{
  static void make_class (PyObject *module);
  static PYAIteratorObject *create (PyObject *origin, gsi::IterAdaptorAbstractBase *iter, const gsi::ArgType *value_type);

  PyObject *origin;
  bool first;
  gsi::IterAdaptorAbstractBase *iter;
  const gsi::ArgType *value_type;

  static PyTypeObject *cls;
};

// -------------------------------------------------------------------
//  Helper objects: PYASignal

/**
 *  @brief The signal object will be delivered by signal getters and allow manipulation of the signal
 */
struct PYASignal
  : public PyObject
{
  static void make_class (PyObject *module);
  static PYASignal *create (PyObject *origin, pya::SignalHandler *handler);

  PYASignal (PyObject *_origin, pya::SignalHandler *_handler);
  ~PYASignal ();

  PyObject *origin;
  tl::weak_ptr<pya::SignalHandler> handler;

  static PyTypeObject *cls;
};

}

#endif

