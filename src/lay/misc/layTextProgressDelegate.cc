#include "layTextProgressDelegate.h"
#include "detail/utilsProgress.h"
#include "layMainWindow.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  TextProgressDelegate implementation

TextProgressDelegate::TextProgressDelegate (MainWindow *mw, int verbosity)
  : lay::TextProgress (verbosity), mp_mw (mw)
{
  //  .. nothing yet ..
}

void TextProgressDelegate::update_progress (geo::Progress *progress)
{
  // if (!mp_mw->update_progress (progress)) {
  //   lay::TextProgress::update_progress (progress);
  // }
}

void TextProgressDelegate::show_progress_bar (bool show)
{
  // if (!mp_mw->show_progress_bar (show)) {
  //   lay::TextProgress::show_progress_bar (show);
  // }
}

bool TextProgressDelegate::progress_wants_widget () const
{
  return mp_mw != 0 && mp_mw->progress_wants_widget ();
}

void TextProgressDelegate::progress_add_widget (QWidget *widget)
{
  if (mp_mw) {
    mp_mw->progress_add_widget (widget);
  }
}

QWidget *TextProgressDelegate::progress_get_widget () const
{
  return mp_mw ? mp_mw->progress_get_widget () : 0;
}

void TextProgressDelegate::progress_remove_widget ()
{
  if (mp_mw) {
    mp_mw->progress_remove_widget ();
  }
}



}
