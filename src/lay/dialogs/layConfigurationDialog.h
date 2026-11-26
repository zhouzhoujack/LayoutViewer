#ifndef HDR_layConfigurationDialog
#define HDR_layConfigurationDialog

#include "layWidgets.h"
#include "tlException.h"
#include "detail/geoTypes.h"

#include <QtWidgets/QDialog>

namespace Ui
{
  class ConfigurationDialog;
}

namespace lay
{

class Dispatcher;
class ConfigPage;
class PluginDeclaration;

class ConfigurationDialog
  : public QDialog
{
  Q_OBJECT

public:
  ConfigurationDialog (QWidget *parent, lay::Dispatcher *root, lay::PluginDeclaration *decl, const char *name = "");
  ConfigurationDialog (QWidget *parent, lay::Dispatcher *root, const std::string &plugin_name, const char *name = "");
  ~ConfigurationDialog ();
  
  void commit ();

public slots:
  virtual void ok_clicked ();

private:
  lay::Dispatcher *mp_root;
  std::vector <lay::ConfigPage *> m_config_pages;
  Ui::ConfigurationDialog *mp_ui;

  void init (const lay::PluginDeclaration *decl);
};

}

#endif

