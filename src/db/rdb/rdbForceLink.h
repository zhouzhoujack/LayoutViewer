#ifndef HDR_rdbForceLink
#define HDR_rdbForceLink

#include "rdbCommon.h"

/**
 *  @file Include this function to force linking of the rdb module
 */

namespace rdb
{
  RDB_PUBLIC int _force_link_f ();
  static int _force_link_target = _force_link_f ();
}

#endif

