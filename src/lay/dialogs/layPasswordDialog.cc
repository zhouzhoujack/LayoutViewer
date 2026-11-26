#include "layPasswordDialog.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  PasswordDialog implementation

PasswordDialog::PasswordDialog (QWidget *parent)
  : QDialog (parent)
{
  setupUi (this);
}

bool
PasswordDialog::user_password (const std::string &url, const std::string &realm, bool proxy, int attempt, std::string &user, std::string &passwd)
{
  realm_label->setText (tr ("<b>Realm:</b> ") + geo::to_qstring (realm));
  if (proxy) {
    where_label->setText (tr ("<b>Proxy:</b> ") + geo::to_qstring (url));
  } else {
    where_label->setText (tr ("<b>URL:</b> ") + geo::to_qstring (url));
  }

  if (attempt > 1) {
    attempt_label->setText (tr ("Authentication failed - please try again"));
    attempt_label->show ();
  } else {
    attempt_label->hide ();
  }

  if (QDialog::exec ()) {
    passwd = geo::to_string (password_le->text ());
    user = geo::to_string (user_le->text ());
    return true;
  } else {
    return false;
  }

}

}

