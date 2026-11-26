#pragma once


#include "tlCommon.h"

namespace tl
{

/** 
 *  @brief The corresponding assert macro
 */

[[noreturn]] TL_PUBLIC void assertion_failed (const char *filename, unsigned int line, const char *condition);

//  toolkit/detail/tlAssert.h 也定义了 tl_assert（展开成不带限定的 assertion_failed，
//  只在 toolkit::logkit 内部有效），这里先撤掉再定义工程版本，避免 redefined 警告。
#ifdef tl_assert
#  undef tl_assert
#endif

//  the throw int(0) instruction will tell the compiler that the assertion will not return
#define tl_assert(COND) if (!(COND)) { \
  tl::assertion_failed (__FILE__, __LINE__, #COND); \
}

} // namespace tl


