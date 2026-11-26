#include "layControlWidgetStack.h"

#include <QtWidgets/QLabel>

namespace lay
{

// -----------------------------------------------------------------------------
//  ControlWidgetStack implementation

ControlWidgetStack::ControlWidgetStack(QWidget *parent, const char *name)
  : QFrame (parent), mp_current_widget (0)
{
  setObjectName (QString::fromUtf8 (name));

  //  Background ist a simple label without a text currently
  mp_bglabel = new QLabel (this);
  mp_bglabel->setAutoFillBackground (true);
  mp_bglabel->setAlignment (Qt::AlignVCenter | Qt::AlignHCenter);
  QPalette bg_palette = mp_bglabel->palette ();
  bg_palette.setColor (QPalette::Window, QColor (255, 255, 255));
  mp_bglabel->setPalette (bg_palette);
  mp_bglabel->show ();
}

void ControlWidgetStack::focusInEvent(QFocusEvent *)
{
  for (size_t i = 0; i < m_widgets.size (); ++i) {
    if (m_widgets [i]->isVisible ()) {
      m_widgets [i]->setFocus ();
      break;
    }
  }
}

void ControlWidgetStack::add_widget(QWidget *w)
{
  m_widgets.push_back (w);
  w->setParent (this);
  resize_children ();
  raise_widget (m_widgets.size () - 1);

  int mw = 0;
  for (size_t i = 0; i < m_widgets.size (); ++i) {
    mw = std::max (m_widgets [i]->sizeHint ().width (), mw);
    mw = std::max (m_widgets [i]->minimumWidth (), mw);
  }

  if (mw > minimumWidth ()) {
    setMinimumWidth (mw);
    resize (minimumWidth (), height ());
  }
}

QSize ControlWidgetStack::sizeHint() const
{
  int w = 0;
  for (size_t i = 0; i < m_widgets.size (); ++i) {
    w = std::max (m_widgets [i]->sizeHint ().width (), w);
  }
  return QSize (w, 0);
}

void ControlWidgetStack::remove_widget(size_t index)
{
  if (index < m_widgets.size ()) {
    if (mp_current_widget == m_widgets [index]) {
      mp_current_widget = 0;
    }
    m_widgets.erase (m_widgets.begin () + index);
  }
  if (m_widgets.size () == 0) {
    mp_bglabel->show ();
  }
}

void ControlWidgetStack::raise_widget(size_t index)
{
  mp_current_widget = 0;
  bool any_visible = false;
  for (size_t i = 0; i < m_widgets.size (); ++i) {
    if (m_widgets [i]) {
      if (i == index) {
        m_widgets [i]->show ();
        mp_current_widget = m_widgets [i];
        any_visible = true;
      } else {
        m_widgets [i]->hide ();
      }
    }
  }

  if (! any_visible) {
    mp_bglabel->show ();
  } else {
    mp_bglabel->hide ();
  }
}

QWidget *ControlWidgetStack::widget(size_t index)
{
  if (index < m_widgets.size ()) {
    return m_widgets [index];
  } else {
    return 0;
  }
}

QWidget *ControlWidgetStack::background_widget()
{
  return mp_bglabel;
}

void ControlWidgetStack::resize_children()
{
  //  set the geometry of all children
  for (std::vector <QWidget *>::iterator child = m_widgets.begin (); child != m_widgets.end (); ++child) {
    if (*child) {
      (*child)->setGeometry (0, 0, width (), height ());
    }
  }
  mp_bglabel->setGeometry (0, 0, width (), height ());
}

}
