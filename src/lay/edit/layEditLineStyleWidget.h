#ifndef HDR_layEditLineStyleWidget
#define HDR_layEditLineStyleWidget

#include <QFrame>
#include <stdint.h>

#include "detail/geoObject.h"

namespace lay
{

class EditLineStyleWidget
  : public QFrame, public geo::Object
{
  Q_OBJECT 

public:
  EditLineStyleWidget (QWidget *parent);

  virtual QSize sizeHint () const;
  virtual QSize minimumSize () const;

  void set_style (uint32_t s, unsigned int sx);

  void undo (geo::Op *op);
  void redo (geo::Op *op);

  uint32_t style () const
  {
    return m_style;
  }

  virtual void paintEvent (QPaintEvent *event);
  virtual void mouseMoveEvent (QMouseEvent *event);
  virtual void mousePressEvent (QMouseEvent *event);
  virtual void mouseReleaseEvent (QMouseEvent *event);

  void set_size (unsigned int sx);

  void set_readonly (bool ro);

  bool readonly () const
  {
    return m_readonly;
  }

  void clear ();
  void invert ();
  void fliph ();
  void shift (int dx);

  unsigned int sx () const
  {
    return m_sx;
  }

signals:
  void changed ();
  void size_changed ();

private:
  unsigned int m_last_mx;
  uint32_t m_last_style;
  unsigned int m_last_sx;
  bool m_last_style_saved;
  uint32_t m_style;
  bool m_clearing;
  bool m_readonly;
  unsigned int m_sx;

  void set_pixel (unsigned int x, bool value);
  bool get_pixel (int x);
  bool mouse_to_pixel (const QPoint &pt, unsigned int &x);
  void expand_style ();
};

}

#endif
