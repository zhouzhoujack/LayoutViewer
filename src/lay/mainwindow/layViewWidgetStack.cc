#include "layViewWidgetStack.h"
#include "layLayoutView.h"

#include <QLabel>

namespace lay
{

// -----------------------------------------------------------------------------
//  ViewWidgetStack implementation

ViewWidgetStack::ViewWidgetStack (QWidget *parent, const char *name)
  : QWidget (parent)
{
  setObjectName (QString::fromUtf8 (name));

  mp_bglabel = new QLabel (this);
  mp_bglabel->setAutoFillBackground (true);
  mp_bglabel->setText (QObject::tr ("<html><body><p style=\"font-size: 12px;\">Use File/Open to open a layout</p></body></html>"));
  mp_bglabel->setAlignment (Qt::AlignVCenter | Qt::AlignHCenter);
  mp_bglabel->show ();
}

void ViewWidgetStack::add_widget (LayoutView *w)
{
  m_widgets.push_back (w);
  w->setParent (this);
  resize_children ();
  raise_widget (m_widgets.size () - 1);

  updateGeometry ();
}

void ViewWidgetStack::remove_widget (size_t index)
{
  if (index < m_widgets.size ()) {
    m_widgets.erase (m_widgets.begin () + index);
  }
  if (m_widgets.size () == 0) {
    mp_bglabel->show ();
  }
}

void ViewWidgetStack::raise_widget (size_t index)
{
  if (index < m_widgets.size ()) {
    mp_bglabel->hide ();
    m_widgets [index]->show ();
  } else {
    mp_bglabel->show ();
  }

  size_t i = 0;
  for (std::vector <LayoutView *>::iterator child = m_widgets.begin (); child != m_widgets.end (); ++child, ++i) {
    if (i != index) {
      (*child)->hide ();
    }
  }
}

LayoutView *ViewWidgetStack::widget (size_t index)
{
  if (index < m_widgets.size ()) {
    return m_widgets [index];
  } else {
    return 0;
  }
}

QWidget *ViewWidgetStack::background_widget ()
{
  return mp_bglabel;
}

void ViewWidgetStack::resize_children ()
{
  //  set the geometry of all children
  for (std::vector <LayoutView *>::iterator child = m_widgets.begin (); child != m_widgets.end (); ++child) {
    (*child)->setGeometry (0, 0, width (), height ());
  }
  mp_bglabel->setGeometry (0, 0, width (), height ());
}

}
