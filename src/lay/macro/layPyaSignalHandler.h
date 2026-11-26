#ifndef _HDR_pyaSignalHandler
#define _HDR_pyaSignalHandler

#include <Python.h>

#include "layPyaRefs.h"

#include "gsiSignals.h"

namespace pya
{

/**
 *  @brief A storage object for a function to callback
 */
struct CallbackFunction
{
  CallbackFunction (PythonRef pym, const gsi::MethodBase *m);

  PythonRef callable () const;
  const gsi::MethodBase *method () const;
  bool operator== (const CallbackFunction &other) const;

private:
  PythonRef m_callable;
  PythonRef m_weak_self;
  PythonRef m_class;
  const gsi::MethodBase *mp_method;

  PyObject *callable_ref () const;
  bool is_instance_method () const;
};

/**
 *  @brief The signal handler abstraction
 *
 *  This class implements the signal handler that interfaces to GSI's signal system
 */
class SignalHandler
  : public gsi::SignalHandler
{
public:
  /**
   *  @brief Constructor
   */
  SignalHandler ();

  /**
   *  @brief Destructor
   */
  ~SignalHandler ();

  /**
   *  @brief Implementation of the callback interface
   */
  virtual void call (const gsi::MethodBase *method, gsi::SerialArgs &args, gsi::SerialArgs &ret) const;

  /**
   *  @brief Adds a callable to the list of targets
   */
  void add (PyObject *callable);

  /**
   *  @brief Removes a callable from the list of targets
   */
  void remove (PyObject *callable);

  /**
   *  @brief Clears the list of callables
   */
  void clear ();

  /**
   *  @brief Assign another handler to this
   */
  void assign (const SignalHandler *other);

private:
  std::vector<CallbackFunction> m_cbfuncs;
};

}

#endif
