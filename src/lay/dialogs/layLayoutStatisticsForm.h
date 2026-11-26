#ifndef HDR_layLayoutStatisticsForm
#define HDR_layLayoutStatisticsForm

#include <QDialog>
#include "ui_LayoutStatistics.h"
#include "layCellView.h"

#include <vector>

namespace lay {

class LayoutView;
class StatisticsSource;

class LayoutStatisticsForm
  : public QDialog, private Ui::LayoutStatisticsForm
{
  Q_OBJECT 

public:
  LayoutStatisticsForm (QWidget *parent, lay::LayoutView *view, const char *name);
  ~LayoutStatisticsForm ();

public slots:
  void layout_selected (int);

private:
  std::vector <lay::LayoutHandleRef> m_handles;
  StatisticsSource *mp_source;
};

}

#endif

