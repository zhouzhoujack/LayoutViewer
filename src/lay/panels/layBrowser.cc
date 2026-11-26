#include <QDialog>

#include "layBrowser.h"
#include "layLayoutView.h"

#include <vector>

namespace lay
{

#if QT_VERSION >= 0x050000
// -----------------------------------------------------------------------------
//  Browser implementation

Browser::Browser (lay::Dispatcher *root, lay::LayoutView *view, const char *name, Qt::WindowFlags fl)
#else
Browser::Browser (lay::Dispatcher *root, lay::LayoutView *view, const char *name, Qt::WFlags fl)
#endif
    //  TODO: clarify whether to keep the browsers as separate (potentially hidden) windows
  : QDialog (0 /*view*/, fl),
    lay::Plugin (view),
    m_active (false),
    mp_view (view),
    mp_root (root)
{
  QObject::setObjectName (QString::fromUtf8 (name));
}

Browser::~Browser ()
{
  if (active ()) {
    deactivated ();
  }
}

void 
Browser::activate ()
{
  if (! active ()) {
    m_active = true;
    activated ();
    QDialog::show ();
  }
}

void 
Browser::deactivate ()
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::hide ();
  }
}

void 
Browser::closeEvent (QCloseEvent *event)
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::closeEvent (event);
  }
}

void
Browser::accept ()
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::accept ();
  }
}

}

