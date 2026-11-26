/*
 *  LayoutViewer - progress dialog implementation
 */

#include "layProgressDialog.h"
#include "detail/utilsInternational.h"
#include "detail/utilsProgress.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "tlProgress.h"

namespace lay
{

ProgressDialog::ProgressDialog(lay::ProgressReporter* reporter, QWidget* parent)
    : QDialog(parent), mp_reporter(reporter)
{
    setObjectName(QString::fromUtf8("progress_dialog"));
    setWindowTitle(tr("Loading..."));
    setModal(false);
    setMinimumWidth(360);

    QVBoxLayout* layout = new QVBoxLayout(this);

    mp_title_label = new QLabel(this);
    mp_title_label->setAlignment(Qt::AlignCenter);
    mp_title_label->setText(QString());
    layout->addWidget(mp_title_label);

    mp_progress_bar = new QProgressBar(this);
    mp_progress_bar->setRange(0, 100);
    mp_progress_bar->setTextVisible(true);
    mp_progress_bar->setMinimumHeight(24);

    mp_cancel_button = new QPushButton(tr("Cancel"), this);
    mp_cancel_button->setEnabled(false);

    //  progress bar with the cancel button to its right
    QHBoxLayout* bar_layout = new QHBoxLayout();
    bar_layout->addWidget(mp_progress_bar, 1);
    bar_layout->addWidget(mp_cancel_button, 0);
    layout->addLayout(bar_layout);

    connect(mp_cancel_button, &QPushButton::clicked, this, &ProgressDialog::cancel_clicked);

    //  keep the cancel button responsive while the progress reporter is
    //  blocking input events during the load
    mark_widget_alive(mp_cancel_button, true);
}

void ProgressDialog::update_progress(geo::Progress* progress)
{
    if (!progress)
    {
        //  the operation has finished -> close the dialog
        close();
        return;
    }

    if (!progress->desc().empty())
    {
        mp_title_label->setText(geo::to_qstring(progress->desc()));
    }

    mp_progress_bar->setRange(0, 100);
    mp_progress_bar->setValue(int(progress->value()));
    mp_progress_bar->setFormat(geo::to_qstring(progress->formatted_value()));

    //  only allow cancelling if the operation supports it
    mp_cancel_button->setEnabled(progress->can_cancel());

    if (!isVisible())
    {
        show();
    }
}

void ProgressDialog::show_progress_bar(bool visible)
{
    if (visible)
    {
        QDialog::show();
    }
    else
    {
        close();
    }
}

void ProgressDialog::cancel_clicked()
{
    if (mp_reporter)
    {
        mp_reporter->signal_break();
    }
}

} // namespace lay
