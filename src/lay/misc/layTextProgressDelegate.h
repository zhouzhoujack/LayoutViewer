#ifndef HDR_layTextProgressDelegate
#define HDR_layTextProgressDelegate

#include "layTextProgress.h"
#include "detail/utilsProgress.h"

namespace lay
{

class MainWindow;

class TextProgressDelegate
  : public lay::TextProgress
{
public:
  TextProgressDelegate (MainWindow *mw, int verbosity);

  virtual void update_progress (geo::Progress *progress);
  virtual void show_progress_bar (bool show);
  virtual bool progress_wants_widget () const;
  virtual void progress_add_widget (QWidget *widget);
  virtual QWidget *progress_get_widget () const;
  virtual void progress_remove_widget ();

private:
  MainWindow *mp_mw;
};

}

#endif
