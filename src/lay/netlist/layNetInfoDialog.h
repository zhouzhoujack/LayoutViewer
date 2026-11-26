#ifndef HDR_layNetInfoDialog
#define HDR_layNetInfoDialog

#include "ui_NetInfoDialog.h"

#include "dbLayoutToNetlist.h"
#include "tlObjectCollection.h"

#include <QDialog>

namespace Ui
{
  class NetInfoDialog;
}

namespace lay
{

/**
 *  @brief A dialog showing the details of a net
 */
class NetInfoDialog
  : public QDialog
{
  Q_OBJECT

public:
  NetInfoDialog (QWidget *parent);
  ~NetInfoDialog ();

  void set_nets (const db::LayoutToNetlist *l2ndb, const std::vector<const db::Net *> &nets);

private slots:
  void detailed_checkbox_clicked ();

protected:
  void showEvent (QShowEvent *);

private:
  tl::weak_ptr<db::LayoutToNetlist> mp_l2ndb;
  tl::weak_collection<db::Net> mp_nets;
  bool m_needs_update;
  Ui::NetInfoDialog *ui;

  void update_info_text ();
  void needs_update ();
};

}

#endif

