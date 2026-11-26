#ifndef HDR_layPasswordDialog
#define HDR_layPasswordDialog

#include "tlHttpStream.h"

#include "ui_PasswordDialog.h"
#include <QDialog>

namespace lay
{

/**
 * @brief A password dialog for registration with tl::HttpStream
 */
class PasswordDialog
  : public QDialog, public tl::HttpCredentialProvider, private Ui::PasswordDialog
{
public:
  PasswordDialog (QWidget *parent);

  bool user_password (const std::string &url, const std::string &realm, bool proxy, int attempt, std::string &user, std::string &passwd);
};



}

#endif
