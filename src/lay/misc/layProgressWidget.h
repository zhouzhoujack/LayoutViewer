#ifndef HDR_layProgressWidget
#define HDR_layProgressWidget

#include <QFrame>
#include "detail/utilsProgress.h"
#include <QToolButton>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>

#include "layProgress.h"
#include "layLogViewerDialog.h"

class QToolButton;
class QLabel;
class QToolButton;
class QGridLayout;
class QListView;
class QFrame;

namespace tl
{
  class Progress;
}

namespace lay
{

class ProgressBarWidget;

class ProgressWidget
  : public QFrame
{
Q_OBJECT

public:
  ProgressWidget (ProgressReporter *pr, QWidget *parent, bool full_width = false);

  void set_progress (geo::Progress *progress);
  void add_widget (QWidget *widget);
  void remove_widget ();
  QWidget *get_widget () const;
  void set_full_width (bool fw);
  bool full_width () const;

  QSize sizeHint () const;

public slots:
  void signal_break ();

private:
  QLabel *mp_label;
  QFrame *mp_progress_bar_frame;
  ProgressBarWidget *mp_progress_bar1, *mp_progress_bar2, *mp_progress_bar3;
  QWidget *mp_widget;
  int m_widget_col;
  QGridLayout *mp_layout;
  QToolButton *mp_cancel_button;
  ProgressReporter *mp_pr;
  lay::LogFile m_log_file;
  QLabel *mp_log_label;
  QFrame *mp_log_frame;
  bool m_full_width;
  int m_left_col, m_right_col;
  bool m_log_visible;

  void set_log_visible (geo::Progress *progress);
};

}

#endif

