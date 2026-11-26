#if !defined(HDR_edtCommon_h)
# define HDR_edtCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_EDT_LIBRARY
#     define EDT_PUBLIC __declspec(dllexport)
#   else
#     define EDT_PUBLIC __declspec(dllimport)
#   endif
#   define EDT_LOCAL
#   define EDT_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define EDT_PUBLIC __attribute__ ((visibility ("default")))
#     define EDT_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define EDT_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define EDT_PUBLIC
#     define EDT_PUBLIC_TEMPLATE
#     define EDT_LOCAL
#   endif

# endif

#endif
