#include "tlAssert.h"
#include "tlException.h"
#include "tlLog.h"

namespace tl
{
// -----------------------------------------------------------------------------
//  Assert implementation


//  to allow setting a debug breakpoint to this function, we declare it "C"
extern "C"
void _assertion_failed (const char *filename, unsigned int line, const char *condition)
{
  LOGE << filename << "," << line << "," << condition; 
}

void 
assertion_failed (const char *filename, unsigned int line, const char *condition)
{
  _assertion_failed (filename, line, condition);
  throw tl::InternalException (filename, line, condition); 
}

}

