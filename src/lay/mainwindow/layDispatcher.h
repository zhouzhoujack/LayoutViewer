#ifndef HDR_layDispatcher
#define HDR_layDispatcher


#include "tlTypeTraits.h"
#include "tlObject.h"
#include "layPlugin.h"
#include "layAbstractMenu.h"
#include "layPluginDeclaration.h"

#include <map>
#include <string>
#include <vector>

namespace lay
{

class AbstractMenu;
class Action;
class ConfigureAction;

}

namespace tl
{
  //  disable copy ctor for Dispatcher
  template <> struct type_traits<lay::Dispatcher> : public type_traits<void> {
    typedef tl::false_tag has_copy_constructor;
    typedef tl::false_tag has_default_constructor;
  };
}

#endif


