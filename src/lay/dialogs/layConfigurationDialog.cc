#include <QDialog>
#include "tlException.h"
#include "tlClassRegistry.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include <QVBoxLayout>
#include <QMessageBox>

#include "ui_ConfigurationDialog.h"
#include "layConfigurationDialog.h"
#include "layPlugin.h"
#include "layDispatcher.h"
#include "layPluginDeclaration.h"
#include "layConfigPage.h"

#include "tlLog.h"

namespace lay
{

// -------------------------------------------------------------

// -----------------------------------------------------------------------------
//  ConfigurationDialog implementation

ConfigurationDialog::ConfigurationDialog (QWidget *parent, lay::Dispatcher *root, const std::string &plugin_name, const char *name)
  : QDialog (parent),
    mp_root (root)
{ 
  mp_ui = 0;

  setObjectName (QString::fromUtf8 (name));

  //  Error message in case no plugin is found
  std::string config_title = "ERROR: Unknown plugin with name " + plugin_name;
  setWindowTitle (geo::to_qstring ((geo::to_string (QObject::tr ("Configuration")) + " - " + config_title)));

  for (tl::Registrar<lay::PluginDeclaration>::iterator p = tl::Registrar<lay::PluginDeclaration>::begin (); p != tl::Registrar<lay::PluginDeclaration>::end (); ++p) {
    if (p.current_name () == plugin_name) {
      init (&*p);
      break;
    }
  }
}

ConfigurationDialog::ConfigurationDialog (QWidget *parent, lay::Dispatcher *root, lay::PluginDeclaration *decl, const char *name)
  : QDialog (parent),
    mp_root (root)
{ 
  setObjectName (QString::fromUtf8 (name));

  init (decl);
}

ConfigurationDialog::~ConfigurationDialog ()
{
  m_config_pages.clear ();
  delete mp_ui;
  mp_ui = 0;
}

void
ConfigurationDialog::init (const lay::PluginDeclaration *decl)
{
  mp_ui = new Ui::ConfigurationDialog ();
  mp_ui->setupUi (this);

  //  signals and slots connections
  connect( mp_ui->ok_button, SIGNAL( clicked() ), this, SLOT( ok_clicked() ) );
  connect( mp_ui->cancel_button, SIGNAL( clicked() ), this, SLOT( reject() ) );

  QVBoxLayout *layout = new QVBoxLayout (mp_ui->centralFrame);

  std::string config_title;

  lay::ConfigPage *page = decl->config_page (mp_ui->centralFrame, config_title);
  if (page) {
    m_config_pages.push_back (page);
    if (page->layout () == 0) {
      LOGW << "No layout in configuration page " << config_title;
    }
    layout->addWidget (page);
  }

  std::vector <std::pair <std::string, lay::ConfigPage *> > pages = decl->config_pages (mp_ui->centralFrame);
  for (std::vector <std::pair <std::string, lay::ConfigPage *> >::iterator p = pages.begin (); p != pages.end (); ++p) {
    m_config_pages.push_back (p->second);
    if (p->second->layout () == 0) {
      LOGW << "No layout in configuration page " << p->first;
    }
    layout->addWidget (p->second);
    config_title = p->first;
  }

  layout->addStretch (0);

  for (std::vector <lay::ConfigPage *>::iterator p = m_config_pages.begin (); p != m_config_pages.end (); ++p) {
    if ((*p)->layout ()) {
      (*p)->layout ()->setMargin (0);
    } 
    (*p)->setup (mp_root);
  }

  config_title = std::string (config_title, 0, config_title.find ("|"));
  setWindowTitle (geo::to_qstring (geo::to_string (QObject::tr ("Configuration")) + " - " + config_title));
}

void 
ConfigurationDialog::commit ()
{
  for (std::vector <lay::ConfigPage *>::iterator p = m_config_pages.begin (); p != m_config_pages.end (); ++p) {
    (*p)->commit (mp_root);
  }

  mp_root->config_end ();
}

void 
ConfigurationDialog::ok_clicked ()
{
  try {
    commit ();
    accept ();
  } catch (tl::Exception &ex) {
    LOGE << ex.msg ();
    QMessageBox::critical (this, QObject::tr ("Error"), geo::to_qstring (ex.msg ()));
  } 
}

}

