#ifndef HDR_laySignalHandler
#define HDR_laySignalHandler

#include <QString>

namespace lay
{

/**
 *  @brief Installs global signal handlers for SIGSEGV and similar
 */
void install_signal_handlers ();

/**
 *  @brief For debugging purposes: get the symbol name from a memory address
 */
QString get_symbol_name_from_address (const QString &mod_name, size_t addr);

/**
 *  @brief Enables GUI support for signal handlers
 */
void enable_signal_handler_gui (bool en);

}

#endif
