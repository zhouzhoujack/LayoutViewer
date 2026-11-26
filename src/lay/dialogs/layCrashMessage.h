#ifndef HDR_layCrashMessage
#define HDR_layCrashMessage

#include <QtWidgets/QDialog>

#include "ui_CrashMessage.h"

namespace lay
{

/**
 *  @brief A window showing a crash message
 */
class CrashMessage  
  : public QDialog, private Ui::CrashMessage
{
Q_OBJECT

public:
  /**
   *  @brief Instantiate a dialog
   *
   *  @param can_resume If true, an "Ok" button is provided
   *  @param text The message shown in the window
   */
  CrashMessage (QWidget *parent, bool can_resume, const QString &text);

  /**
   *  @brief Destructor
   */
  virtual ~CrashMessage ();

  /**
   *  @brief Configures the dialog for "can resume" or "can't resume"
   */
  void set_can_resume (bool f);

  /**
   *  @brief Sets the text
   */
  void set_text (const QString &text);

  /**
   *  @brief Gets a value indicating whether the Cancel button was pressed
   */
  bool is_cancel_pressed ()
  {
    return m_cancel_pressed;
  }

  /**
   *  @brief Resets the flag indicating whether Cancel was pressed
   */
  void reset_cancel_pressed ()
  {
    m_cancel_pressed = false;
  }

private slots:
  void cancel_pressed ();

private:
  bool m_cancel_pressed;
};

}

#endif

