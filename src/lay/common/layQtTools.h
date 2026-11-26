#ifndef HDR_layQtTools
#define HDR_layQtTools


#include <string>
#include "tlException.h"
#include <QFont>

class QLabel;
class QWidget;
class QObject;
class QSignalBlocker;

namespace tl
{
  class Exception;
}

namespace lay
{

/**
 *  @brief Save the given widget's (dialog) state to the string
 *
 *  The state can be recovered from the string using restore_dialog_state;
 */
std::string save_dialog_state (QWidget *dialog, bool with_section_sizes = true);

/**
 *  @brief Restore the dialog's state from the given string
 */
void restore_dialog_state (QWidget *dialog, const std::string &s, bool with_section_sizes = true);

/**
 *  @brief A utility function connecting a label's linkActivated event with the help browser
 */
void activate_help_links (QLabel *label);

/**
 *  @brief A utility function connecting a label's linkActivated event with the help browser (modal help dialogs)
 */
void activate_modal_help_links (QLabel *label);

/**
 *  @brief Register the help handler (object and slots for non-modal and modal help dialogs)
 */
void register_help_handler (QObject *object, const char *slot, const char *modal_slot);

/**
 *  @brief Configures a QLineEdit or other widget to indicate an error
 *
 *  When a non-null ex pointer is passed, the background will be turned red
 *  and the exception's text will be used as tooltip. Use this function with
 *  a null ex pointer to clear the error condition.
 */
void indicate_error (QWidget *le, const tl::Exception *ex);

/**
 *  @brief Configures a QLineEdit or other widget to indicate an error
 */
void indicate_error (QWidget *le, bool error);

/**
 *  @brief Gets the system monospace font
 */
QFont monospace_font ();


#if QT_VERSION < 0x050000

//  Provide missing QSignalBlocker for Qt4

class SignalBlocker
{
public:
  SignalBlocker (QWidget *w);
  ~SignalBlocker ();

private:
  QWidget *mp_widget;
  bool m_state;
};

#else

typedef QSignalBlocker SignalBlocker;

#endif

} // namespace lay

#endif

