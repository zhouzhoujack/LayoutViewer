#ifndef HDR_layTechSetupDialog
#define HDR_layTechSetupDialog

#include "layTechnology.h"
#include "layStream.h"

#include <memory>

#include <QDialog>

namespace geo
{
  class Technology;
}

namespace lym
{
  class Macro;
  class MacroCollection;
}

namespace Ui
{
  class TechBaseEditorPage;
  class TechComponentSetupDialog;
  class TechSetupDialog;
  class TechSaveOptionsEditorPage;
  class TechLoadOptionsEditorPage;
  class TechMacrosPage;
}

class QLabel;
class QModelIndex;
class QTreeWidgetItem;

namespace lay
{

class TechnologyComponentEditor;

class TechBaseEditorPage
  : public TechnologyComponentEditor
{
Q_OBJECT

public:
  TechBaseEditorPage (QWidget *parent);
  ~TechBaseEditorPage ();

  virtual void setup ();
  virtual void commit ();

private slots:
  void browse_clicked ();
  void browse_lyp_clicked ();

private:
  Ui::TechBaseEditorPage *mp_ui;
};

class TechMacrosPage
  : public TechnologyComponentEditor
{
Q_OBJECT

public:
  TechMacrosPage (QWidget *parent, const std::string &cat, const std::string &cat_desc);
  ~TechMacrosPage ();

  virtual void setup ();
  virtual void commit ();

private:
  Ui::TechMacrosPage *mp_ui;
  std::string m_cat, m_cat_desc;
  std::vector<std::pair<QLabel *, QString> > m_original_labels;
  std::unique_ptr<lym::MacroCollection> mp_collection;
  std::string m_current_path;

private slots:
  void macro_selected (const QModelIndex &index);
  void create_folder_clicked ();
};

class TechLoadOptionsEditorPage
  : public TechnologyComponentEditor
{
Q_OBJECT

public:
  TechLoadOptionsEditorPage (QWidget *parent);
  ~TechLoadOptionsEditorPage ();

  virtual void setup ();
  virtual void commit ();

private:
  Ui::TechLoadOptionsEditorPage *mp_ui;
  std::vector< std::pair<lay::StreamReaderOptionsPage *, std::string> > m_pages;
};

class TechSaveOptionsEditorPage
  : public TechnologyComponentEditor
{
Q_OBJECT

public:
  TechSaveOptionsEditorPage (QWidget *parent);
  ~TechSaveOptionsEditorPage ();

  virtual void setup ();
  virtual void commit ();

private:
  Ui::TechSaveOptionsEditorPage *mp_ui;
  std::vector< std::pair<lay::StreamWriterOptionsPage *, std::string> > m_pages;
};

class TechSetupDialog
  : public QDialog
{
Q_OBJECT

public:
  TechSetupDialog (QWidget *parent);
  ~TechSetupDialog ();

  int exec_dialog (geo::Technologies &technologies);

protected slots:
  void current_tech_changed (QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void add_clicked ();
  void delete_clicked ();
  void rename_clicked ();
  void import_clicked ();
  void export_clicked ();
  void refresh_clicked ();

private:
  void update_tech_tree ();
  void update_tech (geo::Technology *tech);
  void update_tech_component ();
  void accept ();
  geo::Technology *selected_tech ();
  void select_tech (const geo::Technology &tech);
  std::string selected_tech_component_name ();
  void commit_tech_component ();
  void clear_components ();
  void update ();

  Ui::TechSetupDialog *mp_ui;
  geo::Technologies m_technologies;
  geo::Technology *mp_current_tech;
  std::map <std::string, lay::TechnologyComponentEditor *> m_component_editors;
  std::map <std::string, geo::TechnologyComponent *> m_technology_components;
  lay::TechnologyComponentEditor *mp_current_editor;
  geo::TechnologyComponent *mp_current_tech_component;
  bool m_current_tech_changed_enabled;
};

class TechComponentSetupDialog
  : public QDialog
{
public:
  TechComponentSetupDialog (QWidget *parent, geo::Technology *tech, const std::string &component_name);
  ~TechComponentSetupDialog ();

protected:
  void accept ();

private:
  Ui::TechComponentSetupDialog *mp_ui;
  geo::Technology *mp_tech;
  geo::TechnologyComponent *mp_component;
  lay::TechnologyComponentEditor *mp_editor;
};

}

#endif


