#ifndef HDR_layProgress
#define HDR_layProgress


#include <string>
#include "detail/utilsObject.h"
#include "detail/utilsProgress.h"
#include <list>
#include <map>
#include <set>

#include "tlProgress.h"
#include "tlObject.h"
#include "tlTimer.h"

namespace lay
{

class ProgressReporter;

/**
 *  @brief This interface provides the actual implementation of the progress bar
 */
class ProgressBar
  : public geo::RefCounted
{
public:
  virtual ~ProgressBar () { }

  virtual void update_progress (geo::Progress *progress) = 0;
  virtual bool progress_wants_widget () const { return false; }
  virtual void progress_add_widget (QWidget * /*widget*/) { }
  virtual void progress_remove_widget () { }
  virtual QWidget *progress_get_widget () const { return 0; }
  virtual void show_progress_bar (bool show) = 0;
};

class ProgressReporter 
  : public QObject, public geo::ProgressAdaptor
{
public:
  ProgressReporter ();
  virtual ~ProgressReporter ();

  virtual void register_object (geo::Progress *progress);
  virtual void unregister_object (geo::Progress *progress);
  virtual void trigger (geo::Progress *progress);
  virtual void yield (geo::Progress *progress);
  virtual bool eventFilter (QObject *dest, QEvent *event);

  void set_progress_bar (lay::ProgressBar *pb);

private:
  lay::ProgressBar *mp_pb;
  bool m_pw_visible;
  std::map<geo::Progress *, tl::Clock> m_queued;
  std::set<geo::Progress *> m_active;

  void process_events ();
  void update_and_yield ();
  void set_visible (bool vis);
};

/**
 *  @brief Marks a widget as alive
 *  "alive" widgets receive input events also while a progress reporter is shown
 */
void mark_widget_alive (QWidget *w, bool alive);

}

#endif

