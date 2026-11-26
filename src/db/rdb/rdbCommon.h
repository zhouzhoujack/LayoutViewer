#if !defined(HDR_rdbCommon_h)
# define HDR_rdbCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_RDB_LIBRARY
#     define RDB_PUBLIC __declspec(dllexport)
#   else
#     define RDB_PUBLIC __declspec(dllimport)
#   endif
#   define RDB_LOCAL
#   define RDB_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define RDB_PUBLIC __attribute__ ((visibility ("default")))
#     define RDB_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define RDB_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define RDB_PUBLIC
#     define RDB_PUBLIC_TEMPLATE
#     define RDB_LOCAL
#   endif

# endif

#endif
