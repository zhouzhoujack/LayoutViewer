#ifndef HDR_laySnap
#define HDR_laySnap


#include <QPoint>

#include <vector>

#include "detail/geoPoint.h"
#include "detail/geoVector.h"
#include "detail/geoEdge.h"
#include "detail/geoTypes.h"

namespace lay
{
  class LayoutView;

  /**
   *  @brief An angle constraint type
   *
   *  Any: no angle constraint
   *  Diagonal: vertical, horizontal and 45 degree diagonals
   *  Ortho: vertical and horizontal
   *  Horizontal: horizontal only
   *  Vertical: vertical only
   *  Global: use global setting (templates and ruler specific setting only)
   */
  enum angle_constraint_type { AC_Any = 0, AC_Diagonal, AC_Ortho, AC_Horizontal, AC_Vertical, AC_Global, AC_NumModes };

  /**
   *  @brief snap a coordinate value to a unit grid
   */
  inline geo::DCoord snap (geo::DCoord c)
  {
    return floor (c + 0.5 + geo::coord_traits<double>::prec ());
  }

  /**
   *  @brief snap a coordinate value to a unit grid
   */
  inline geo::DPoint snap (const geo::DPoint &p)
  {
    return geo::DPoint (snap (p.x ()), snap (p.y ()));
  }

  /**
   *  @brief snap a coordinate value to the given grid
   */
  geo::DCoord snap (geo::DCoord c, geo::DCoord grid);

  /**
   *  @brief snap a point value to the given grid
   */
  geo::DPoint snap (const geo::DPoint &p, geo::DCoord grid);

  /**
   *  @brief snap a point value to the given (anisotropic) grid
   */
  geo::DPoint snap_xy (const geo::DPoint &p, const geo::DPoint &grid);

  /**
   *  @brief snap a point value to the given (anisotropic) grid
   */
  inline geo::DPoint snap_xy (const geo::DPoint &p, const geo::DVector &grid)
  {
    return snap_xy (p, geo::DPoint (grid.x (), grid.y ()));
  }

  /**
   *  @brief snapping of a two-point vector 
   *
   *  The snapping algorithm used ensures maximum fidelity of the
   *  resulting vector with respect to the original (angle-wise and length-wise).
   */
  std::pair<geo::DPoint, geo::DPoint> snap (const geo::DPoint &p1, const geo::DPoint &p2);

  /**
   *  @brief snapping of a two-point vector to a grid
   *
   *  The snapping algorithm used ensures maximum fidelity of the
   *  resulting vector with respect to the original (angle-wise and length-wise).
   */
  std::pair<geo::DPoint, geo::DPoint> snap (const geo::DPoint &p1, const geo::DPoint &p2, geo::DCoord grid);

  /**
   *  @brief A structure describing the snap result for a point-wise snap
   */
  struct PointSnapToObjectResult
  {
    enum ObjectSnap {
      NoObject = 0,
      ObjectVertex,
      ObjectEdge,
      ObjectUnspecific
    };

    PointSnapToObjectResult () : object_snap (NoObject) { }

    /**
     *  @brief The result of the snap
     */
    geo::DPoint snapped_point;

    /**
     *  @brief Indicates whether and how the point was snapped to an object
     */
    ObjectSnap object_snap;

    /**
     *  @brief Indicates the edge the point was snapped against unless in NoObject mode
     */
    geo::DEdge object_ref;
  };

  /**
   *  @brief combined grid-, projection- and object snapping provided to implementing "magnetic features"
   *
   *  This method will snap a point pt to the grid, project it to one of the cutlines or try to snap it 
   *  to an object edge or point. It will use a heuristics to determine
   *  what criterion is applied if a conflict is detected.
   *
   *  The function will return a PointSnapToObjectResult object.
   *
   *  @param view The layout view used for object snapping. Can be 0 to disable object snapping
   *  @param pt The point to snap
   *  @param grid Either (0,0) to disable grid snapping or a (gx,gy) value for the (potentially anisotropic grid)
   *  @param snap_range The search range for objects 
   */
  PointSnapToObjectResult obj_snap (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, double snap_range);

