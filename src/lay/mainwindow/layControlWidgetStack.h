#ifndef HDR_layControlWidgetStack
#define HDR_layControlWidgetStack

#include <QtWidgets/QFrame>

class QLabel;

namespace lay
{

class ControlWidgetStack
  : public QFrame
{
public:
  ControlWidgetStack (QWidget *parent = 0, const char *name = 0);

  void focusInEvent (QFocusEvent *);

  QSize sizeHint () const;

  void add_widget (QWidget *w);
  void remove_widget (size_t index);
  void raise_widget (size_t index);
  QWidget *widget (size_t index);
  QWidget *background_widget ();

  QWidget *currentWidget () const
  {
    return mp_current_widget;
  }

  size_t count () const
  {
    return m_widgets.size ();
  }

protected:
  virtual void resizeEvent (QResizeEvent *)
  {
    resize_children ();
  }

  void resize_children ();

  std::vector <QWidget *> m_widgets;
  QWidget *mp_current_widget;
  QLabel *mp_bglabel;
};

}

#endif
