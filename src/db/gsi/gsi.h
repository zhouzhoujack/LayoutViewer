#ifndef _HDR_gsi
#define _HDR_gsi

/**
 *  @brief GSI - the generic scripting interface
 *
 *  The generic scripting interface provides a way to export C++ classes to a scripting 
 *  language in some generic way. The main target scripting language is Ruby, but the concept
 *  is not restricted to this. 
 */

#include "tlAssert.h"
#include "tlInternational.h"
#include "tlException.h"
#include "tlTypeTraits.h"

#include "gsiObject.h"

namespace gsi
{
  /**
   *  @brief Initialize the GSI system 
   *
   *  This function needs to be called before all other operations. It builds the GSI
   *  method table and registers the classes for the tl::variant container.
   */
  void GSI_PUBLIC initialize ();
}

#endif