  /**
   *  @brief combined grid-, projection- and object snapping provided to implementing "magnetic features"
   *
   *  This is a convenience method that creates the projection axes from a reference point and an angle mode.
   *  "pr" is the reference point, "pt" is the point to snap.
   */
  PointSnapToObjectResult obj_snap (lay::LayoutView *view, const geo::DPoint &pr, const geo::DPoint &pt, const geo::DVector &grid, lay::angle_constraint_type ac, double snap_range);

  /**
   *  @brief A structure describing the snap result for a two-sided object snap (distance measurement)
   */
  struct TwoPointSnapToObjectResult
  {
    enum ObjectSnap {
      NoObject = 0,
      ObjectVertex,
      ObjectEdge,
      ObjectUnspecific
    };

    TwoPointSnapToObjectResult () : any (false), object_snap_first (NoObject), object_snap_second (NoObject) { }

    /**
     *  @brief Indicates whether the two-sided snap was successful
     */
    bool any;

    /**
     *  @brief The result of the snap
     *  Two values are provided for the first and second snap.
     */
    geo::DPoint first, second;

    /**
     *  @brief Indicates whether and how the point was snapped to an object
     *  Two values are provided for the first and second snap.
     */
    ObjectSnap object_snap_first, object_snap_second;

    /**
     *  @brief Indicates the edge the point was snapped against unless in NoObject mode
     *  Two values are provided for the first and second snap.
     */
    geo::DEdge object_ref_first, object_ref_second;
  };

  /**
   *  @brief Same than obj_snap, but delivers two points on two opposite sides of the initial point
   *
   *  This method basically implements "auto measure". The first value of the returned pair
   *  is true if such an edge could be found. Otherwise it's false.
   */
  TwoPointSnapToObjectResult obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, double min_search_range, double max_search_range);

  /**
   *  @brief Same than obj_snap, but delivers two points on two opposite sides of the initial points
   *
   *  This method basically implements "auto measure". The first value of the returned pair
   *  is true if such an edge could be found. Otherwise it's false.
   *
   *  This version accepts two points defining different search regions for first and second edge.
   */
  TwoPointSnapToObjectResult obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt1, const geo::DPoint &pt2, const geo::DVector &grid, double min_search_range, double max_search_range);

  /**
   *  @brief Same than the previous obj_snap2, but allows specification of an angle constraint
   *
   *  Measurements will be confined to the direction specified.
   */
  TwoPointSnapToObjectResult obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, lay::angle_constraint_type ac, double min_search_range, double max_search_range);

  /**
   *  @brief Same than the previous obj_snap2, but allows specification of an angle constraint
   *
   *  Measurements will be confined to the direction specified.
   *
   *  This version accepts two points defining different search regions for first and second edge.
   */
  TwoPointSnapToObjectResult obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt1, const geo::DPoint &pt2, const geo::DVector &grid, lay::angle_constraint_type ac, double min_search_range, double max_search_range);

  /**
   *  @brief Reduce a given vector according to the angle constraint
   */
  geo::DVector snap_angle (const geo::DVector &in, lay::angle_constraint_type ac);

  /**
   *  @brief rounding of a double value for drawing purposes
   */
  int draw_round (double x);

  /**
   *  @brief rounding (and height-transformation) of a double point 
   */
  QPoint draw_round (geo::DPoint p, int h);

  /**
   *  @brief rounding (and height-transformation) of a two-point vector
   */
  std::pair<QPoint, QPoint> draw_round (const geo::DPoint &p1, const geo::DPoint &p2, int h);

  /**
   *  @brief rounding (and height-transformation) of a two-point vector
   */
  std::pair<geo::DPoint, geo::DPoint> draw_round_dbl (const geo::DPoint &p1, const geo::DPoint &p2, int h);
}

#endif


