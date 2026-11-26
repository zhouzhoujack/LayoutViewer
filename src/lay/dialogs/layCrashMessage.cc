#include "layCrashMessage.h"
#include "layQtTools.h"

#include <QPushButton>

namespace lay
{

// -----------------------------------------------------------------------------
//  CrashMessage implementation

CrashMessage::CrashMessage (QWidget *parent, bool can_resume, const QString &t)
  : QDialog (parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
  setupUi (this);
  m_cancel_pressed = false;

  text->setFont (monospace_font ());
  text->setPlainText (t);
  set_can_resume (can_resume);

  connect (buttonBox->button (QDialogButtonBox::Cancel), SIGNAL (pressed ()), this, SLOT (cancel_pressed ()));
}

CrashMessage::~CrashMessage ()
{
  //  .. nothing yet ..
}

void
CrashMessage::set_can_resume (bool f)
{
  buttonBox->button (QDialogButtonBox::Ok)->setVisible (f);
}

void
CrashMessage::set_text (const QString &t)
{
  text->setPlainText (t);
}

void
CrashMessage::cancel_pressed ()
{
  m_cancel_pressed = true;
}

}

