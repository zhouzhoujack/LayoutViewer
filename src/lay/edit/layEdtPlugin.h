#ifndef HDR_edtPlugin
#define HDR_edtPlugin

#include "layPlugin.h"
#include "layPluginDeclaration.h"

#include <vector>

namespace lay
{
  class Dispatcher;
  class EditorOptionsPage;
}

namespace edt
{
  /**
   *  @brief A helper class for plugin declarations for editor services
   */
  class PluginDeclarationBase
    : public lay::PluginDeclaration
  {
    //  .. nothing yet ..
  };

  /**
   *  @brief Commits the current configuration for the recently used configuration list
   */
  void commit_recent (lay::LayoutView *view);
}

#endif

