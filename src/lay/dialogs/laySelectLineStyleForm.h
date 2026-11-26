#ifndef HDR_laySelectLineStyleForm
#define HDR_laySelectLineStyleForm

#include <QDialog>

#include "layLineStyles.h"

class QListWidgetItem;

namespace Ui
{
  class SelectLineStyleForm;
}

namespace lay
{

class SelectLineStyleForm
  : public QDialog
{
  Q_OBJECT 

public:
  SelectLineStyleForm (QWidget *parent, const lay::LineStyles &styles, bool include_nil = false);

  ~SelectLineStyleForm ();

  int selected () const
  {
    return m_selected;
  }

  void set_selected (int selected);
  
public slots:
  void sel_changed (QListWidgetItem *current, QListWidgetItem *); 

protected:
  void update ();

private:
  Ui::SelectLineStyleForm *mp_ui;
  int m_selected;
  lay::LineStyles m_styles;
  bool m_include_nil;
};

}

#endif

