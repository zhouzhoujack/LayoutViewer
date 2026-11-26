#ifndef HDR_laySelectStippleForm
#define HDR_laySelectStippleForm

#include <QDialog>

#include "layDitherPattern.h"

class QListWidgetItem;

namespace Ui
{
  class SelectStippleForm;
}

namespace lay
{

class SelectStippleForm
  : public QDialog
{
  Q_OBJECT 

public:
  SelectStippleForm (QWidget *parent, const lay::DitherPattern &pattern, bool include_nil = false);

  ~SelectStippleForm ();

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
  Ui::SelectStippleForm *mp_ui;
  int m_selected;
  lay::DitherPattern m_pattern;
  bool m_include_nil;
};

}

#endif

