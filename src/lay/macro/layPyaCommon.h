#include "tlDefs.h"

#if !defined(HDR_pyaCommon_h)
# define HDR_pyaCommon_h

# ifdef MAKE_PYA_LIBRARY
#   define PYA_PUBLIC           DEF_INSIDE_PUBLIC
#   define PYA_PUBLIC_TEMPLATE  DEF_INSIDE_PUBLIC_TEMPLATE
#   define PYA_LOCAL            DEF_INSIDE_LOCAL
# else
#   define PYA_PUBLIC           DEF_OUTSIDE_PUBLIC
#   define PYA_PUBLIC_TEMPLATE  DEF_OUTSIDE_PUBLIC_TEMPLATE
#   define PYA_LOCAL            DEF_OUTSIDE_LOCAL
# endif

#endif
