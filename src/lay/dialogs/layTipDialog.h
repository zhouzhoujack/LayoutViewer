#ifndef HDR_layTipDialog
#define HDR_layTipDialog


#include <QtWidgets/QDialog>
#include <string>

namespace Ui {
  class TipDialog;
}

namespace lay {

/**
 *  @brief A dialog for showing a general "tip" window
 *
 *  A tip window is basically a message box with the ability to hide the box forever (or at least until the 
 *  configuration is reset). 
 */
class TipDialog 
  : public QDialog
{
Q_OBJECT

public:
  enum buttons_type { close_buttons = 0, okcancel_buttons = 1, yesno_buttons = 2, yesnocancel_buttons = 3 };
  enum button_type { null_button = -1, close_button = 0, cancel_button = 1, ok_button = 2, yes_button = 3, no_button = 4 };

  /** 
   *  @brief Constructor
   *
   *  Creates a tip dialog with the given parent, message text and configuration key. The configuration key
   *  is the key under which the status of the dialog is stored.
   *  The buttons argument allows one to specify which buttons to show (see buttons_type enum for the button
   *  combinations available.
   */
  TipDialog (QWidget *parent, const std::string &text, const std::string &key, buttons_type buttons = close_buttons); 

  /**
   *  @brief Destructor
   */
  ~TipDialog ();

  /**
   *  @brief Returns true, if the tip dialog will be shown
   */
  bool will_be_shown ();

  /**
   *  @brief Show the dialog
   *
   *  This method is intended for the "close_buttons" style. It will show the dialog and return true, 
   *  if the dialog was closed without checking the "hide forever" checkbox.
   */
  bool exec_dialog ();

  /**
   *  @brief Show the dialog
   *
   *  Shows the dialog and return true, if the dialog was closed without checking the "hide forever" checkbox.
   *  The button argument will receive a value indicating the button that was pressed. 
   *  If the dialog is closed with the "hide forever" checkbox checked, it will remember the button that
   *  was pressed to close the dialog (unless the button was "Cancel") and return this value on the next calls
   *  in the button argument, even if the dialog is no longer shown.
   */
  bool exec_dialog (button_type &button);

public slots:
  void close_pressed ();
  void ok_pressed ();
  void cancel_pressed ();
  void yes_pressed ();
  void no_pressed ();

private:
  Ui::TipDialog *mp_ui;
  std::string m_key;
  button_type *mp_res;

  bool do_exec_dialog (button_type *button);
  void accept (); 
  void init (const std::string &text, buttons_type buttons);
};

}

#endif

