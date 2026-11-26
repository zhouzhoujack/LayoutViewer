#ifndef HDR_layRedrawThread
#define HDR_layRedrawThread

#include <vector>
#include "detail/utilsObject.h"
#include <memory>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "geoBox.h"
#include "detail/geoTrans.h"
#include "geoLayout.h"
#include "layRenderer.h"
#include "layLayoutView.h"
#include "layRedrawThreadCanvas.h"
#include "layRedrawLayerInfo.h"
#include "layCanvasPlane.h"
#include "tlTimer.h"
#include "tlThreadedWorkers.h"

namespace lay {

class Viewport;

//  update (snapshot) interval in ms
const int update_interval = 500;

class RedrawThread 
  : public geo::RefCounted,
    public tl::JobBase
{
public:
  RedrawThread (lay::RedrawThreadCanvas *canvas, lay::LayoutView *view);
  virtual ~RedrawThread ();

  void commit (const std::vector <lay::RedrawLayerInfo> &layers, const lay::Viewport &vp, double resolution);
  void start (int workers, const std::vector <lay::RedrawLayerInfo> &layers, const lay::Viewport &vp, double resolution, bool force_redraw);
  void restart (const std::vector<int> &restart);
  void wakeup_checked ();
  void wakeup ();

  /**
   *  @brief change the visibility of entries in the redrawing queue
   *
   *  HINT: this should be done only when the redraw thread is stopped.
   */
  void change_visibility (const std::vector<bool> &visibility);

  const RedrawLayerInfo &get_layer_info (int id) const
  {
    return m_layers [id];
  }

  int num_layers () const
  {
    return int (m_layers.size ());
  }

  void task_finished (int id);

protected:
  tl::Worker *create_worker ();
  void setup_worker (tl::Worker *worker);
  void finished ();
  void stopped ();

private:
  void start ();
  void do_start (bool clear, const geo::Vector *shift_vector, const std::vector <lay::RedrawLayerInfo> *layers, const std::vector<int> &restart, int workers);
  void done ();

  void layout_changed ();

  void layout_changed_with_int (int)
  {
    layout_changed ();
  }

  bool m_initial_update;
  std::vector <RedrawLayerInfo> m_layers;
  int m_nlayers;
  bool m_boxes_already_drawn;
  bool m_custom_already_drawn;

  geo::DCplxTrans m_vp_trans;
  int m_width, m_height;
  double m_resolution;
  std::vector<geo::Box> m_redraw_regions;
  geo::DBox m_stored_region, m_valid_region;
  geo::DPoint m_last_center;
  geo::DFTrans m_stored_fp;

  lay::RedrawThreadCanvas *mp_canvas;
  lay::LayoutView *mp_view;
  bool m_start_recursion_sentinel;

  tl::Clock m_clock;
  QMutex m_initial_wait_lock;
  QWaitCondition m_initial_wait_cond;

  std::unique_ptr<tl::SelfTimer> m_main_timer;
};

}

#endif

