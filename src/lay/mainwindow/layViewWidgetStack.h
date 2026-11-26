#ifndef HDR_layViewWidgetStack
#define HDR_layViewWidgetStack

#include <QWidget>

class QLabel;

namespace lay
{

class LayoutView;

class ViewWidgetStack
  : public QWidget
{
public:
  ViewWidgetStack (QWidget *parent = 0, const char *name = 0);

  void add_widget (LayoutView *w);
  void remove_widget (size_t index);
  void raise_widget (size_t index);
  LayoutView *widget (size_t index);
  QWidget *background_widget ();

protected:
  virtual void resizeEvent (QResizeEvent *)
  {
    resize_children ();
  }

  void resize_children ();

  std::vector <LayoutView *> m_widgets;
  QLabel *mp_bglabel;
};

}

#endif
