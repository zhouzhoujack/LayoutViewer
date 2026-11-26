#if !defined(HDR_lymCommon_h)
# define HDR_lymCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_LYM_LIBRARY
#     define LYM_PUBLIC __declspec(dllexport)
#   else
#     define LYM_PUBLIC __declspec(dllimport)
#   endif
#   define LYM_LOCAL
#   define LYM_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define LYM_PUBLIC __attribute__ ((visibility ("default")))
#     define LYM_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define LYM_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define LYM_PUBLIC
#     define LYM_PUBLIC_TEMPLATE
#     define LYM_LOCAL
#   endif

# endif

#endif
