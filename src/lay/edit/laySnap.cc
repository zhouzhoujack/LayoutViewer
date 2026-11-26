#include "laySnap.h"
#include "layLayoutView.h"

#include "detail/geoEdge.h"

namespace lay
{
// -----------------------------------------------------------------------------
//  Snap implementation


geo::DCoord 
snap (geo::DCoord c, geo::DCoord grid)
{
  if (grid <= 1e-10) {
    return c;
  } else {
    return snap (c / grid) * grid;
  }
}

geo::DPoint 
snap (const geo::DPoint &p, geo::DCoord grid)
{
  if (grid <= 1e-10) {
    return p;
  } else {
    return snap (geo::DPoint (p.x () / grid, p.y () / grid)) * grid;
  }
}

geo::DPoint 
snap_xy (const geo::DPoint &p, const geo::DPoint &grid)
{
  if (grid.x () <= 1e-10 || grid.y () <= 1e-10) {
    return p;
  } else {
    geo::DPoint pp = snap (geo::DPoint (p.x () / grid.x (), p.y () / grid.y ()));
    return geo::DPoint (pp.x () * grid.x (), pp.y () * grid.y ());
  }
}

std::pair<geo::DPoint, geo::DPoint>
snap (const geo::DPoint &p1, const geo::DPoint &p2)
{
  if (fabs (p1.y () - p2.y ()) < 0.1) {
    
    double x1 = floor (p1.x () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    double y = floor (p1.y () + 0.5);
    return std::make_pair (geo::DPoint (x1, y), geo::DPoint (x2, y));

  } else if (fabs (p1.x () - p2.x ()) < 0.1) {

    double y1 = floor (p1.y () + 0.5);
    double y2 = floor (p2.y () + 0.5);
    double x = floor (p1.x () + 0.5);
    return std::make_pair (geo::DPoint (x, y1), geo::DPoint (x, y2));

  } else if (fabs (fabs (p1.x () - p2.x ()) - fabs (p1.y () - p2.y ())) < 0.1) {

    double x1 = floor (p1.x () + 0.5);
    double y1 = floor (p1.y () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    double y2;
    if (p2.y () < p1.y ()) {
      y2 = y1 - fabs (x2 - x1);
    } else {
      y2 = y1 + fabs (x2 - x1);
    }
    return std::make_pair (geo::DPoint (x1, y1), geo::DPoint (x2, y2));

  } else {

    double y1 = floor (p1.y () + 0.5);
    double y2 = floor (p2.y () + 0.5);
    double x1 = floor (p1.x () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    return std::make_pair (geo::DPoint (x1, y1), geo::DPoint (x2, y2));

  }
}

std::pair<geo::DPoint, geo::DPoint>
snap (const geo::DPoint &p1, const geo::DPoint &p2, geo::DCoord grid)
{
  if (grid <= 1e-10) {
    return std::make_pair (p1, p2);
  } else {
    std::pair<geo::DPoint, geo::DPoint> v = snap (geo::DPoint (p1.x () / grid, p1.y () / grid),
                                                geo::DPoint (p2.x () / grid, p2.y () / grid));
    return std::make_pair (v.first * grid, v.second * grid);
  }
}


// ------------------------------------------------------------------------
//  drawing related stuff ...

int draw_round (double x)
{
  if (x < -10000.0) {
    return -10000;
  } else if (x > 10000.0) {
    return 10000;
  } else {
    return x > 0 ? int (x + 0.5) : int (x - 0.5);
  }
}

QPoint draw_round (geo::DPoint p, int h)
{
  return QPoint (draw_round (p.x ()), h - 1 - draw_round (p.y ()));
}

std::pair<QPoint, QPoint>
draw_round (const geo::DPoint &p1, const geo::DPoint &p2, int h)
{
  std::pair<geo::DPoint , geo::DPoint> dp = draw_round_dbl (p1, p2, h);
  return std::make_pair (draw_round (dp.first, h), draw_round (dp.second, h));
}

std::pair<geo::DPoint, geo::DPoint>
draw_round_dbl (const geo::DPoint &p1, const geo::DPoint &p2, int /*h*/)
{
  if (fabs (p1.y () - p2.y ()) < 0.1) {
    
    double x1 = floor (p1.x () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    double y = floor (p1.y () + 0.5);
    return std::make_pair (geo::DPoint (x1, y), geo::DPoint (x2, y));

  } else if (fabs (p1.x () - p2.x ()) < 0.1) {

    double y1 = floor (p1.y () + 0.5);
    double y2 = floor (p2.y () + 0.5);
    double x = floor (p1.x () + 0.5);
    return std::make_pair (geo::DPoint (x, y1), geo::DPoint (x, y2));

  } else if (fabs (fabs (p1.x () - p2.x ()) - fabs (p1.y () - p2.y ())) < 0.1) {

    double x1 = floor (p1.x () + 0.5);
    double y1 = floor (p1.y () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    double y2;
    if (p2.y () < p1.y ()) {
      y2 = y1 - fabs (x2 - x1);
    } else {
      y2 = y1 + fabs (x2 - x1);
    }
    return std::make_pair (geo::DPoint (x1, y1), geo::DPoint (x2, y2));

  } else {

    double y1 = floor (p1.y () + 0.5);
    double y2 = floor (p2.y () + 0.5);
    double x1 = floor (p1.x () + 0.5);
    double x2 = floor (p2.x () + 0.5);
    return std::make_pair (geo::DPoint (x1, y1), geo::DPoint (x2, y2));

  }
}

geo::DVector 
snap_angle (const geo::DVector &in, lay::angle_constraint_type ac)
{
  std::vector <geo::DVector> ref_dir;
  if (ac != lay::AC_Any) {
    ref_dir.reserve (4);
    ref_dir.push_back (geo::DVector (1.0, 0));
    ref_dir.push_back (geo::DVector (0, 1.0));
    if (ac == lay::AC_Diagonal) {
      ref_dir.push_back (geo::DVector (-1.0, 1.0));
      ref_dir.push_back (geo::DVector (1.0, 1.0));
    }
  }

  geo::DVector out = in;

  double elen = in.length ();
  if (elen > 1e-6) {
    double max_proj = -10.0;
    for (std::vector <geo::DVector>::const_iterator re = ref_dir.begin (); re != ref_dir.end (); ++re) {
      double proj;
      proj = geo::sprod (*re, in) / (elen * re->length ());
      if (proj > max_proj) {
        max_proj = proj;
        out = *re * (elen * proj / re->length ());
      }
      proj = geo::sprod (-*re, in) / (elen * re->length ());
      if (proj > max_proj) {
        max_proj = proj;
        out = -*re * (elen * proj / re->length ());
      }
    }
  }

  return out;
}
// ---------------------------------------------------------------------------------------
//  obj_snap implementations

/**
 *  @brief A finder for a point on a contour
 *
 *  This is an object to look up a point on a contour.
 *  It will start from a given point and look into the direction of given cutlines.
 *  It will try to find the closest snapping point while also considering snapping.
 *
 *  There are basically three modes:
 *   - free mode (no cutlines, but snapping)
 *   - constrained mode (with a variety of directions)
 *   - directed mode (one direction and positive direction only)
 *
 *  There are more and mode constraints with each mode. In constrained mode, the
 *  resulting point will sit in one of the given directions. In directed mode,
 *  the (exact) resulting point will sit on the line.
 *
 *  Vertex and non-vertex snapping is available: with vertex snapping, the search
 *  point will snap to vertexes too. Without vertex snapping, the snap point is
 *  guaranteed to sit on an edge.
 *
 *  The finder will deliver two solutions if available: an exact one where the
 *  point sits exactly on the constraint and a more generic one where the resulting
 *  point is allowed to deviate a little from that direction. The exact solution is
 *  priority in the generic case.
 */
class ContourFinder
{
public:
  /**
   *  @brief Constructor
   */
  ContourFinder (const geo::DPoint &original, const geo::DVector &grid, const std::vector <geo::DEdge> &cutlines, bool with_vertex = true, bool directed = false)
    : m_any (false), m_any_exact (false), 
      m_original (original), m_is_vertex (false), m_is_vertex_exact (false),
      m_tests (10000 /* max. number of tests, TODO: make variable? */),
      mp_layout (0), m_cutlines (cutlines), mp_prop_sel (0), m_inv_prop_sel (false),
      m_with_vertex (with_vertex), m_directed (directed)
  {
    m_projection_constraint = ! m_cutlines.empty ();

    //  Add synthetic cutlines for the grid snap along edges
    if (m_cutlines.empty () && grid != geo::DVector ()) {

      geo::DPoint ps = lay::snap_xy (original, grid);

      double dy = (ps.y () > original.y ()) ? -grid.y () : grid.y ();
      double dx = (ps.x () > original.x ()) ? -grid.x () : grid.x ();

      m_cutlines.push_back (geo::DEdge (geo::DPoint (original.x (), ps.y ()), geo::DPoint (original.x () + 1.0, ps.y ())));
      m_cutlines.push_back (geo::DEdge (geo::DPoint (original.x (), ps.y () + dy), geo::DPoint (original.x () + 1.0, ps.y () + dy)));
      m_cutlines.push_back (geo::DEdge (geo::DPoint (ps.x (), original.y ()), geo::DPoint (ps.x (), original.y () + 1.0)));
      m_cutlines.push_back (geo::DEdge (geo::DPoint (ps.x () + dx, original.y ()), geo::DPoint (ps.x () + dx, original.y () + 1.0)));

    }
  }

  /**
   *  @brief Triggers the finder
   *  This will run the finder on the given view.
   *  "search_range" is the search range in micron units.
   */
  void find (lay::LayoutView *view, double search_range)
  {
    if (! view) {
      return;
    }

    for (lay::LayerPropertiesConstIterator l = view->begin_layers (); ! l.at_end (); ++l) {

      if (l->is_visual ()) {

        int cv_index = l->cellview_index ();

        const lay::CellView &cv = view->cellview (l->cellview_index ());
        if (cv.is_valid ()) {

          const geo::Layout &layout = cv->layout ();

          double px = m_original.x ();
          double py = m_original.y ();
          double dd = std::max (0.0, search_range);

          double dmin = layout.dbu () * std::numeric_limits <geo::Coord>::min ();
          double dmax = layout.dbu () * std::numeric_limits <geo::Coord>::max ();
          geo::DBox dworld (dmin, dmin, dmax, dmax);

          geo::DBox dregion = dworld & geo::DBox (px - dd, py - dd, px + dd, py + dd);
          if (! dregion.empty ()) {

            int ctx_levels = int (cv.specific_path ().size ());

            int min_hier_level = view->get_min_hier_levels () - ctx_levels;
            int max_hier_level = view->get_max_hier_levels () - ctx_levels;
            if (l->hier_levels ().has_from_level ()) {
              min_hier_level = l->hier_levels ().from_level (ctx_levels, min_hier_level);
            }
            if (l->hier_levels ().has_to_level ()) {
              max_hier_level = l->hier_levels ().to_level (ctx_levels, max_hier_level);
            }

            m_region = geo::DBox (dregion.p1 (), dregion.p2 ());
            mp_layout = &layout;
            mp_prop_sel = &l->prop_sel ();
            m_inv_prop_sel = l->inverse_prop_sel ();

            const std::vector<geo::DCplxTrans> &trans = l->trans ();
            for (std::vector<geo::DCplxTrans>::const_iterator t = trans.begin (); t != trans.end () && m_tests > 0; ++t) {
              do_find (view, cv_index, *cv.cell (), l->layer_index (), min_hier_level, max_hier_level, *t * geo::CplxTrans (mp_layout->dbu ()) * cv.context_trans ());
            }

          }

        }

      }

    }
  }

  /**
   *  @brief Returns a value indicating whether a vertex was found
   *  Returns true, if the point returned by \get_found is a vertex.
   *  In that case, the associated edges (at least two of them) are
   *  returned by \get_found_vertex edges.
   */
  bool is_vertex () const
  {
    if (m_any_exact) {
      return m_is_vertex_exact;
    } else if (m_any) {
      return m_is_vertex;
    } else {
      return false;
    }
  }

  /**
   *  @brief Returns the vertex edges if a vertex was found
   *  See \is_vertex for details.
   */
  std::pair<geo::DEdge, geo::DEdge> get_found_vertex_edges () const
  {
    if (m_any_exact) {
      return std::make_pair (m_edge1_exact, m_edge2_exact);
    } else if (m_any) {
      return std::make_pair (m_edge1, m_edge2);
    } else {
      return std::pair<geo::DEdge, geo::DEdge> ();
    }
  }

  /**
   *  @brief Returns a value indicating whether an associated edge is present
   *  See \get_found_edge for details.
   */
  bool has_found_edge () const
  {
    if (m_any_exact) {
      return ! m_edge1_exact.is_degenerate ();
    } else if (m_any) {
      return ! m_edge1.is_degenerate ();
    } else {
      return false;
    }
  }

  /**
   *  @brief Gets the edge associated with the best point (see \get_found)
   *  This edge is valid if \has_found_edge returns true. If a vertex is
   *  present, get_found_vertex_edges should be used.
   */
  geo::DEdge get_found_edge () const
  {
    if (m_any_exact) {
      return m_edge1_exact;
    } else if (m_any) {
      return m_edge1;
    } else {
      return geo::DEdge ();
    }
  }

  /**
   *  @brief Gets the best point found
   */
  geo::DPoint get_found () const
  {
    if (m_any_exact) {
      return m_closest_exact;
    } else if (m_any) {
      return m_closest;
    } else {
      return m_original;
    }
  }

  /**
   *  @brief Returns true if there is a generic solution
   */
  bool any () const
  {
    return (m_any || m_any_exact);
  }

  /**
   *  @brief Returns true if there is an exact solution
   */
  bool any_exact () const
  {
    return m_any_exact;
  }

private:
  void 
  find_closest_exact (const geo::DPoint &p, const geo::DEdge &e)
  {
    if (! m_any_exact || m_original.distance (p) < m_original.distance (m_closest_exact)) {

      if (m_directed) {
        for (std::vector<geo::DEdge>::const_iterator cl = m_cutlines.begin (); cl != m_cutlines.end (); ++cl) {
          if (geo::sprod_sign (p - m_original, cl->d ()) <= 0) {
            return;
          }
        }
      }

      m_edge1_exact = m_edge2_exact = e;
      m_is_vertex_exact = false;
      m_closest_exact = p;
      m_any_exact = true;

    }
  }

  void 
  find_closest (const geo::DPoint &p, const geo::DEdge &e)
  {
    if (! m_any || m_original.distance (p) < m_original.distance (m_closest)) {

      if (m_directed) {
        for (std::vector<geo::DEdge>::const_iterator cl = m_cutlines.begin (); cl != m_cutlines.end (); ++cl) {
          if (geo::sprod_sign (p - m_original, cl->d ()) < 0) {
            return;
          }
        }
      }

      m_edge1 = m_edge2 = e;
      m_is_vertex = false;
      m_closest = p;
      m_any = true;

    }
  }

  void closest (const geo::DPoint &p)
  {
    if (! m_with_vertex) {

      //  Only edges are considered for snapping.

    } else if (! m_projection_constraint) {

      find_closest_exact (p, geo::DEdge (p, p));

    } else {

      //  Test, if there is a crossing between a cut line and the vertical
      //  or horizontal lines through the given point. Take this as the
      //  test point.
      for (std::vector <geo::DEdge>::const_iterator cl = m_cutlines.begin (); cl != m_cutlines.end (); ++cl) {
        std::pair<bool, geo::DPoint> ret;
        ret = geo::DEdge (p, p + geo::DVector (1.0, 0.0)).cut_point (*cl);
        if (ret.first) {
          find_closest_exact (ret.second, geo::DEdge (p, p));
        }
        ret = geo::DEdge (p, p + geo::DVector (0.0, 1.0)).cut_point (*cl);
        if (ret.first) {
          find_closest_exact (ret.second, geo::DEdge (p, p));
        }
      }

    }
  }

  void closest (const geo::DEdge &e)
  {
    bool any_point = false;

    //  do the checks in dbu space rather than micron space because
    //  the tolerances are set up for this.
    for (std::vector <geo::DEdge>::const_iterator cl = m_cutlines.begin (); cl != m_cutlines.end (); ++cl) {
      std::pair<bool, geo::DPoint> ret = e.cut_point (*cl);
      if (ret.first) {
        //  if the projection exactly hits the edge and the point
        //  of crossing is inside the search region, take this as
        //  the exact hit. Exact hits have priority over projected
        //  hits.
        if (e.contains (ret.second) && m_region.contains (ret.second)) {
          find_closest_exact (ret.second, e);
        } else {
          find_closest (ret.second, e);
        }
        any_point = true;
      }
    }

    if (! any_point) {
    
      //  no certain direction to look into:
      //  compute the projection of the point and if within a 
      //  certain range, take the projected point.

      geo::DCoord d = std::max (m_region.width (), m_region.height ());
      geo::DVector v (e.p2 () - e.p1 ());
      geo::DVector n (-v.y (), v.x ());
      double f = d / n.double_length ();

      geo::DPoint e1 (m_original.x () - n.x () * f,
                     m_original.y () - n.y () * f);
      geo::DPoint e2 (m_original.x () + n.x () * f,
                     m_original.y () + n.y () * f);
      
      double dmin = mp_layout->dbu () * std::numeric_limits <geo::Coord>::min ();
      double dmax = mp_layout->dbu () * std::numeric_limits <geo::Coord>::max ();
      geo::DBox dworld (dmin, dmin, dmax, dmax);

      if (dworld.contains (e1) && dworld.contains (e2)) {

        std::pair<bool, geo::DPoint> ip = e.intersect_point (geo::DEdge (e1, e2));
        if (ip.first) {
          //  as we are not looking into a specific direction, this found
          //  is high-prio only on "any angle". Otherwise it's a low-prio
          //  fallback.
          if (m_cutlines.empty ()) {
            find_closest_exact (ip.second, e);
          } else {
            find_closest (ip.second, e);
          }
        }

      }

    }

    if (m_any && m_closest.equal (e.p1 ())) {
      m_edge1 = e;
      m_is_vertex = true;
    }
    if (m_any && m_closest.equal (e.p2 ())) {
      m_edge2 = e;
      m_is_vertex = true;
    }

    if (m_any_exact && m_closest_exact.equal (e.p1 ())) {
      m_edge1_exact = e;
      m_is_vertex_exact = true;
    }
    if (m_any_exact && m_closest_exact.equal (e.p2 ())) {
      m_edge2_exact = e;
      m_is_vertex_exact = true;
    }
  }

  void
  do_find (lay::LayoutView *view, int cv_index, const geo::Cell &cell, unsigned int l, int min_level, int max_level, const geo::CplxTrans &t)
  {
    geo::Box touch_box = t.inverted () * m_region;

    if (max_level <= 0 || ! cell.bbox (l).touches (touch_box) || view->is_cell_hidden (cell.cell_index (), cv_index)) {
      return;
    }

    if (min_level <= 0) {

      const geo::Shapes &shapes = cell.shapes (l);

      geo::ShapeIterator shape = shapes.begin_touching (touch_box, geo::ShapeIterator::Polygons | geo::ShapeIterator::Paths | geo::ShapeIterator::Boxes, mp_prop_sel, m_inv_prop_sel);
      while (! shape.at_end () && m_tests > 0) {

        --m_tests;

        if (shape->is_polygon ()) {

          for (geo::Shape::polygon_edge_iterator e = shape->begin_edge (); ! e.at_end (); ++e) {
            test_edge (t * *e);
          }

        } else if (shape->is_path ()) {

          //  test the "spine"
          geo::Shape::point_iterator pt = shape->begin_point (); 
          if (pt != shape->end_point ()) {
            geo::Point p (*pt);
            ++pt;
            for (; pt != shape->end_point (); ++pt) {
              test_edge (t * geo::Edge (p, *pt));
              p = *pt;
            }
          }

          //  convert to polygon and test those edges
          geo::Path path;
          shape->path (path);

          geo::DPath::pointlist_type pts;
          path.transformed (t).hull (pts);

          geo::DPath::pointlist_type::const_iterator p = pts.begin ();
          if (p != pts.end ()) {

            geo::DPath::pointlist_type::const_iterator pp = p;
            ++pp;

            while (pp != pts.end ()) {
              test_edge (geo::DEdge (*p, *pp));
              p = pp;
              ++pp;
            }

            test_edge (geo::DEdge (*p, pts [0]));
          }

        } else if (shape->is_box ()) {

          const geo::Box &box = shape->box ();

          test_edge (t * geo::Edge (box.p1 (), geo::Point (box.left (), box.top ())));
          test_edge (t * geo::Edge (geo::Point (box.left (), box.top ()), box.p2 ()));
          test_edge (t * geo::Edge (box.p2 (), geo::Point (box.right (), box.bottom ())));
          test_edge (t * geo::Edge (geo::Point (box.right (), box.bottom ()), box.p1 ()));

        }

        ++shape;

      }

    }

    geo::box_convert <geo::CellInst> bc (*mp_layout, l);

    geo::Cell::touching_iterator inst = cell.begin_touching (touch_box); 
    while (! inst.at_end () && m_tests > 0) {

      const geo::CellInstArray &cell_inst = inst->cell_inst ();
      for (geo::CellInstArray::iterator p = cell_inst.begin_touching (touch_box, bc); ! p.at_end () && m_tests > 0; ++p) {

        --m_tests;

        do_find (view, cv_index, 
                 mp_layout->cell (cell_inst.object ().cell_index ()), l,
                 min_level - 1, max_level - 1,
                 t * cell_inst.complex_trans (*p));

      }

      ++inst;

    }

  }

  void
  test_edge (const geo::DEdge &edg)
  {
    if (m_with_vertex) {

      //  vertex snap is just annoying when trying to measure the width of simulation contours ..
      //  But: when measuring corner-to-corner distances it is very valuable ..

      //  we hit the region with this edge: vertex snap.
      if (m_region.contains (edg.p1 ())) {
        closest (edg.p1 ());
      }
      if (m_region.contains (edg.p2 ())) {
        closest (edg.p2 ());
      }

    }
    
    //  if the edge cuts through the active region: test the
    //  edge as a whole
    if (edg.clipped (m_region).first) {
      closest (edg);
    }
  }

  bool m_any, m_any_exact;
  geo::DPoint m_closest, m_closest_exact;
  geo::DPoint m_original;
  geo::DEdge m_edge1, m_edge2;
  geo::DEdge m_edge1_exact, m_edge2_exact;
  bool m_is_vertex, m_is_vertex_exact;
  int m_tests;
  const geo::Layout *mp_layout;
  geo::DBox m_region;
  std::vector <geo::DEdge> m_cutlines;
  const std::set<geo::properties_id_type> *mp_prop_sel;
  bool m_inv_prop_sel;
  bool m_projection_constraint;
  bool m_with_vertex;
  bool m_directed;
};

static PointSnapToObjectResult
do_obj_snap (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, double snap_range, const std::vector <geo::DEdge> &cutlines)
{
  geo::DPoint dp (pt);

  ContourFinder finder (dp, grid, cutlines);
  finder.find (view, snap_range);

  //  in grid snap mode, for the "object free" analysis snap to the grid now
  if (grid != geo::DVector ()) {
    dp = lay::snap_xy (dp, grid);
  }

  //  try to project the point onto the cutlines
  geo::DPoint closest;
  bool anyp = false;

  for (std::vector<geo::DEdge>::const_iterator c = cutlines.begin (); c != cutlines.end (); ++c) {
    std::pair<bool, geo::DPoint> r = c->cut_point (geo::DEdge (dp, dp + geo::DVector (-c->dy (), c->dx ())));
    if (r.first) {
      if (! anyp || dp.sq_distance (r.second) < dp.sq_distance (closest)) {
        anyp = true;
        closest = r.second;
      }
    }
  }

  //  if both the projection and the finder are successful, decide by a heuristic criterion which to take
  //  (the projection gets a penalty (of the snap range) to make it count less than the finder's choice)
  //  This avoids extreme distortions of the ruler due to projection on long edges.
  if (finder.any () && anyp && (dp.distance (closest) + snap_range) * 5.0 < dp.distance (finder.get_found ())) {

    PointSnapToObjectResult res;
    res.snapped_point = closest;
    return res;

  } else if (finder.any ()) {

    PointSnapToObjectResult res;
    res.snapped_point = finder.get_found ();
    res.object_ref = finder.get_found_edge ();
    if (finder.is_vertex ()) {
      res.object_snap = PointSnapToObjectResult::ObjectVertex;
    } else if (finder.has_found_edge ()) {
      res.object_snap = PointSnapToObjectResult::ObjectEdge;
    } else {
      res.object_snap = PointSnapToObjectResult::ObjectUnspecific;
    }
    return res;

  } else if (anyp) {

    PointSnapToObjectResult res;
    res.snapped_point = closest;
    return res;

  } else {

    PointSnapToObjectResult res;
    res.snapped_point = dp;
    return res;

  }
}

static TwoPointSnapToObjectResult
do_obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt1, const geo::DPoint &pt2, const geo::DVector &grid, double min_search_range, double max_search_range, const std::vector <geo::DEdge> &cutlines)
{
  geo::DPoint dp1 (pt1);
  geo::DPoint dp2 (pt2);

  ContourFinder finder (dp1, grid, cutlines, cutlines.empty () /*vertex snap on "any direction"*/);

  double sr = min_search_range;
  while (sr < max_search_range + 1e-6) {

    finder.find (view, sr);

    if ((cutlines.empty () && finder.any ()) || finder.any_exact ()) {

      geo::DPoint p1 = finder.get_found ();

      std::vector <geo::DEdge> cl;
      geo::DVector n;

      if (! cutlines.empty ()) {

        //  select those cutlines that lead to the first intersection point
        for (std::vector<geo::DEdge>::const_iterator i = cutlines.begin (); i != cutlines.end (); ++i) {

          geo::DVector n = i->d ();
          geo::DVector d = dp2 - p1;
          if (fabs (geo::vprod (n, d)) < 1e-6 * n.length () * d.length ()) {
            if (geo::sprod_sign (n, d) < 0) {
              n = -n;
            }
            cl.push_back (geo::DEdge (p1, p1 + n));
          }

        }

      } else if (finder.is_vertex ()) {

        cl.push_back (geo::DEdge (p1, dp2));

      } else if (finder.has_found_edge ()) {

        n = finder.get_found_edge ().d ().transformed (geo::DTrans (geo::DTrans::r90));
        if (geo::sprod_sign (n, dp2 - p1) < 0) {
          n = -n;
        }
        cl.push_back (geo::DEdge (p1, p1 + n));

      }

      ContourFinder finder2 (dp2, grid, cl, false /*no vertex snap*/, true /*directional cutlines*/);

      double sr2 = min_search_range;
      while (sr2 < max_search_range + 1e-6) {

        finder2.find (view, sr2);
        if (finder2.any_exact ()) {

          geo::DPoint p2 = finder2.get_found ();

          TwoPointSnapToObjectResult res;
          res.any = true;
          res.first = p1;
          res.second = p2;

          res.object_ref_first = finder.get_found_edge ();
          if (finder.is_vertex ()) {
            res.object_snap_first = TwoPointSnapToObjectResult::ObjectVertex;
          } else if (finder.has_found_edge ()) {
            res.object_snap_first = TwoPointSnapToObjectResult::ObjectEdge;
          } else {
            res.object_snap_first = TwoPointSnapToObjectResult::ObjectUnspecific;
          }

          res.object_ref_second = finder2.get_found_edge ();
          if (finder2.is_vertex ()) {
            res.object_snap_second = TwoPointSnapToObjectResult::ObjectVertex;
          } else if (finder2.has_found_edge ()) {
            res.object_snap_second = TwoPointSnapToObjectResult::ObjectEdge;
          } else {
            res.object_snap_second = TwoPointSnapToObjectResult::ObjectUnspecific;
          }

          return res;

        }

        sr2 *= 2.0;

      }

      return TwoPointSnapToObjectResult ();

    }

    sr *= 2.0;

  }

  return TwoPointSnapToObjectResult ();
}

static void
make_cutlines (lay::angle_constraint_type snap_mode, const geo::DPoint &p1, std::vector <geo::DEdge> &cutlines)
{
  //  set up cutlines depending on the mode
  if (snap_mode == lay::AC_Ortho) {
    cutlines.reserve (2);
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (0.0, 1.0)));
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (1.0, 0.0)));
  } else if (snap_mode == lay::AC_Horizontal) {
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (1.0, 0.0)));
  } else if (snap_mode == lay::AC_Vertical) {
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (0.0, 1.0)));
  } else if (snap_mode == lay::AC_Diagonal) {
    cutlines.reserve (4);
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (0.0, 1.0)));
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (1.0, 0.0)));
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (1.0, 1.0)));
    cutlines.push_back (geo::DEdge (p1, p1 + geo::DVector (1.0, -1.0)));
  }
}

