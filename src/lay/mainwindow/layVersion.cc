#include "layVersion.h"

#include "tlLog.h"

namespace lay
{
// -----------------------------------------------------------------------------
//  Version implementation


std::string Version::s_about_text = "";
std::string Version::s_version = "";
std::string Version::s_name = "";
std::string Version::s_subversion = "";
std::string Version::s_exe_name = "";

const char* Version::exe_name()
{
    return s_exe_name.c_str();
}

const char* Version::name()
{
    return s_name.c_str();
}

const char* Version::version()
{
    return s_version.c_str();
}

const char* Version::subversion()
{
    return s_subversion.c_str();
}

const char* Version::about_text()
{
    return s_about_text.c_str();
}

void Version::set_exe_name(const char* s)
{

    s_exe_name = s;
}

void Version::set_name(const char* s)
{

    s_name = s;
    LOGI << "app name: " << s;
}

void Version::set_version(const char* s)
{

    s_version = s;
}

void Version::set_subversion(const char* s)
{

    s_subversion = s;
}

void Version::set_about_text(const char* s)
{

    s_about_text = s;
}

} // namespace lay
