#ifndef HDR_laySaltGrainPropertiesDialog
#define HDR_laySaltGrainPropertiesDialog

#include "laySaltGrain.h"

#include <QDialog>

#include "ui_SaltGrainPropertiesDialog.h"

namespace lay
{

class Salt;

/**
 *  @brief The dialog for managing the Salt ("Packages")
 */
class SaltGrainPropertiesDialog
  : public QDialog, private Ui::SaltGrainPropertiesDialog
{
Q_OBJECT

public:
  /**
   *  @brief Constructor
   */
  SaltGrainPropertiesDialog (QWidget *parent);

  /**
   *  @brief Executes the dialog for the given grain
   *  If the dialog is committed with "Ok", the new data is written into
   *  the grain provided and "true" is returned. Otherwise, "false" is
   *  returned and the object remains unchanged.
   */
  bool exec_dialog (lay::SaltGrain *grain, lay::Salt *salt);

  /**
   *  @brief Gets the current package index
   */
  lay::Salt *salt ()
  {
    return mp_salt;
  }

private slots:
  void reset_icon ();
  void set_icon ();
  void reset_screenshot ();
  void set_screenshot ();
  void url_changed (const QString &url);
  void add_dependency_clicked ();
  void remove_dependency_clicked ();
  void dependency_changed (QTreeWidgetItem *item, int column);

protected:
  void accept ();

private:
  lay::SaltGrain m_grain;
  lay::Salt *mp_salt;
  QString m_title;
  QString m_open_label;
  QString m_image_dir;
  bool m_update_enabled;

  void update_controls ();
  void update_data ();
  void update_icon ();
  void update_screenshot ();
};

}

#endif
