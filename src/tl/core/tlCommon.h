#pragma once

#include "tlDefs.h"

# ifndef TL_PUBLIC
#   ifdef MAKE_TL_LIBRARY
#     define TL_PUBLIC           DEF_INSIDE_PUBLIC
#     define TL_PUBLIC_TEMPLATE  DEF_INSIDE_PUBLIC_TEMPLATE
#     define TL_LOCAL            DEF_INSIDE_LOCAL
#   else
#     define TL_PUBLIC           DEF_OUTSIDE_PUBLIC
#     define TL_PUBLIC_TEMPLATE  DEF_OUTSIDE_PUBLIC_TEMPLATE
#     define TL_LOCAL            DEF_OUTSIDE_LOCAL
#   endif
# endif

# ifndef GSI_PUBLIC
#   ifdef MAKE_GSI_LIBRARY
#     define GSI_PUBLIC           DEF_INSIDE_PUBLIC
#     define GSI_PUBLIC_TEMPLATE  DEF_INSIDE_PUBLIC_TEMPLATE
#     define GSI_LOCAL            DEF_INSIDE_LOCAL
#   else
#     define GSI_PUBLIC           DEF_OUTSIDE_PUBLIC
#     define GSI_PUBLIC_TEMPLATE  DEF_OUTSIDE_PUBLIC_TEMPLATE
#     define GSI_LOCAL            DEF_OUTSIDE_LOCAL
#   endif
# endif
