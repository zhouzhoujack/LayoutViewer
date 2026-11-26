#ifndef HDR_layLayoutPropertiesForm
#define HDR_layLayoutPropertiesForm

#include <QDialog>
#include "ui_LayoutProperties.h"
#include "layCellView.h"

#include <vector>

namespace lay {

class LayoutView;

class LayoutPropertiesForm
  : public QDialog, private Ui::LayoutPropertiesForm
{
  Q_OBJECT 

public:
  LayoutPropertiesForm (QWidget *parent, lay::LayoutView *view, const char *name);

  void accept ();
  void commit ();

public slots:
  void layout_selected (int);

private slots:
  void prop_pb_clicked ();

private:
  std::vector <lay::LayoutHandleRef> m_handles;
  lay::LayoutView *mp_view;
  int m_index;
  bool m_editable;
};

}

#endif

