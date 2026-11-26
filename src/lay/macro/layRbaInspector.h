#ifndef _HDR_rbaInspector
#define _HDR_rbaInspector

namespace gsi
{
  class Inspector;
}

namespace rba
{

/**
 *  @brief Creates an inspector for the given context level above the current frame
 *
 *  Returns 0 if no inspector is available. Otherwise a new'd inspector object is returned.
 *  the caller is responsible for destroying that object.
 */
gsi::Inspector *create_inspector (int context);

}

#endif

