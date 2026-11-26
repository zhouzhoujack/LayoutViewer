#include "tlProgress.h"
#include "detail/utilsString.h"
#include "detail/utilsProgress.h"
#include "layFinder.h"

namespace lay
{

/** 
 *  @brief A pretty heuristic method to determine the "enclosing distance" of a polygon around a point
 */
template<class Iter, class Point>
double poly_dist (Iter edge, const Point &pt)
{
  double distance = std::numeric_limits<double>::max ();

  while (! edge.at_end ()) {
    std::pair <bool, Point> ret = (*edge).projected (pt);
    if (ret.first) {
      double d (ret.second.distance (pt));
      if (d < distance) {
        distance = d;
      }
    }
    ++edge;
  }

  return distance;
}

// -------------------------------------------------------------

//  max. number of tries in single-click selection before giving up
static int point_sel_tests = 10000;

//  max. number of tries in single-click selection before giving up
static int inst_point_sel_tests = 10000;

// ----------------------------------------------------------------------
//  Finder implementation

Finder::Finder (bool point_mode, bool top_level_sel)
  : m_min_level (0), m_max_level (0),
    mp_layout (0), mp_view (0), m_cv_index (0), m_point_mode (point_mode), m_catch_all (false), m_top_level_sel (top_level_sel)
{
  m_distance = std::numeric_limits<double>::max ();
}

Finder::~Finder ()
{
  //  .. nothing yet ..
}

bool 
Finder::closer (double d)
{
  //  the proximity is checked and delivered in micron units.
  d *= mp_view->cellview (m_cv_index)->layout ().dbu ();
  if (d <= m_distance) {
    m_distance = d;
    return true;
  } else {
    return false;
  }
}

void 
Finder::start (lay::LayoutView *view, const lay::CellView &cv, unsigned int cv_index, const std::vector<geo::ICplxTrans> &trans, const geo::Box &region, int min_level, int max_level, const std::vector<int> &layers)
{
  m_layers = layers;
  m_region = region; 
  mp_layout = &cv->layout ();
  mp_view = view;
  m_cv_index = cv_index;
  m_min_level = std::max (0, min_level);
  m_max_level = std::max (m_min_level, std::min (max_level, m_top_level_sel ? ((int) cv.specific_path ().size () + 1) : max_level));

  if (layers.size () == 1) {
    m_box_convert = geo::box_convert <geo::CellInst> (*mp_layout, (unsigned int) layers [0]);
    m_cell_box_convert = geo::box_convert <geo::Cell> ((unsigned int) layers [0]);
  } else {
    m_box_convert = geo::box_convert <geo::CellInst> (*mp_layout);
    m_cell_box_convert = geo::box_convert <geo::Cell> ();
  }

  m_path.erase (m_path.begin (), m_path.end ());

  for (std::vector<geo::ICplxTrans>::const_iterator t = trans.begin (); t != trans.end (); ++t) {
    do_find (*cv.cell (), int (cv.specific_path ().size ()), *t * cv.context_trans ());
  }
}

unsigned int
Finder::test_edge (const geo::ICplxTrans &trans, const geo::Edge &edg, double &distance, bool &match)
{
  geo::Point p1 = trans * edg.p1 ();
  geo::Point p2 = trans * edg.p2 ();

  unsigned int ret = 0;

  //  we hit the region with the edge end points - take the closest vertex
  if (m_region.contains (p1) || m_region.contains (p2)) {

    double d1 = p1.double_distance (m_region.center ());
    double d2 = p2.double_distance (m_region.center ());

    //  snap to the point - nothing can get closer
    distance = 0.0;
    if (d1 < d2) {
      ret = 1;
    } else {
      ret = 2;
    }

    match = true;

  }
  
  //  if the edge cuts through the active region: test the
  //  edge as a whole
  if (ret == 0) {
    geo::Edge edg_trans (p1, p2);
    if (edg_trans.clipped (m_region).first) {
      double d = edg_trans.distance_abs (m_region.center ());
      if (! match || d < distance) {
        distance = d;
        ret = 3;
      }
      match = true;
    }
  }

  return ret;
}

void
Finder::do_find (const geo::Cell &cell, int level, const geo::ICplxTrans &t)
{
  if (level <= m_max_level /*take level of cell itself*/ 
      && cell.is_proxy () 
      && m_layers.size () == 1 
      && (unsigned int) m_layers [0] == mp_layout->guiding_shape_layer ()) {

    //  when looking at the guiding shape layer, we can visit this cell as well allowing to find the guiding shapes

    geo::Box touch_box (t.inverted () * m_region);

    if (level >= m_min_level) {
      visit_cell (cell, touch_box, t, level);
    }

  } else if (level < m_max_level 
      && (t * m_cell_box_convert (cell)).touches (m_region) 
      && (mp_view->select_inside_pcells_mode () || !cell.is_proxy ()) 
      && !mp_view->is_cell_hidden (cell.cell_index (), m_cv_index)) {

    geo::Box touch_box (t.inverted () * m_region);

    if (level >= m_min_level) {
      visit_cell (cell, touch_box, t, level);
    }

    geo::Cell::touching_iterator inst = cell.begin_touching (touch_box); 
    while (! inst.at_end ()) {

      const geo::CellInstArray &cell_inst = inst->cell_inst ();
      for (geo::CellInstArray::iterator p = cell_inst.begin_touching (touch_box, m_box_convert); ! p.at_end (); ++p) {

        m_path.push_back (geo::InstElement (*inst, p));

        do_find (mp_layout->cell (cell_inst.object ().cell_index ()), 
                 level + 1,
                 t * cell_inst.complex_trans (*p));

        m_path.pop_back ();

      }

      ++inst;

    }

  }
}

// -------------------------------------------------------------
//  ShapeFinder implementation

ShapeFinder::ShapeFinder (bool point_mode, bool top_level_sel, geo::ShapeIterator::flags_type flags, const std::set<lay::ObjectInstPath> *excludes)
  : Finder (point_mode, top_level_sel), 
    mp_excludes ((excludes && !excludes->empty ()) ? excludes : 0),
    m_flags (flags), m_cv_index (0), m_topcell (0), 
    mp_prop_sel (0), m_inv_prop_sel (false), mp_progress (0)
{
  m_tries = point_sel_tests;
}

struct LPContextEqualOp
{
  bool operator() (const lay::LayerPropertiesConstIterator &a, const lay::LayerPropertiesConstIterator &b)
  {
    if (a->cellview_index () != b->cellview_index ()) {
      return false;
    }
    if (a->inverse_prop_sel () != b->inverse_prop_sel ()) {
      return false;
    }
    if (a->prop_sel () != b->prop_sel ()) {
      return false;
    }
    if (a->trans () != b->trans ()) {
      return false;
    }
    if (a->hier_levels () != b->hier_levels ()) {
      return false;
    }

    return true;
  }
};

struct LPContextCompareOp
{
  bool operator() (const lay::LayerPropertiesConstIterator &a, const lay::LayerPropertiesConstIterator &b)
  {
    if (a->cellview_index () != b->cellview_index ()) {
      return a->cellview_index () < b->cellview_index ();
    }
    if (a->inverse_prop_sel () != b->inverse_prop_sel ()) {
      return a->inverse_prop_sel () < b->inverse_prop_sel ();
    }
    if (a->prop_sel () != b->prop_sel ()) {
      return a->prop_sel () < b->prop_sel ();
    }
    if (a->trans () != b->trans ()) {
      return a->trans () < b->trans ();
    }
    if (a->hier_levels () != b->hier_levels ()) {
      return a->hier_levels () < b->hier_levels ();
    }

    return false;
  }
};

bool
ShapeFinder::find (lay::LayoutView *view, const geo::DBox &region_mu)
{
  geo::AbsoluteProgress progress (geo::to_string (QObject::tr ("Selecting ...")));
  progress.set_unit (1000);
  progress.set_format ("");
  mp_progress = &progress;

  m_context_layers.clear ();
  m_cells_with_context.clear ();

  std::vector<lay::LayerPropertiesConstIterator> lprops;
  for (lay::LayerPropertiesConstIterator lp = view->begin_layers (); ! lp.at_end (); ++lp) {
    if (lp->is_visual ()) {
      lprops.push_back (lp);
    }
  }

  std::sort (lprops.begin (), lprops.end (), LPContextCompareOp ());

  std::vector<int> layers;
  for (std::vector<lay::LayerPropertiesConstIterator>::const_iterator llp = lprops.begin (); llp != lprops.end (); ) {

    layers.clear ();

    lay::LayerPropertiesConstIterator lp0 = *llp;
    LPContextEqualOp eq;
    do {
      layers.push_back ((*llp)->layer_index ());
      ++llp;
    } while (llp != lprops.end () && eq(lp0, *llp));

    find_internal (view, lp0->cellview_index (), &lp0->prop_sel (), lp0->inverse_prop_sel (), lp0->hier_levels (), lp0->trans (), layers, region_mu);

  }

  //  search on the guiding shapes layer as well 
 
  //  Use the visible layers for the context: the guiding shape is only looked up for cells
  //  having one of these layers
  m_context_layers.clear ();
  for (std::vector<lay::LayerPropertiesConstIterator>::const_iterator llp = lprops.begin (); llp != lprops.end (); ++llp) {
    m_context_layers.push_back ((*llp)->layer_index ());
  }

  std::set< std::pair<geo::DCplxTrans, int> > variants = view->cv_transform_variants ();
  for (std::set< std::pair<geo::DCplxTrans, int> >::const_iterator v = variants.begin (); v != variants.end (); ++v) {

    layers.clear ();
    layers.push_back (view->cellview (v->second)->layout ().guiding_shape_layer ());

    std::vector<geo::DCplxTrans> trans;
    trans.push_back (v->first);

    find_internal (view, (unsigned int) v->second, 0, false, lay::HierarchyLevelSelection (), trans, layers, region_mu);

  }

  mp_progress = 0;
  m_cells_with_context.clear ();
  m_context_layers.clear ();

  return ! m_founds.empty ();
}

bool 
ShapeFinder::find (lay::LayoutView *view, const lay::LayerProperties &lprops, const geo::DBox &region_mu)
{
  geo::AbsoluteProgress progress (geo::to_string (QObject::tr ("Selecting ...")));
  progress.set_unit (1000);
  progress.set_format ("");
  mp_progress = &progress;

  m_cells_with_context.clear ();
  m_context_layers.clear ();

  std::vector<int> layers;
  layers.push_back (lprops.layer_index ());
  bool result = find_internal (view, lprops.cellview_index (), &lprops.prop_sel (), lprops.inverse_prop_sel (), lprops.hier_levels (), lprops.trans (), layers, region_mu);

  mp_progress = 0;
  return result;
}

bool 
ShapeFinder::find_internal (lay::LayoutView *view, unsigned int cv_index, const std::set<geo::properties_id_type> *prop_sel, bool inv_prop_sel, const lay::HierarchyLevelSelection &hier_sel, const std::vector<geo::DCplxTrans> &trans_mu, const std::vector<int> &layers, const geo::DBox &region_mu)
{
  m_cv_index = cv_index;

  const lay::CellView &cv = view->cellview (m_cv_index);
  if (! cv.is_valid ()) {
    return false;
  }

  m_topcell = cv.cell_index ();

  double dbu = cv->layout ().dbu ();
  geo::Box region = geo::VCplxTrans (1.0 / dbu) * region_mu;
  std::vector<geo::ICplxTrans> trans;
  trans.reserve(trans_mu.size());
  for (std::vector<geo::DCplxTrans>::const_iterator t = trans_mu.begin(); t != trans_mu.end(); ++t) {
    trans.push_back(geo::VCplxTrans(1.0 / dbu) * *t * geo::CplxTrans(dbu));
  }

  mp_prop_sel = prop_sel;
  m_inv_prop_sel = inv_prop_sel;

  int ctx_path_length = int (cv.specific_path ().size ());

  int min_level = view->get_min_hier_levels ();
  int max_level = view->get_max_hier_levels ();
  if (hier_sel.has_from_level ()) {
    min_level = hier_sel.from_level (ctx_path_length, min_level);
  }
  if (hier_sel.has_to_level ()) {
    max_level = hier_sel.to_level (ctx_path_length, max_level);
  }

  //  actually find
  try {
    start (view, cv, m_cv_index, trans, region, min_level, max_level, layers);
  } catch (StopException) {
    // ..
  }

  //  return true if anything was found
  return ! m_founds.empty ();
}

void
ShapeFinder::checkpoint ()
{
  if (! point_mode ()) {
    ++*mp_progress;
  } else {
    if (--m_tries < 0) {
      throw StopException ();
    }
  }
}

void 
ShapeFinder::visit_cell (const geo::Cell &cell, const geo::Box &search_box, const geo::ICplxTrans &t, int /*level*/)
{
  if (! m_context_layers.empty ()) {

    std::map<geo::cell_index_type, bool>::const_iterator ctx = m_cells_with_context.find (cell.cell_index ());
    if (ctx == m_cells_with_context.end ()) {

      bool has_context = false;
      for (std::vector<int>::const_iterator l = m_context_layers.begin (); l != m_context_layers.end () && ! has_context; ++l) {
        if (! cell.bbox ((unsigned int) *l).empty ()) {
          has_context = true;
        }
      }

      ctx = m_cells_with_context.insert (std::make_pair (cell.cell_index (), has_context)).first;

    }

    if (! ctx->second) {
      return;
    }

  }

  if (! point_mode ()) {

    checkpoint ();

    for (std::vector<int>::const_iterator l = layers ().begin (); l != layers ().end (); ++l) {

      if (layers ().size () == 1 || (layers ().size () > 1 && cell.bbox ((unsigned int) *l).touches (search_box))) {

        const geo::Shapes &shapes = cell.shapes ((unsigned int) *l);

        geo::ShapeIterator shape = shapes.begin_touching (search_box, m_flags, mp_prop_sel, m_inv_prop_sel);
        while (! shape.at_end ()) {

          checkpoint ();

          //  in box mode, just test the boxes
          if (shape->bbox ().inside (search_box)) {

            m_founds.push_back (lay::ObjectInstPath ());
            m_founds.back ().set_cv_index (m_cv_index);
            m_founds.back ().set_topcell (m_topcell);
            m_founds.back ().assign_path (path ().begin (), path ().end ());
            m_founds.back ().set_layer (*l);
            m_founds.back ().set_shape (*shape);

            //  Remove the selection if it's part of the excluded set
            if (mp_excludes != 0 && mp_excludes->find (m_founds.back ()) != mp_excludes->end ()) {
              m_founds.pop_back ();
            }

          }

          ++shape;

        } 

      }

    }

  } else {

    for (std::vector<int>::const_iterator l = layers ().begin (); l != layers ().end (); ++l) {

      if (layers ().size () == 1 || (layers ().size () > 1 && cell.bbox ((unsigned int) *l).touches (search_box))) {

        checkpoint ();

        const geo::Shapes &shapes = cell.shapes (*l);

        geo::ShapeIterator shape = shapes.begin_touching (search_box, m_flags, mp_prop_sel, m_inv_prop_sel);
        while (! shape.at_end ()) {

          bool match = false;
          double d = std::numeric_limits<double>::max ();

          checkpoint ();

          geo::Point point (search_box.center ());

          //  in point mode, test the edges and use a "closest" criterion
          if (shape->is_polygon ()) {

            for (geo::Shape::polygon_edge_iterator e = shape->begin_edge (); ! e.at_end (); ++e) {
              test_edge (t, *e, d, match);
            }

            //  test if inside the polygon
            if (! match && inside_poly (shape->begin_edge (), point) >= 0) { 
              d = t.ctrans (poly_dist (shape->begin_edge (), point)); 
              match = true;
            }

          } else if (shape->is_path ()) {

            //  test the "spine"
            geo::Shape::point_iterator pt = shape->begin_point (); 
            if (pt != shape->end_point ()) {
              geo::Point p (*pt);
              ++pt;
              for (; pt != shape->end_point (); ++pt) {
                test_edge (t, geo::Edge (p, *pt), d, match);
                p = *pt;
              }
            }

            //  convert to polygon and test those edges
            geo::Polygon poly;
            shape->polygon (poly);
            for (geo::Polygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
              test_edge (t, *e, d, match);
            }

            //  test if inside the polygon
            if (! match && inside_poly (poly.begin_edge (), point) >= 0) { 
              d = t.ctrans (poly_dist (poly.begin_edge (), point));
              match = true;
            }

          } else if (shape->is_box ()) {

            const geo::Box &box = shape->box ();

            //  point-like boxes are handles which attract the finder
            if (box.width () == 0 && box.height () == 0) {
              d = 0.0;
              match = true;
            } else {

              //  convert to polygon and test those edges
              geo::Polygon poly (box);
              for (geo::Polygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
                test_edge (t, *e, d, match);
              }

              if (! match && box.contains (search_box.center ())) {
                d = t.ctrans (poly_dist (poly.begin_edge (), point)); 
                match = true;
              }

            }

          } else if (shape->is_text ()) {

            geo::Point tp (shape->text_trans () * geo::Point ());
            if (search_box.contains (tp)) {
              d = t.ctrans (tp.distance (search_box.center ()));
              match = true;
            }

          }

          if (match) {

            lay::ObjectInstPath found;
            found.set_cv_index (m_cv_index);
            found.set_topcell (m_topcell);
            found.assign_path (path ().begin (), path ().end ());
            found.set_layer (*l);
            found.set_shape (*shape);

            if (mp_excludes) {

              //  with an exclude list first create the selection item so we can check
              //  if it's part of the exclude set.

              //  in point mode just store the found object that has the least "distance" and is
              //  not in the exclude set
              match = (mp_excludes->find (found) == mp_excludes->end ());

            }

            if (match && (catch_all () || closer (d))) {

              //  in point mode just store that found that has the least "distance"
              if (m_founds.empty () || catch_all ()) {
                m_founds.push_back (found);
              }

              m_founds.back () = found;

            }

          }

          ++shape;

        }

      }

    }

  }

}

// -------------------------------------------------------------
//  InstFinder implementation

InstFinder::InstFinder (bool point_mode, bool top_level_sel, bool full_arrays, bool enclose_inst, const std::set<lay::ObjectInstPath> *excludes, bool visible_layers)
  : Finder (point_mode, top_level_sel), 
    m_cv_index (0), m_topcell (0), 
    mp_excludes ((excludes && !excludes->empty ()) ? excludes : 0),
    m_full_arrays (full_arrays), 
    m_enclose_insts (enclose_inst), 
    m_visible_layers (visible_layers),
    mp_view (0), 
    mp_progress (0)
{
  m_tries = inst_point_sel_tests;
}

bool
InstFinder::find (lay::LayoutView *view, const geo::DBox &region_mu)
{
  geo::AbsoluteProgress progress (geo::to_string (QObject::tr ("Selecting ...")));
  progress.set_unit (1000);
  progress.set_format ("");
  mp_progress = &progress;

  std::set< std::pair<geo::DCplxTrans, int> > variants = view->cv_transform_variants ();
  for (std::set< std::pair<geo::DCplxTrans, int> >::const_iterator v = variants.begin (); v != variants.end (); ++v) {
    find (view, v->second, v->first, region_mu);
  }

  mp_progress = 0;
  return ! m_founds.empty ();
}

bool 
InstFinder::find (lay::LayoutView *view, unsigned int cv_index, const geo::DCplxTrans &trans_mu, const geo::DBox &region_mu)
{
  geo::AbsoluteProgress progress (geo::to_string (QObject::tr ("Selecting ...")));
  progress.set_unit (1000);
  progress.set_format ("");
  mp_progress = &progress;

  bool result = find_internal (view, cv_index, trans_mu, region_mu);

  mp_progress = 0;
  return result;
}

bool 
InstFinder::find_internal (lay::LayoutView *view, unsigned int cv_index, const geo::DCplxTrans &trans_mu, const geo::DBox &region_mu)
{
  const lay::CellView &cv = view->cellview (cv_index);
  if (! cv.is_valid ()) {
    return false;
  }

  m_visible_layer_indexes.clear ();
  if (m_visible_layers) {
    for (lay::LayerPropertiesConstIterator l = view->begin_layers (); !l.at_end (); ++l) {
      if (! l->has_children () && l->visible (true /*real*/) && l->valid (true /*real*/) && l->cellview_index () == int (cv_index)) {
        m_visible_layer_indexes.push_back (l->layer_index ());
      }
    }
  }

  if (!m_visible_layers || view->guiding_shapes_visible ()) {
    m_visible_layer_indexes.push_back (cv->layout ().guiding_shape_layer ());
  }

  m_cv_index = cv_index;
  m_topcell = cv.cell ()->cell_index ();
  mp_view = view;

  double dbu = cv->layout ().dbu ();
  geo::Box region = geo::VCplxTrans (1.0 / dbu) * region_mu;

  //  actually find
  try {
    std::vector<geo::ICplxTrans> tv;
    tv.push_back (geo::VCplxTrans (1.0 / dbu) * trans_mu * geo::CplxTrans (dbu));
    start (view, cv, cv_index, tv, region, view->get_min_hier_levels (), view->get_max_hier_levels (), std::vector<int> ());
  } catch (StopException) {
    // ..
  }

  //  return true if anything was found
  return ! m_founds.empty ();
}

void 
InstFinder::visit_cell (const geo::Cell &cell, const geo::Box &search_box, const geo::ICplxTrans &t, int level)
{
  if (! point_mode ()) {

    ++*mp_progress;

    //  look for instances to check here ..
    geo::Cell::touching_iterator inst = cell.begin_touching (search_box); 
    while (! inst.at_end ()) {

      const geo::CellInstArray &cell_inst = inst->cell_inst ();
      const geo::Cell &inst_cell = layout ().cell (cell_inst.object ().cell_index ());

      ++*mp_progress;

      //  just consider the instances exactly at the last level of 
      //  hierarchy (this is where the boxes are drawn) or of cells that
      //  are hidden.
      if (level == max_level () - 1 || inst_cell.is_proxy () || mp_view->is_cell_hidden (inst_cell.cell_index (), m_cv_index)) {

        geo::box_convert <geo::CellInst, false> bc (layout ());
        for (geo::CellInstArray::iterator p = cell_inst.begin_touching (search_box, bc); ! p.at_end (); ++p) {
        
          ++*mp_progress;

          geo::Box ibox;
          if (inst_cell.bbox ().empty ()) {
            ibox = geo::Box (geo::Point (0, 0), geo::Point (0, 0));
          } else if (! m_visible_layers || level == mp_view->get_max_hier_levels () - 1 || mp_view->is_cell_hidden (inst_cell.cell_index (), m_cv_index)) {
            ibox = inst_cell.bbox ();
          } else {
            for (std::vector<int>::const_iterator l = m_visible_layer_indexes.begin (); l != m_visible_layer_indexes.end (); ++l) {
              ibox += inst_cell.bbox (*l);
            }
          }

          if (! ibox.empty ()) {

            geo::Box box = cell_inst.complex_trans (*p) * ibox;

            //  in box mode, just test the boxes
            if (! m_enclose_insts || box.inside (search_box)) {

              //  in point mode just store that found that has the least "distance"
              m_founds.push_back (lay::ObjectInstPath ());
              m_founds.back ().set_cv_index (m_cv_index);
              m_founds.back ().set_topcell (m_topcell);
              m_founds.back ().assign_path (path ().begin (), path ().end ());

              //  add the selected instance as the last element of the path
              geo::InstElement el;
              el.inst_ptr = *inst;
              if (! m_full_arrays) {
                el.array_inst = p;
              }
              m_founds.back ().add_path (el);

              //  Remove the selection if it's part of the excluded set
              if (mp_excludes != 0 && mp_excludes->find (m_founds.back ()) != mp_excludes->end ()) {
                m_founds.pop_back ();
              }

              //  in "full arrays" mode, a single reference to that array is sufficient
              if (m_full_arrays) {
                break;
              }

            }

          }

        }

      }

      ++inst;

    }

  } else {

    if (--m_tries < 0) {
      throw StopException ();
    }

    //  look for instances to check here ..
    geo::Cell::touching_iterator inst = cell.begin_touching (search_box); 
    while (! inst.at_end ()) {

      if (--m_tries < 0) {
        throw StopException ();
      }

      const geo::CellInstArray &cell_inst = inst->cell_inst ();
      const geo::Cell &inst_cell = layout ().cell (cell_inst.object ().cell_index ());

      //  just consider the instances exactly at the last level of 
      //  hierarchy (this is where the boxes are drawn) or if of cells that
      //  are hidden.
      if (level == max_level () - 1 || inst_cell.is_proxy () || mp_view->is_cell_hidden (inst_cell.cell_index (), m_cv_index)) {

        geo::box_convert <geo::CellInst, false> bc (layout ());
        for (geo::CellInstArray::iterator p = cell_inst.begin_touching (search_box, bc); ! p.at_end (); ++p) {
        
          if (--m_tries < 0) {
            throw StopException ();
          }

          bool match = false;
          double d = std::numeric_limits<double>::max ();

          geo::Box ibox;
          if (inst_cell.bbox ().empty ()) {
            ibox = geo::Box (geo::Point (0, 0), geo::Point (0, 0));
          } else if (! m_visible_layers || level == mp_view->get_max_hier_levels () - 1 || mp_view->is_cell_hidden (inst_cell.cell_index (), m_cv_index)) {
            ibox = inst_cell.bbox ();
          } else {
            for (std::vector<int>::const_iterator l = m_visible_layer_indexes.begin (); l != m_visible_layer_indexes.end (); ++l) {
              ibox += inst_cell.bbox (*l);
            }
          }

          if (! ibox.empty ()) {

            if (ibox.width () == 0 && ibox.height () == 0) {
              match = true;
              d = 0.0;
            } else {

              //  convert to polygon and test those edges
              geo::Polygon poly (cell_inst.complex_trans (*p) * geo::Polygon (ibox));

              for (geo::Polygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
                test_edge (t, *e, d, match);
              }

              if (! match && geo::inside_poly (poly.begin_edge (), search_box.center ())) {
                d = t.ctrans (poly_dist (poly.begin_edge (), search_box.center ()));
                match = true;
              }

            }

            d += 1; // the instance has a small penalty so that shapes win over instances

          }

          if (match) {

            lay::ObjectInstPath found;
            found.set_cv_index (m_cv_index);
            found.set_topcell (m_topcell);
            found.assign_path (path ().begin (), path ().end ());

            //  add the selected instance as the last element of the path
            geo::InstElement el;
            el.inst_ptr = *inst;
            if (! m_full_arrays) {
              el.array_inst = p;
            }
            found.add_path (el);

            if (mp_excludes) {

              //  with an exclude list first create the selection item so we can check
              //  if it's part of the exclude set.

              //  in point mode just store the found object that has the least "distance" and is
              //  not in the exclude set
              match = (mp_excludes->find (found) == mp_excludes->end ());

            }

            if (match && (catch_all () || closer (d))) {

              //  in point mode just store that found that has the least "distance"
              if (m_founds.empty () || catch_all ()) {
                m_founds.push_back (lay::ObjectInstPath ());
              }

              m_founds.back () = found;

            }

          }

        }

      }

      ++inst;

    }

  }

}

} // namespace edt

