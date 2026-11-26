#if !defined(HDR_rbaCommon_h)
# define HDR_rbaCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_RBA_LIBRARY
#     define RBA_PUBLIC __declspec(dllexport)
#   else
#     define RBA_PUBLIC __declspec(dllimport)
#   endif
#   define RBA_LOCAL
#   define RBA_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define RBA_PUBLIC __attribute__ ((visibility ("default")))
#     define RBA_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define RBA_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define RBA_PUBLIC
#     define RBA_PUBLIC_TEMPLATE
#     define RBA_LOCAL
#   endif

# endif

#endif
