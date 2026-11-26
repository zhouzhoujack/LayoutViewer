#define STRINGIFY(s) _STRINGIFY(s)
#define _STRINGIFY(s) #s

const char *prg_exe_name        = "LayoutViewer";
const char *prg_name            = "LayoutViewer";

#if defined(KLAYOUT_VERSION)
const char *prg_version         = STRINGIFY(KLAYOUT_VERSION);
#else
const char *prg_version         = "x.xx";
#endif

#if defined(KLAYOUT_VERSION_DATE)
const char *prg_date            = STRINGIFY(KLAYOUT_VERSION_DATE);
#else
const char *prg_date            = "xxxx-xx-xx";
#endif

#if defined(KLAYOUT_VERSION_REV)
const char *prg_rev             = STRINGIFY(KLAYOUT_VERSION_REV);
#else
const char *prg_rev             = "xxxxxxxx";
#endif

const char *prg_author =  "hxzz";

const char *prg_about_text =  "contact with 1259419892@qq.com";

