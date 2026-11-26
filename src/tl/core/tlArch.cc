#include "tlArch.h"

namespace tl
{
// -----------------------------------------------------------------------------
//  Arch implementation


std::string
arch_string ()
{

#if defined(_WIN32) || defined(_WIN64)
# if defined(_WIN64)
#   if defined(_MSC_VER)
  return "x86_64-win32-msvc";
#   elif defined(__MINGW32__)
  return "x86_64-win32-mingw";
#   endif
# else
#   if defined(_MSC_VER)
  return "i686-win32-msvc";
#   elif defined(__MINGW32__)
  return "i686-win32-mingw";
#   endif
# endif
#elif defined(__clang__)
# if defined(__x86_64__)
  return "x86_64-linux-clang";
# else
  return "i686-linux-clang";
# endif
#elif defined(__GNUC__)
# if defined(__x86_64__)
  return "x86_64-linux-gcc";
# else
  return "i686-linux-gcc";
# endif
#else
  return "";
#endif

}

}

