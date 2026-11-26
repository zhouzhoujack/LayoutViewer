#ifndef HDR_laySystemPaths
#define HDR_laySystemPaths


#include <string>
#include <vector>

namespace lay
{

/**
 *  @brief Gets the application data path 
 *  The application data path is the path where the application stores it's
 *  data for each user.
 *  By default this is HOME/.klayout or HOME/KLayout (Windows). The value
 *  can be overridden by the KLAYOUT_HOME environment variable.
 */
std::string get_appdata_path ();

/**
 *  @brief Gets the KLayout path 
 *  This is a path (in the sense of a search path - i.e. multiple paths)
 *  where KLayout (and derived applications) looks for configuration files.
 *  The path is created from the appdata and inst path by default. It can
 *  be overridden by the KLAYOUT_PATH environment variable.
 */
std::vector<std::string> get_klayout_path ();

/**
 *  @brief Sets the KLayout path
 *  This method is mainly used for test purposes. It will force the application
 *  to use a specific path. Use reset_klayout_path to restore the
 *  default behavior.
 */
void set_klayout_path (const std::vector<std::string> &path);

/**
 *  @brief Resets the KLayout path
 *  See "set_klayout_path" for a description.
 */
void reset_klayout_path ();

/**
 *  @brief Gets the package manager URL
 *  The default value can be overridden by the KLAYOUT_SALT_MINE environment
 *  variable.
 */
std::string salt_mine_url ();

}

#endif
