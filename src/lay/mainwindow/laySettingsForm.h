#ifndef HDR_laySettingsForm
#define HDR_laySettingsForm

#include <QDialog>

#include "ui_SettingsForm.h"

#include "layWidgets.h"
#include "tlException.h"
#include "detail/geoTypes.h"

#include <vector>

namespace lay
{

class Dispatcher;
class ConfigPage;

class SettingsForm
  : public QDialog, private Ui::SettingsForm 
{
  Q_OBJECT

public:
  SettingsForm (QWidget *parent, lay::Dispatcher *dispatcher, const char *name);
  
  void setup ();
  void commit ();

public slots:
  virtual void ok_clicked ();
  virtual void apply_clicked ();
  void reset_clicked ();
  void item_changed (QTreeWidgetItem *, QTreeWidgetItem *);

private:
  lay::Dispatcher *mp_dispatcher;
  std::vector <lay::ConfigPage *> m_config_pages;
  bool m_finalize_recursion;
};

}

#endif

