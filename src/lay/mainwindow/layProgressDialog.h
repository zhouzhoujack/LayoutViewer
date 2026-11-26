/*
 *  LayoutViewer - progress dialog
 *
 *  A dialog that shows a progress bar while a layout file (e.g. GDS) is being
 *  loaded. It implements the lay::ProgressBar interface so it can be driven by
 *  the ProgressReporter: the dialog shows itself when progress is reported and
 *  closes automatically once the operation has finished.
 */

#ifndef HDR_layProgressDialog
#define HDR_layProgressDialog

#include <QDialog>
#include "detail/utilsProgress.h"

#include "layProgress.h"

class QLabel;
class QProgressBar;
class QPushButton;

namespace tl
{
class Progress;
}

namespace lay
{

class ProgressDialog
    : public QDialog, public lay::ProgressBar
{
    Q_OBJECT

public:
    ProgressDialog(lay::ProgressReporter* reporter, QWidget* parent = 0);

    //  lay::ProgressBar interface
    void update_progress(geo::Progress* progress);
    void show_progress_bar(bool show);

private slots:
    void cancel_clicked();

private:
    lay::ProgressReporter* mp_reporter;
    QLabel* mp_title_label;
    QProgressBar* mp_progress_bar;
    QPushButton* mp_cancel_button;
};

} // namespace lay

#endif
