#ifndef HDR_layMainConfigPages
#define HDR_layMainConfigPages

#include <QObject>

#include "layPlugin.h"
#include "layConfigPage.h"

#include <map>

namespace Ui {
  class MainConfigPage;
  class MainConfigPage2;
  class MainConfigPage3;
  class MainConfigPage4;
  class MainConfigPage5;
  class MainConfigPage6;
  class MainConfigPage7;
  class CustomizeMenuConfigPage;
}

class QTreeWidgetItem;
class QAction;

namespace lay
{

class ColorButton;

class MainConfigPage 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage (QWidget *parent);
  ~MainConfigPage ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage *mp_ui;
};

class MainConfigPage2 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage2 (QWidget *parent);
  ~MainConfigPage2 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage2 *mp_ui;
};

class MainConfigPage3 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage3 (QWidget *parent);
  ~MainConfigPage3 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage3 *mp_ui;
};

class MainConfigPage4 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage4 (QWidget *parent);
  ~MainConfigPage4 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage4 *mp_ui;
};

class MainConfigPage5 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage5 (QWidget *parent);
  ~MainConfigPage5 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage5 *mp_ui;
};

class MainConfigPage6 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage6 (QWidget *parent);
  ~MainConfigPage6 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage6 *mp_ui;
};

class MainConfigPage7
  : public lay::ConfigPage
{
Q_OBJECT

public:
  MainConfigPage7 (QWidget *parent);
  ~MainConfigPage7 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::MainConfigPage7 *mp_ui;
};

class CustomizeMenuConfigPage
  : public lay::ConfigPage
{
Q_OBJECT

public:
  CustomizeMenuConfigPage (QWidget *parent);
  ~CustomizeMenuConfigPage ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private slots:
  void current_changed (QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void item_changed (QTreeWidgetItem *, int);
  void text_changed ();
  void text_cleared ();
  void filter_changed ();
  void reset_clicked ();

private:
  Ui::CustomizeMenuConfigPage *mp_ui;
  std::map<std::string, std::string> m_current_bindings;
  std::map<std::string, bool> m_hidden_flags;
  std::map<std::string, QTreeWidgetItem *> m_item_for_path;
  std::map<QAction *, std::vector<std::string> > m_paths_for_action;
  bool m_enable_event;
  lay::Dispatcher *mp_dispatcher;

  void apply (const std::vector<std::pair<std::string, std::string> > &bindings, const std::vector<std::pair<std::string, bool> > &menu_items_hidden);
  void update_list_item (QTreeWidgetItem *item);
};

}

#endif

