#ifndef HDR_edtPartialService
#define HDR_edtPartialService

#include "layEditorServiceBase.h"
#include "layObjectInstPath.h"
#include "layViewObject.h"
#include "layRubberBox.h"
#include "laySnap.h"
#include "tlAssert.h"
#include "layEdtUtils.h"
#include "layEdtConfig.h"

#include <QObject>
#include <QTimer>

namespace geo {
  class Manager;
}

namespace lay {
  class Dispatcher;
  class Marker;
  class InstanceMarker;
}

namespace edt {

// -------------------------------------------------------------
//  A point with an unsigned index describing a certain point of a polygon or path

struct PointWithIndex 
  : public geo::Point
{
  PointWithIndex ()
    : geo::Point (), n (0), c (0)
  { }

  PointWithIndex (const geo::Point &p, unsigned int _n, unsigned int _c)
    : geo::Point (p), n (_n), c (_c)
  { }

  unsigned int n, c;

  bool operator== (const PointWithIndex &d) const
  {
    if (n != d.n) {
      return false;
    }
    return geo::Point::operator== (d);
  }

  bool operator< (const PointWithIndex &d) const
  {
    if (n != d.n) {
      return n < d.n;
    }
    return geo::Point::operator< (d);
  }
};

// -------------------------------------------------------------
//  An edge with two indices describing an edge of a polygon or segment of a path

struct EdgeWithIndex 
  : public geo::Edge
{
  EdgeWithIndex ()
    : geo::Edge (), n (0), nn (0), c (0) 
  { }

  EdgeWithIndex (const geo::Edge &e, unsigned int _n, unsigned int _nn, unsigned int _c)
    : geo::Edge (e), n (_n), nn (_nn), c (_c) 
  { }

  unsigned int n, nn, c;

  PointWithIndex pi1 () const
  {
    return PointWithIndex (p1 (), n, c);
  }

  PointWithIndex pi2 () const
  {
    return PointWithIndex (p2 (), nn, c);
  }

  bool operator== (const EdgeWithIndex &d) const
  {
    if (n != d.n || nn != d.nn || c != d.c) {
      return false;
    }
    return geo::Edge::operator== (d);
  }

  bool operator< (const EdgeWithIndex &d) const
  {
    if (n != d.n) {
      return n < d.n;
    }
    if (nn != d.nn) {
      return nn < d.nn;
    }
    if (c != d.c) {
      return c < d.c;
    }
    return geo::Edge::operator< (d);
  }
};

// -------------------------------------------------------------

/**
 *  @brief The partial selection and manipulation service
 */
class PartialService
  : public QObject,
    public lay::EditorServiceBase,
    public geo::Object
{
Q_OBJECT

public: 
  typedef std::map<lay::ObjectInstPath, std::set<EdgeWithIndex> > partial_objects;

  /**
   *  @brief The constructor
   */
  PartialService (geo::Manager *manager, lay::LayoutView *view, lay::Dispatcher *root);

  /**
   *  @brief The destructor
   */
  ~PartialService ();

  /**
   *  @brief Access to the view object
   */
  lay::LayoutView *view () const
  {
    tl_assert (mp_view != 0);
    return mp_view;
  }

  /**
   *  @brief Obtain the lay::ViewService interface
   */
  lay::ViewService *view_service_interface ()
  {
    return this;
  }

  /**
   *  @brief Obtain the lay::Editable interface
   */
  lay::Editable *editable_interface ()
  {
    return this;
  }

  /** 
   *  @brief Implementation of the menu functions
   */
  virtual void menu_activated (const std::string &symbol);

  /**
   *  @brief Implementation of "Plugin" interface: configuration setup
   */
  bool configure (const std::string &name, const std::string &value);

  /**
   *  @brief Implementation of "Plugin" interface: configuration finalization
   */
  void config_finalize ();

  /**
   *  @brief Implement the wheel event (for resetting hove state)
   */
  virtual bool wheel_event (int delta, bool horizontal, const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Implement the mouse mode: move event
   */
  virtual bool mouse_move_event (const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Implement the mouse mode: button press event
   */
  virtual bool mouse_press_event (const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Implement the mouse mode: button clicked (pressed and released)
   */
  virtual bool mouse_click_event (const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Implement the mouse mode: button double clicked 
   */
  virtual bool mouse_double_click_event (const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Implement the mouse mode: button release
   */
  virtual bool mouse_release_event (const geo::DPoint &p, unsigned int buttons, bool prio);

  /**
   *  @brief Transforms the selection
   *
   *  Currently only displacements are allowed which basically moves the partial selection 
   *  by the given distance
   */
  virtual void transform (const geo::DCplxTrans &tr);

  /**
   *  @brief Gets the catch distance (for single click)
   */
  virtual double catch_distance ();

  /**
   *  @brief Gets the catch distance (for box)
   */
  virtual double catch_distance_box ();

  /**
   *  @brief Indicates whether objects are selected
   */
  virtual bool has_selection ();

  /**
   *  @brief Indicates how many objects are selected
   */
  virtual size_t selection_size ();

  /**
   *  @brief Indicates whether objects are selected in transient mode
   */
  virtual bool has_transient_selection ();

  /**
   *  @brief Implement the "select" method at least to clear the selection
   */
  virtual bool select (const geo::DBox &box, SelectionMode mode);

  /** 
   *  @brief "delete" operation
   */
  virtual void del ();

  /**
   *  @brief Implement the mouse mode: deactivate mouse mode
   */
  virtual void deactivated ();

  /**
   *  @brief Implement the mouse mode: mode activated
   */
  virtual void activated ();

  /**
   *  @brief Reimplementation of the ViewService interface: set the colors
   */
  virtual void set_colors (QColor background, QColor text);

  /**
   *  @brief Cancel any edit operations (in this case, unselect all & cancel any drag operation)
   */
  virtual void edit_cancel ();

public slots:
  void timeout ();

protected:
  lay::angle_constraint_type connect_ac () const;
  lay::angle_constraint_type move_ac () const;

private:
  //  The layout view that this service is attached to
  lay::LayoutView *mp_view;
  lay::Dispatcher *mp_root;
  bool m_dragging;
  bool m_keep_selection;
  geo::DPoint m_start, m_current;
  geo::DPoint m_p1, m_p2;
  lay::RubberBox *mp_box;
  unsigned int m_color;
  unsigned int m_buttons;

  //  Angle constraints and grids
  lay::angle_constraint_type m_connect_ac, m_move_ac, m_alt_ac;
  geo::DVector m_edit_grid;
  bool m_snap_to_objects;
  geo::DVector m_global_grid;
  bool m_top_level_sel;

  //  The selection
  partial_objects m_selection;

  //  The marker objects representing the selection
  std::vector<lay::Marker *> m_markers;
  std::vector<lay::Marker *> m_transient_markers;
  std::vector<lay::InstanceMarker *> m_inst_markers;
  std::vector<lay::InstanceMarker *> m_transient_inst_markers;

  QTimer m_timer;
  bool m_hover;
  bool m_hover_wait;
  geo::DPoint m_hover_point;

  void hover_reset ();

  void clear_partial_transient_selection ();
  bool partial_select (const geo::DBox &box, lay::Editable::SelectionMode mode);
  void selection_to_view ();

  geo::DPoint snap (const geo::DPoint &p) const;
  geo::DVector snap (const geo::DVector &p) const;
  lay::PointSnapToObjectResult snap2 (const geo::DPoint &p) const;

  void enter_edge (const EdgeWithIndex &e, size_t &nmarker, partial_objects::const_iterator sel, const std::map <PointWithIndex, geo::Point> &new_points, const std::map <EdgeWithIndex, geo::Edge> &new_edges, const geo::ICplxTrans &gt, const std::vector<geo::DCplxTrans> &tv, bool transient);
  void enter_vertices (size_t &nmarker, partial_objects::const_iterator sel, const std::map <PointWithIndex, geo::Point> &new_points, const std::map <EdgeWithIndex, geo::Edge> &new_edges, const geo::ICplxTrans &gt, const std::vector<geo::DCplxTrans> &tv, bool transient);
  void enter_polygon (geo::Polygon &p, size_t &nmarker, partial_objects::const_iterator sel, const std::map <PointWithIndex, geo::Point> &new_points, const std::map <EdgeWithIndex, geo::Edge> &new_edges, const geo::ICplxTrans &gt, const std::vector<geo::DCplxTrans> &tv, bool transient);
  void enter_path (geo::Path &p, size_t &nmarker, partial_objects::const_iterator sel, const std::map <PointWithIndex, geo::Point> &new_points, const std::map <EdgeWithIndex, geo::Edge> &new_edges, const geo::ICplxTrans &gt, const std::vector<geo::DCplxTrans> &tv, bool transient);
  lay::Marker *new_marker (size_t &nmarker, unsigned int cv_index, bool transient);
  lay::InstanceMarker *new_inst_marker (size_t &nmarker, unsigned int cv_index, bool transient);
  void resize_markers (size_t n, bool transient);
  void resize_inst_markers (size_t n, bool transient);
  bool is_single_point_selection () const;
  geo::DPoint single_selected_point () const;
  bool handle_guiding_shape_changes ();
  void transform_selection (const geo::DTrans &move_trans);
};

}

#endif

