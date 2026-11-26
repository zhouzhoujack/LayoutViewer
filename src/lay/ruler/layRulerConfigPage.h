#ifndef HDR_antConfigPage
#define HDR_antConfigPage

#include "layPlugin.h"
#include "layRulerTemplate.h"
#include "layConfigPage.h"

class QListWidgetItem;

namespace Ui
{
  class RulerConfigPage;
  class RulerConfigPage2;
  class RulerConfigPage3;
  class RulerConfigPage4;
}

namespace lay
{

class RulerConfigPage
  : public lay::ConfigPage
{
  Q_OBJECT 

public:
  RulerConfigPage (QWidget *parent);
  ~RulerConfigPage ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::RulerConfigPage *mp_ui;
  
  void show ();
  void commit ();
};

class RulerConfigPage2
  : public lay::ConfigPage
{
  Q_OBJECT 

public:
  RulerConfigPage2 (QWidget *parent);
  ~RulerConfigPage2 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::RulerConfigPage2 *mp_ui;
  
  void show ();
  void commit ();
};

class RulerConfigPage3
  : public lay::ConfigPage
{
  Q_OBJECT 

public:
  RulerConfigPage3 (QWidget *parent);
  ~RulerConfigPage3 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::RulerConfigPage3 *mp_ui;
  
  void show ();
  void commit ();
};

class RulerConfigPage4
  : public lay::ConfigPage
{
  Q_OBJECT 

public:
 RulerConfigPage4 (QWidget *parent);
  ~RulerConfigPage4 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

public slots:
  void add_clicked ();
  void del_clicked ();
  void up_clicked ();
  void down_clicked ();
  void current_template_changed (int index);
  void double_clicked (QListWidgetItem *);
  
private:
  Ui::RulerConfigPage4 *mp_ui;
  std::vector<Template> m_ruler_templates;
  int m_current_template;
  bool m_current_changed_enabled;
  
  void show ();
  void commit ();
  void update_list ();
};

} // namespace ant

#endif

