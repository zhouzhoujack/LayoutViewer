#ifndef HDR_layNetExportDialog
#define HDR_layNetExportDialog

#include "ui_NetExportDialog.h"

#include "dbLayoutToNetlist.h"
#include "tlObjectCollection.h"

#include <QDialog>

namespace Ui
{
  class NetExportDialog;
}

namespace lay
{

class Dispatcher;

/**
 *  @brief A dialog showing the details of a net
 */
class NetExportDialog
  : public QDialog
{
  Q_OBJECT

public:
  NetExportDialog (QWidget *parent);
  ~NetExportDialog ();

  void set_net_prefix (const std::string &net_prefix);
  std::string net_prefix ();

  void set_net_propname (const tl::Variant &net_propname);
  tl::Variant net_propname ();

  void set_produce_circuit_cells (bool f);
  bool produce_circuit_cells ();

  void set_circuit_cell_prefix (const std::string &net_prefix);
  std::string circuit_cell_prefix ();

  void set_produce_device_cells (bool f);
  bool produce_device_cells ();

  void set_device_cell_prefix (const std::string &net_prefix);
  std::string device_cell_prefix ();

  void set_start_layer_number (int ln);
  int start_layer_number ();

  int exec_dialog (lay::Dispatcher *mp_plugin_root);

protected:
  void accept ();

private:
  Ui::NetExportDialog *ui;
};

}

#endif

