#pragma once

#include <string>

namespace lay
{

class Version
{
public:
    static const char* exe_name();
    static const char* name();
    static const char* version();
    static const char* subversion();
    static const char* about_text();

    static void set_exe_name(const char* s);
    static void set_name(const char* s);
    static void set_version(const char* s);
    static void set_subversion(const char* s);
    static void set_about_text(const char* s);

    static std::string s_exe_name;
    static std::string s_name;
    static std::string s_version;
    static std::string s_subversion;
    static std::string s_about_text;
};

} // namespace lay

