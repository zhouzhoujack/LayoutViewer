#include "gsiInterpreter.h"

namespace gsi
{

GSI_PUBLIC tl::Registrar<Interpreter> interpreters;

// -----------------------------------------------------------------------------
//  Interpreter implementation

Interpreter::Interpreter (int position, const char *name)
  : tl::RegisteredClass<Interpreter> (this, position, name, false)
{
  //  .. nothing yet ..
}

Interpreter::~Interpreter ()
{
  //  .. nothing yet ..
}

}

