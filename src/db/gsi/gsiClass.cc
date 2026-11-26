#include "gsiDecl.h"

#include "tlLog.h"
#include "tlInternational.h"

#include <map>

namespace gsi
{

//  Note: this is just a fallback implementation, mainly provided for debugging purposes.
//  The dummy class is just provided to avoid some assertions. 
struct EmptyClass { };

static gsi::Class<EmptyClass> default_cls ("tl", "EmptyClass", gsi::Methods ());

const ClassBase *fallback_cls_decl (const std::type_info &ti)
{
  //  This is the main purpose of this function: print the missing class
  tl::warn << tl::to_string (tr ("Unable to find GSI class binding for: ")) << ti.name ();
  return &default_cls;
}

}