PointSnapToObjectResult
obj_snap (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, double snap_range)
{
  return do_obj_snap (view, pt, grid, snap_range, std::vector<geo::DEdge> ());
}

PointSnapToObjectResult
obj_snap (lay::LayoutView *view, const geo::DPoint &p1, const geo::DPoint &p2, const geo::DVector &grid, lay::angle_constraint_type snap_mode, double snap_range)
{
  std::vector <geo::DEdge> cutlines;
  make_cutlines (snap_mode, p1, cutlines);
  return do_obj_snap (view, p2, grid, snap_range, cutlines);
}

TwoPointSnapToObjectResult
obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, double min_search_range, double max_search_range)
{
  return obj_snap2 (view, pt, pt, grid, min_search_range, max_search_range);
}

TwoPointSnapToObjectResult
obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt, const geo::DVector &grid, lay::angle_constraint_type ac, double min_search_range, double max_search_range)
{
  return obj_snap2 (view, pt, pt, grid, ac, min_search_range, max_search_range);
}

TwoPointSnapToObjectResult
obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt1, const geo::DPoint &pt2, const geo::DVector &grid, double min_search_range, double max_search_range)
{
  geo::DPoint dp1 = lay::snap_xy (pt1, grid);
  geo::DPoint dp2 = lay::snap_xy (pt2, grid);

  return do_obj_snap2 (view, dp1, dp2, geo::DVector (), min_search_range, max_search_range, std::vector<geo::DEdge> ());
}

TwoPointSnapToObjectResult
obj_snap2 (lay::LayoutView *view, const geo::DPoint &pt1, const geo::DPoint &pt2, const geo::DVector &grid, lay::angle_constraint_type snap_mode, double min_search_range, double max_search_range)
{
  geo::DPoint dp1 = lay::snap_xy (pt1, grid);
  geo::DPoint dp2 = lay::snap_xy (pt2, grid);

  std::vector <geo::DEdge> cutlines;
  make_cutlines (snap_mode, dp1, cutlines);
  return do_obj_snap2 (view, dp1, dp2, geo::DVector (), min_search_range, max_search_range, cutlines);
}

}

