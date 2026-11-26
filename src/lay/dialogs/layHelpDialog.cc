#include "layHelpDialog.h"
#include "detail/utilsInternational.h"
#include "layHelpSource.h"
#include "layBrowserPanel.h"
#include "layDispatcher.h"
#include "layConfig.h"
#include "tlStaticObjects.h"
#include "ui_HelpDialog.h"

#include "tlString.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>

namespace lay
{

lay::HelpSource *HelpDialog::mp_help_source = 0;

// -----------------------------------------------------------------------------
//  HelpDialog implementation

HelpDialog::HelpDialog (QWidget *parent, bool modal)
  : QDialog (modal ? parent : 0 /*show as separate window*/, modal ? Qt::WindowFlags () : Qt::Window /*enabled minimize button*/),
    m_initialized (false)
{
  mp_ui = new Ui::HelpDialog ();
  mp_ui->setupUi (this);

  setModal (modal);

  mp_ui->button_frame->setVisible (modal);
  mp_ui->browser_panel->set_dispatcher (lay::Dispatcher::instance (), cfg_assistant_bookmarks);

  m_def_title = windowTitle ();
  connect (mp_ui->browser_panel, SIGNAL (title_changed (const QString &)), this, SLOT (title_changed (const QString &)));
  connect (mp_ui->browser_panel, SIGNAL (url_changed (const QString &)), this, SLOT (title_changed (const QString &)));
}

HelpDialog::~HelpDialog ()
{
  //  .. nothing yet ..
}

void HelpDialog::title_changed (const QString &)
{
  QString wt;

  QString title = geo::to_qstring (mp_ui->browser_panel->title ());
  if (title.isNull () || title.size () == 0) {
    wt = m_def_title;
  } else {
    wt = m_def_title + QString::fromUtf8 (" - ") + title;
  }

  QString url = geo::to_qstring (mp_ui->browser_panel->url ());
  if (! url.isNull () && url.size () > 0) {
    wt += QString::fromUtf8 (" [") + url + QString::fromUtf8 ("]");
  }

  setWindowTitle (wt);
}

void HelpDialog::load (const std::string &url)
{
  initialize ();
  mp_ui->browser_panel->load (url);
}

void HelpDialog::search (const std::string &topic)
{
  initialize ();
  mp_ui->browser_panel->search (topic);
}

void HelpDialog::showEvent (QShowEvent *)
{
  initialize ();
  if (! m_geometry.isNull ()) {
    setGeometry (m_geometry);
  }
}

void HelpDialog::initialize ()
{
  if (! m_initialized) {
    m_initialized = true;
    mp_ui->browser_panel->set_search_url ("int:/search.xml", "string");
    if (! mp_help_source) {
      mp_help_source = new lay::HelpSource ();
      tl::StaticObjects::reg (&mp_help_source);
    }
    mp_ui->browser_panel->set_source (mp_help_source);
    mp_ui->browser_panel->set_home ("int:/index.xml");
  }
}

void HelpDialog::hideEvent (QHideEvent *)
{
  m_geometry = geometry ();
}

}
