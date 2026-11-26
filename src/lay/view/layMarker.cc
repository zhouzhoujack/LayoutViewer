#include "geoLayout.h"
#include "detail/utilsString.h"
#include "detail/geoRecursiveShapeIterator.h"
#include "detail/geoBoxConvert.h"
#include "layMarker.h"
#include "laySnap.h"
#include "layCanvasPlane.h"
#include "layViewOp.h"
#include "layRenderer.h"
#include "layLayoutView.h"
#include "tlAssert.h"

namespace lay
{

// ------------------------------------------------------------------------

void render_cell_inst (const geo::Layout &layout, const geo::CellInstArray &inst, const geo::CplxTrans &tr, lay::Renderer &r,
                       unsigned int font, lay::CanvasPlane *fill, lay::CanvasPlane *contour, lay::CanvasPlane *vertex, lay::CanvasPlane *text,
                       bool cell_name_text_transform, int min_size_for_label, bool draw_outline, size_t max_shapes)
{
  bool render_origins = false;

  const geo::Cell &cell = layout.cell (inst.object ().cell_index ());
  std::string cell_name = layout.display_name (inst.object ().cell_index ());
  geo::Box cell_box = cell.bbox ();

  geo::Vector a, b;
  unsigned long amax = 0, bmax = 0;
  unsigned long long n = 1;
  if (inst.is_regular_array (a, b, amax, bmax)) {
    n = (unsigned long long) amax * (unsigned long long) bmax;
  }

  bool draw_shapes;

  if (max_shapes > 0) {

    size_t nshapes = 0;
    draw_shapes = true;

    for (geo::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers () && draw_shapes; ++l) {
      geo::RecursiveShapeIterator shapes (layout, cell, (*l).first); 
      while (draw_shapes && ! shapes.at_end ()) {
        nshapes += n;
        if (nshapes > max_shapes) {
          draw_shapes = false;
        }
        ++shapes;
      }
    }

  } else {
    draw_shapes = false;
  }

  if (draw_outline || ! draw_shapes) {

    if (n > 1000) {

      geo::Vector av(a), bv(b);

      //  fallback to simpler representation using a description text
      geo::CplxTrans tbox (tr * inst.complex_trans ());

      //  one representative instance
      r.draw (cell_box, tbox, fill, contour, 0, text);
      r.draw (cell_box, geo::CplxTrans (tr * (av * long (amax - 1))) * tbox, fill, contour, 0, text);
      r.draw (cell_box, geo::CplxTrans (tr * (bv * long (bmax - 1))) * tbox, fill, contour, 0, text);
      r.draw (cell_box, geo::CplxTrans (tr * (av * long (amax - 1) + bv * long (bmax - 1))) * tbox, fill, contour, 0, text);

      geo::DBox cb (tbox * cell_box);
      geo::DPolygon p;
      geo::DPoint points[] = {
        geo::DPoint (cb.lower_left ()), 
        geo::DPoint (cb.lower_left () + tr * (av * long (amax - 1))),
        geo::DPoint (cb.lower_left () + tr * (av * long (amax - 1) + bv * long (bmax - 1))),
        geo::DPoint (cb.lower_left () + tr * (bv * long (bmax - 1))),
      };
      p.assign_hull (points, points + sizeof (points) / sizeof (points[0]));
      r.draw (p, fill, contour, 0, text);

      if (text) {
        geo::DBox arr_box (geo::DPoint (), geo::DPoint () + tr * (av * long (amax - 1) + bv * long (bmax - 1)));
        arr_box *= cb;
        r.draw (arr_box, geo::sprintf (geo::to_string (QObject::tr ("Array %ldx%ld")), amax, bmax), geo::Font (font), geo::HAlignCenter, geo::VAlignCenter, geo::DFTrans (geo::DFTrans::r0), 0, 0, 0, text);
      }

    } else {

      for (geo::CellInstArray::iterator arr = inst.begin (); ! arr.at_end (); ++arr) {

        //  fallback to simpler representation using a description text
        geo::CplxTrans tbox (tr * inst.complex_trans ());

        r.draw (cell_box, tbox, fill, contour, 0, 0);

        geo::DBox dbox = tbox * cell_box;
        if (text && ! cell_name.empty () && dbox.width () > min_size_for_label && dbox.height () > min_size_for_label) {

          //  Hint: we render to contour because the texts plane is reserved for properties
          r.draw (dbox, cell_name,
                  geo::Font (font),
                  geo::HAlignCenter,
                  geo::VAlignCenter,
                  //  TODO: apply "real" transformation?
                  geo::DFTrans (cell_name_text_transform ? tbox.fp_trans ().rot () : geo::DFTrans::r0), 0, 0, 0, text);

        }

      }

      render_origins = true;

    }

  }

  //  draw the interiour of the instance if required.
  if (draw_shapes) {

    render_origins = true;

    for (geo::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers () && draw_shapes; ++l) {

      geo::RecursiveShapeIterator shapes (layout, cell, (*l).first); 
      while (! shapes.at_end ()) {

        for (geo::CellInstArray::iterator arr = inst.begin (); ! arr.at_end (); ++arr) {
          r.draw (*shapes, tr * inst.complex_trans (*arr) * shapes.trans (), fill, contour, 0 /*use vertex for origin*/, text);
        }

        ++shapes;

      }

    }

  }

  // render the origins
  if (render_origins && vertex) {

    for (geo::CellInstArray::iterator arr = inst.begin (); ! arr.at_end (); ++arr) {
      geo::DPoint dp = geo::DPoint () + (tr * inst.complex_trans (*arr)).disp ();
      r.draw (geo::DEdge (dp, dp), 0, 0, vertex, 0);
    }

  }
}

// ------------------------------------------------------------------------

MarkerBase::MarkerBase (lay::LayoutView *view)
  : lay::ViewObject (view->view_object_widget ()),
    m_line_width (-1), m_vertex_size (-1), m_halo (-1), m_text_enabled (true), m_vertex_shape (lay::ViewOp::Rect), m_line_style (-1), m_dither_pattern (-1), m_frame_pattern (0), mp_view (view)
{ 
  // .. nothing yet ..
}

void 
MarkerBase::set_frame_color (QColor color)
{
  if (color != m_frame_color) {
    m_frame_color = color;
    redraw ();
  }
}

void 
MarkerBase::set_color (QColor color)
{
  if (color != m_color) {
    m_color = color;
    redraw ();
  }
}

void 
MarkerBase::set_line_width (int lw)
{
  if (m_line_width != lw) {
    m_line_width = lw;
    redraw ();
  }
}

void 
MarkerBase::set_vertex_shape (lay::ViewOp::Shape vs)
{
  if (m_vertex_shape != vs) {
    m_vertex_shape = vs;
    redraw ();
  }
}

void 
MarkerBase::set_vertex_size (int vs)
{
  if (m_vertex_size != vs) {
    m_vertex_size = vs;
    redraw ();
  }
}

void 
MarkerBase::set_halo (int halo)
{
  if (m_halo != halo) {
    m_halo = halo;
    redraw ();
  }
}

void
MarkerBase::set_text_enabled (bool en)
{
  if (m_text_enabled != en) {
    m_text_enabled = en;
    redraw ();
  }
}

void
MarkerBase::set_frame_pattern (int frame_pattern)
{
  if (m_frame_pattern != frame_pattern) {
    m_frame_pattern = frame_pattern;
    redraw ();
  }
}

void 
MarkerBase::set_dither_pattern (int dither_pattern)
{
  if (m_dither_pattern != dither_pattern) {
    m_dither_pattern = dither_pattern;
    redraw ();
  }
}

void
MarkerBase::set_line_style (int line_style)
{
  if (m_line_style != line_style) {
    m_line_style = line_style;
    redraw ();
  }
}

void
MarkerBase::get_bitmaps (const Viewport & /*vp*/, ViewObjectCanvas &canvas, lay::CanvasPlane *&fill, lay::CanvasPlane *&contour, lay::CanvasPlane *&vertex, lay::CanvasPlane *&text)
{
  double resolution = canvas.resolution ();
  int basic_width = int(0.5 + 1.0 / resolution);

  //  obtain bitmaps
  QColor color = m_color;
  if (! color.isValid ()) {
    color = mp_view->default_marker_color ();
  }
  if (! color.isValid ()) {
    color = canvas.foreground_color ();
  }

  QColor frame_color = m_frame_color;
  if (! frame_color.isValid ()) {
    frame_color = color;
  }

  int line_width = m_line_width < 0 ? mp_view->default_marker_line_width () : m_line_width;
  int vertex_size = m_vertex_size < 0 ? mp_view->default_marker_vertex_size () : m_vertex_size;
  bool halo = m_halo < 0 ? mp_view->default_marker_halo () : (m_halo != 0);
  int dither_pattern = m_dither_pattern < 0 ? mp_view->default_dither_pattern () : m_dither_pattern;
  int line_style = m_line_style < 0 ? mp_view->default_line_style () : m_line_style;

  if (halo) {

    std::vector <lay::ViewOp> ops;
    ops.resize (2);

    if (dither_pattern >= 0) {
      ops[0] = lay::ViewOp (canvas.background_color ().rgb (), lay::ViewOp::Copy, 0, (unsigned int) dither_pattern, 0, lay::ViewOp::Rect, 3 * basic_width, 0);
      ops[1] = lay::ViewOp (color.rgb (), lay::ViewOp::Copy, 0, (unsigned int) dither_pattern, 0, lay::ViewOp::Rect, basic_width, 1);
      fill = canvas.plane (ops);
    } else {
      fill = 0;
    }

    ops[0] = lay::ViewOp (canvas.background_color ().rgb (), lay::ViewOp::Copy, (unsigned int) line_style, (unsigned int) m_frame_pattern, 0, lay::ViewOp::Rect, line_width > 0 ? (line_width + 2) * basic_width : 0, 0);
    ops[1] = lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, (unsigned int) line_style, (unsigned int) m_frame_pattern, 0, lay::ViewOp::Rect, line_width * basic_width, 1);
    contour = canvas.plane (ops);

    if (! m_text_enabled) {
      text = 0;
    } else if (line_width == 1) {
      text = contour;
    } else {
      ops[0] = lay::ViewOp (canvas.background_color ().rgb (), lay::ViewOp::Copy, 0, 0, 0, lay::ViewOp::Rect, 3 * basic_width, 0);
      ops[1] = lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, 0, 0, 0, lay::ViewOp::Rect, basic_width, 1);
      text = canvas.plane (ops);
    }

    if (m_vertex_shape == lay::ViewOp::Rect) { 
      ops[0] = lay::ViewOp (canvas.background_color ().rgb (), lay::ViewOp::Copy, 0, 0, 0, m_vertex_shape, vertex_size > 0 ? (vertex_size + 2) * basic_width : 0, 0);
      ops[1] = lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, 0, 0, 0, m_vertex_shape, vertex_size * basic_width, 1);
      vertex = canvas.plane (ops);
    } else {
      std::vector <lay::ViewOp> ops1;
      ops1.resize (1);
      ops1[0] = lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, 0, 0, 0, m_vertex_shape, vertex_size * basic_width, 1);
      vertex = canvas.plane (ops1);
    }

  } else {

    if (dither_pattern >= 0) {
      fill = canvas.plane (lay::ViewOp (color.rgb (), lay::ViewOp::Copy, 0, (unsigned int) dither_pattern, 0, lay::ViewOp::Rect, basic_width));
    } else {
      fill = 0;
    }

    contour = canvas.plane (lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, (unsigned int) line_style, (unsigned int) m_frame_pattern, 0, lay::ViewOp::Rect, line_width * basic_width));
    vertex = canvas.plane (lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, 0, 0, 0, m_vertex_shape, vertex_size * basic_width));
    if (! m_text_enabled) {
      text = 0;
    } else if (line_width == 1) {
      text = contour;
    } else {
      text = canvas.plane (lay::ViewOp (frame_color.rgb (), lay::ViewOp::Copy, 0, 0, 0, lay::ViewOp::Rect, basic_width));
    }

  }
}

// ------------------------------------------------------------------------

GenericMarkerBase::GenericMarkerBase (lay::LayoutView *view, unsigned int cv_index)
  : MarkerBase (view), mp_trans_vector (0), mp_view (view), m_cv_index (cv_index)
{ 
  // .. nothing yet ..
}

GenericMarkerBase::~GenericMarkerBase ()
{
  if (mp_trans_vector) {
    delete mp_trans_vector;
    mp_trans_vector = 0;
  }
}

void
GenericMarkerBase::set_trans (const geo::CplxTrans &trans)
{
  if (! m_trans.equal (trans)) {
    m_trans = trans;
    redraw ();
  }
}

void 
GenericMarkerBase::set (const geo::ICplxTrans &t1)
{
  if (mp_trans_vector) {
    delete mp_trans_vector;
    mp_trans_vector = 0;
  }
  m_trans = geo::CplxTrans (dbu ()) * t1;
  redraw ();
}

void
GenericMarkerBase::set (const geo::DCplxTrans &t1)
{
  if (mp_trans_vector) {
    delete mp_trans_vector;
    mp_trans_vector = 0;
  }
  //  Note: this cast is not really correct but we handle float and integer types in the same fashion now.
  m_trans = geo::CplxTrans (geo::DCplxTrans (dbu ()) * t1);
  redraw ();
}

void
GenericMarkerBase::set (const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  if (mp_trans_vector) {
    delete mp_trans_vector;
    mp_trans_vector = 0;
  }
  if (trans.size () == 1) {
    m_trans = trans [0] * geo::CplxTrans (dbu ()) * t1;
  } else {
    m_trans = geo::CplxTrans (dbu ()) * t1;
    mp_trans_vector = new std::vector<geo::DCplxTrans> (trans);
  }
  redraw ();
}

void
GenericMarkerBase::set (const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  if (mp_trans_vector) {
    delete mp_trans_vector;
    mp_trans_vector = 0;
  }
  if (trans.size () == 1) {
    //  Note: this cast is not really correct but we handle float and integer types in the same fashion now.
    m_trans = geo::CplxTrans (trans [0] * geo::DCplxTrans (dbu ()) * t1);
  } else {
    //  Note: this cast is not really correct but we handle float and integer types in the same fashion now.
    m_trans = geo::CplxTrans (geo::DCplxTrans (dbu ()) * t1);
    mp_trans_vector = new std::vector<geo::DCplxTrans> (trans);
  }
  redraw ();
}

geo::DBox
GenericMarkerBase::bbox () const
{
  const lay::CellView &cv = mp_view->cellview (m_cv_index);
  if (! cv.is_valid ()) {
    return geo::DBox ();
  }
  
  if (mp_trans_vector) {
    geo::DBox b;
    geo::DBox ib = item_bbox ();
    for (std::vector<geo::DCplxTrans>::const_iterator t = mp_trans_vector->begin (); t != mp_trans_vector->end (); ++t) {
      b += (*t * geo::DCplxTrans (m_trans)) * ib;
    }
    return b;
  } else {
    return geo::DCplxTrans (m_trans) * item_bbox ();
  }
}

const geo::Layout *
GenericMarkerBase::layout () const
{
  if (m_cv_index >= (unsigned int) (mp_view->cellviews ())) {
    return 0;
  }

  const lay::CellView &cv = mp_view->cellview (m_cv_index);
  if (! cv.is_valid ()) {
    return 0;
  } else {
    return &cv->layout ();
  }
}

double
GenericMarkerBase::dbu () const
{
  const geo::Layout *ly = layout ();
  return ly ? ly->dbu () : 1.0;
}

// ------------------------------------------------------------------------

InstanceMarker::InstanceMarker (lay::LayoutView *view, unsigned int cv_index, bool draw_outline, size_t max_shapes)
  : GenericMarkerBase (view, cv_index), m_draw_outline (draw_outline), m_max_shapes (max_shapes), m_inst ()
{ 
  // .. nothing yet ..
}

InstanceMarker::~InstanceMarker ()
{
  // .. nothing yet ..
}

void 
InstanceMarker::set (const geo::Instance &instance, const geo::ICplxTrans &trans)
{
  m_inst = instance;
  GenericMarkerBase::set (trans);
}

void 
InstanceMarker::set (const geo::Instance &instance, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  m_inst = instance;
  GenericMarkerBase::set (t1, trans);
}

void 
InstanceMarker::render (const Viewport &vp, ViewObjectCanvas &canvas)
{
  const geo::Layout *ly = layout ();
  if (! ly) {
    return;
  }

  lay::CanvasPlane *fill, *contour, *vertex, *text;
  get_bitmaps (vp, canvas, fill, contour, vertex, text);
  if (contour == 0 && vertex == 0 && fill == 0 && text == 0) {
    return;
  }

  lay::Renderer &r = canvas.renderer ();
  bool label_transform = view ()->cell_box_text_transform ();
  int min_size = view ()->min_inst_label_size ();

  geo::box_convert<geo::CellInst> bc (*ly);

  if (! trans_vector ()) {
    render_cell_inst (*ly, m_inst.cell_inst (), vp.trans () * trans (), r, view ()->cell_box_text_font (), fill, contour, vertex, text, label_transform, min_size, m_draw_outline, m_max_shapes);
  } else {
    for (std::vector<geo::DCplxTrans>::const_iterator tr = trans_vector ()->begin (); tr != trans_vector ()->end (); ++tr) {
      render_cell_inst (*ly, m_inst.cell_inst (), vp.trans () * *tr * trans (), r, view ()->cell_box_text_font (), fill, contour, vertex, text, label_transform, min_size, m_draw_outline, m_max_shapes);
    }
  }
}

void 
InstanceMarker::set_draw_outline (bool d)
{
  if (d != m_draw_outline) {
    m_draw_outline = d;
    redraw ();
  }
}

void 
InstanceMarker::set_max_shapes (size_t s)
{
  if (s != m_max_shapes) {
    m_max_shapes = s;
    redraw ();
  }
}

geo::DBox
InstanceMarker::item_bbox () const 
{
  return geo::DBox (m_inst.bbox ());
}

// ------------------------------------------------------------------------

ShapeMarker::ShapeMarker (lay::LayoutView *view, unsigned int cv_index)
  : GenericMarkerBase (view, cv_index), m_shape ()
{ 
  // .. nothing yet ..
}

ShapeMarker::~ShapeMarker ()
{
  // .. nothing yet ..
}

void 
ShapeMarker::set (const geo::Shape &shape, const geo::ICplxTrans &trans)
{
  m_shape = shape;
  GenericMarkerBase::set (trans);
}

void 
ShapeMarker::set (const geo::Shape &shape, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  m_shape = shape;
  GenericMarkerBase::set (t1, trans);
}

void 
ShapeMarker::render (const Viewport &vp, ViewObjectCanvas &canvas)
{
  const geo::Layout *ly = layout ();
  if (! ly) {
    return;
  }

  lay::CanvasPlane *fill, *contour, *vertex, *text; 
  get_bitmaps (vp, canvas, fill, contour, vertex, text);
  if (contour == 0 && vertex == 0 && fill == 0 && text == 0) {
    return;
  }

  lay::Renderer &r = canvas.renderer ();

  r.set_font (geo::Font (view ()->text_font ()));
  r.apply_text_trans (view ()->apply_text_trans ());
  r.default_text_size (geo::Coord (view ()->default_text_size () / ly->dbu ()));
  r.set_precise (true);

  if (trans_vector ()) {
    for (std::vector<geo::DCplxTrans>::const_iterator tr = trans_vector ()->begin (); tr != trans_vector ()->end (); ++tr) {
      geo::CplxTrans t = vp.trans () * *tr * trans ();
      r.draw (m_shape, t, fill, contour, vertex, text);
      r.draw_propstring (m_shape, &ly->properties_repository (), text, t);
    }
  } else {
    geo::CplxTrans t = vp.trans () * trans ();
    r.draw (m_shape, t, fill, contour, vertex, text);
    r.draw_propstring (m_shape, &ly->properties_repository (), text, t);
  }
}

geo::DBox
ShapeMarker::item_bbox () const 
{
  return geo::DBox (m_shape.bbox ());
}

// ------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  Marker implementation

Marker::Marker (lay::LayoutView *view, unsigned int cv_index, bool draw_outline, size_t max_shapes)
  : GenericMarkerBase (view, cv_index), m_draw_outline (draw_outline), m_max_shapes (max_shapes) 
{ 
  m_type = None;
  m_object.any = 0;
}

Marker::~Marker ()
{
  remove_object ();
}

void
Marker::set ()
{
  remove_object ();
  redraw ();
}

void 
Marker::set (const geo::Box &box, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Box;
  m_object.box = new geo::Box (box);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DBox &box, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DBox;
  m_object.dbox = new geo::DBox (box);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DBox &box, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DBox;
  m_object.dbox = new geo::DBox (box);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::Box &box, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Box;
  m_object.box = new geo::Box (box);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::Polygon &poly, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Polygon;
  m_object.polygon = new geo::Polygon (poly);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::Polygon &poly, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Polygon;
  m_object.polygon = new geo::Polygon (poly);

  GenericMarkerBase::set (t1, trans);
}

void
Marker::set (const geo::PolygonRef &poly, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = PolygonRef;
  m_object.polygon_ref = new geo::PolygonRef (poly);

  GenericMarkerBase::set (trans);
}

void
Marker::set (const geo::PolygonRef &poly, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = PolygonRef;
  m_object.polygon_ref = new geo::PolygonRef (poly);

  GenericMarkerBase::set (t1, trans);
}

void
Marker::set (const geo::DPolygon &poly, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DPolygon;
  m_object.dpolygon = new geo::DPolygon (poly);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DPolygon &poly, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DPolygon;
  m_object.dpolygon = new geo::DPolygon (poly);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::EdgePair &edge_pair, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = EdgePair;
  m_object.edge_pair = new geo::EdgePair (edge_pair);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::EdgePair &edge_pair, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = EdgePair;
  m_object.edge_pair = new geo::EdgePair (edge_pair);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::DEdgePair &edge_pair, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DEdgePair;
  m_object.dedge_pair = new geo::DEdgePair (edge_pair);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DEdgePair &edge_pair, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DEdgePair;
  m_object.dedge_pair = new geo::DEdgePair (edge_pair);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::Edge &edge, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Edge;
  m_object.edge = new geo::Edge (edge);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::Edge &edge, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Edge;
  m_object.edge = new geo::Edge (edge);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::DEdge &edge, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DEdge;
  m_object.dedge = new geo::DEdge (edge);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DEdge &edge, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DEdge;
  m_object.dedge = new geo::DEdge (edge);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::Path &path, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Path;
  m_object.path = new geo::Path (path);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::Path &path, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Path;
  m_object.path = new geo::Path (path);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::DPath &path, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DPath;
  m_object.dpath = new geo::DPath (path);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DPath &path, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DPath;
  m_object.dpath = new geo::DPath (path);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::Text &text, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Text;
  m_object.text = new geo::Text (text);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::Text &text, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Text;
  m_object.text = new geo::Text (text);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::DText &text, const geo::DCplxTrans &trans)
{
  remove_object ();

  m_type = DText;
  m_object.dtext = new geo::DText (text);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::DText &text, const geo::DCplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = DText;
  m_object.dtext = new geo::DText (text);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set (const geo::CellInstArray &instance, const geo::ICplxTrans &trans)
{
  remove_object ();

  m_type = Instance;
  m_object.inst = new geo::CellInstArray (instance);

  GenericMarkerBase::set (trans);
}

void 
Marker::set (const geo::CellInstArray &instance, const geo::ICplxTrans &t1, const std::vector<geo::DCplxTrans> &trans)
{
  remove_object ();

  m_type = Instance;
  m_object.inst = new geo::CellInstArray (instance);

  GenericMarkerBase::set (t1, trans);
}

void 
Marker::set_draw_outline (bool d)
{
  if (d != m_draw_outline) {
    m_draw_outline = d;
    redraw ();
  }
}

void 
Marker::set_max_shapes (size_t s)
{
  if (s != m_max_shapes) {
    m_max_shapes = s;
    redraw ();
  }
}

geo::DBox
Marker::item_bbox () const
{
  if (m_type == Box) {
    return geo::DBox (*m_object.box);
  } else if (m_type == DBox) {
    return *m_object.dbox;
  } else if (m_type == Polygon) {
    return geo::DBox (m_object.polygon->box ());
  } else if (m_type == PolygonRef) {
    return geo::DBox (m_object.polygon_ref->box ());
  } else if (m_type == DPolygon) {
    return m_object.dpolygon->box ();
  } else if (m_type == EdgePair) {
    return geo::DBox (m_object.edge_pair->bbox ());
  } else if (m_type == DEdgePair) {
    return m_object.dedge_pair->bbox ();
  } else if (m_type == Edge) {
    return geo::DBox (m_object.edge->bbox ());
  } else if (m_type == DEdge) {
    return m_object.dedge->bbox ();
  } else if (m_type == Path) {
    return geo::DBox (m_object.path->box ());
  } else if (m_type == DPath) {
    return m_object.dpath->box ();
  } else if (m_type == Text) {
    return geo::DBox (m_object.text->box ());
  } else if (m_type == DText) {
    return m_object.dtext->box ();
  } else if (m_type == Instance) {
    const geo::Layout *ly = layout ();
    if (ly) {
      return geo::DBox (m_object.inst->bbox (geo::box_convert <geo::CellInst> (*ly)));
    }
  }
  return geo::DBox ();
}

void
Marker::remove_object ()
{
  if (m_type == Box) {
    delete m_object.box;
  } else if (m_type == DBox) {
    delete m_object.dbox;
  } else if (m_type == Polygon) {
    delete m_object.polygon;
  } else if (m_type == PolygonRef) {
    delete m_object.polygon_ref;
  } else if (m_type == DPolygon) {
    delete m_object.dpolygon;
  } else if (m_type == EdgePair) {
    delete m_object.edge_pair;
  } else if (m_type == DEdgePair) {
    delete m_object.dedge_pair;
  } else if (m_type == Edge) {
    delete m_object.edge;
  } else if (m_type == DEdge) {
    delete m_object.dedge;
  } else if (m_type == Path) {
    delete m_object.path;
  } else if (m_type == DPath) {
    delete m_object.dpath;
  } else if (m_type == Text) {
    delete m_object.text;
  } else if (m_type == DText) {
    delete m_object.dtext;
  } else if (m_type == Instance) {
    delete m_object.inst;
  } 

  m_type = None;
  m_object.any = 0;
}

void 
Marker::draw (lay::Renderer &r, const geo::CplxTrans &t, lay::CanvasPlane *fill, lay::CanvasPlane *contour, lay::CanvasPlane *vertex, lay::CanvasPlane *text)
{
  if (m_type == Box) {
    r.draw (*m_object.box, t, fill, contour, vertex, text);
  } else if (m_type == DBox) {
    r.draw (*m_object.dbox, geo::DCplxTrans (t), fill, contour, vertex, text);
  } else if (m_type == Polygon) {
    r.draw (*m_object.polygon, t, fill, contour, vertex, text);
  } else if (m_type == PolygonRef) {
    r.draw (m_object.polygon_ref->obj (), t * geo::ICplxTrans (m_object.polygon_ref->trans ()), fill, contour, vertex, text);
  } else if (m_type == DPolygon) {
    r.draw (*m_object.dpolygon, geo::DCplxTrans (t), fill, contour, vertex, text);
  } else if (m_type == Path) {
    r.draw (*m_object.path, t, fill, contour, vertex, text);
  } else if (m_type == DPath) {
    r.draw (*m_object.dpath, geo::DCplxTrans (t), fill, contour, vertex, text);
  } else if (m_type == Text) {
    r.draw (*m_object.text, t, fill, contour, vertex, text);
  } else if (m_type == DText) {
    r.draw (*m_object.dtext, geo::DCplxTrans (t), fill, contour, vertex, text);
  } else if (m_type == Edge) {
    r.draw (*m_object.edge, t, fill, contour, vertex, text);
  } else if (m_type == DEdge) {
    r.draw (*m_object.dedge, geo::DCplxTrans (t), fill, contour, vertex, text);
  } else if (m_type == EdgePair) {
    r.draw (m_object.edge_pair->first (), t, fill, contour, vertex, text);
    r.draw (m_object.edge_pair->second (), t, fill, contour, vertex, text);
    geo::Polygon poly = m_object.edge_pair->normalized ().to_polygon (0);
    r.draw (poly, t, fill, 0, 0, 0);
  } else if (m_type == DEdgePair) {
    r.draw (m_object.dedge_pair->first (), geo::DCplxTrans (t), fill, contour, vertex, text);
    r.draw (m_object.dedge_pair->second (), geo::DCplxTrans (t), fill, contour, vertex, text);
    geo::DPolygon poly = m_object.dedge_pair->normalized ().to_polygon (0);
    r.draw (poly, geo::DCplxTrans (t), fill, 0, 0, 0);
  } else if (m_type == Instance) {
    const lay::CellView &cv = view ()->cellview (cv_index ());
    bool label_transform = view ()->cell_box_text_transform ();
    int min_size = view ()->min_inst_label_size ();
    render_cell_inst (cv->layout (), *m_object.inst, t, r, view ()->cell_box_text_font (), fill, contour, vertex, text, label_transform, min_size, m_draw_outline, m_max_shapes);
  }
}

void 
Marker::render (const Viewport &vp, ViewObjectCanvas &canvas)
{ 
  lay::CanvasPlane *fill, *contour, *vertex, *text; 
  get_bitmaps (vp, canvas, fill, contour, vertex, text);
  if (contour == 0 && vertex == 0 && fill == 0 && text == 0) {
    return;
  }

  lay::Renderer &r = canvas.renderer ();

  r.set_font (geo::Font (view ()->text_font ()));
  r.apply_text_trans (view ()->apply_text_trans ());
  r.default_text_size (geo::Coord (view ()->default_text_size () / dbu ()));
  r.set_precise (true);

  if (! trans_vector ()) {
    geo::CplxTrans t = vp.trans () * trans ();
    draw (r, t, fill, contour, vertex, text);
  } else {
    for (std::vector<geo::DCplxTrans>::const_iterator tr = trans_vector ()->begin (); tr != trans_vector ()->end (); ++tr) {
      geo::CplxTrans t = vp.trans () * *tr * trans ();
      draw (r, t, fill, contour, vertex, text);
    }
  }
}

// ------------------------------------------------------------------------

DMarker::DMarker (lay::LayoutView *view)
  : MarkerBase (view), mp_view (view)
{ 
  m_type = None;
  m_object.any = 0;
}

DMarker::~DMarker ()
{
  remove_object ();
}

void 
DMarker::set (const geo::DBox &box)
{
  remove_object ();

  m_type = Box;
  m_object.box = new geo::DBox (box);

  redraw ();
}

void 
DMarker::set (const geo::DPolygon &poly)
{
  remove_object ();

  m_type = Polygon;
  m_object.polygon = new geo::DPolygon (poly);

  redraw ();
}

void 
DMarker::set (const geo::DEdgePair &edge_pair)
{
  remove_object ();

  m_type = EdgePair;
  m_object.edge_pair = new geo::DEdgePair (edge_pair);

  redraw ();
}

void 
DMarker::set (const geo::DEdge &edge)
{
  remove_object ();

  m_type = Edge;
  m_object.edge = new geo::DEdge (edge);

  redraw ();
}

void 
DMarker::set (const geo::DPath &path)
{
  remove_object ();

  m_type = Path;
  m_object.path = new geo::DPath (path);

  redraw ();
}

void 
DMarker::set (const geo::DText &text)
{
  remove_object ();

  m_type = Text;
  m_object.text = new geo::DText (text);

  redraw ();
}

geo::DBox
DMarker::bbox () const
{
  if (m_type == Box) {
    return *m_object.box;
  } else if (m_type == Polygon) {
    return m_object.polygon->box ();
  } else if (m_type == Edge) {
    return m_object.edge->bbox ();
  } else if (m_type == EdgePair) {
    return m_object.edge_pair->bbox ();
  } else if (m_type == Path) {
    return m_object.path->box ();
  } else if (m_type == Text) {
    return m_object.text->box ();
  } else {
    return geo::DBox ();
  }
}

void
DMarker::remove_object ()
{
  if (m_type == Box) {
    delete m_object.box;
  } else if (m_type == Polygon) {
    delete m_object.polygon;
  } else if (m_type == Edge) {
    delete m_object.edge;
  } else if (m_type == EdgePair) {
    delete m_object.edge_pair;
  } else if (m_type == Path) {
    delete m_object.path;
  } else if (m_type == Text) {
    delete m_object.text;
  } 

  m_type = None;
  m_object.any = 0;
}

void 
DMarker::render (const Viewport &vp, ViewObjectCanvas &canvas)
{ 
  lay::CanvasPlane *fill, *contour, *vertex, *text; 
  get_bitmaps (vp, canvas, fill, contour, vertex, text);
  if (contour == 0 && vertex == 0 && fill == 0 && text == 0) {
    return;
  }

  lay::Renderer &r = canvas.renderer ();

  r.set_font (geo::Font (mp_view->text_font ()));
  r.apply_text_trans (mp_view->apply_text_trans ());
  r.default_text_size (mp_view->default_text_size ());
  r.set_precise (true);

  geo::DCplxTrans t = vp.trans ();

  if (m_type == Box) {
    r.draw (*m_object.box, t, fill, contour, vertex, text);
  } else if (m_type == Polygon) {
    r.draw (*m_object.polygon, t, fill, contour, vertex, text);
  } else if (m_type == Path) {
    r.draw (*m_object.path, t, fill, contour, vertex, text);
  } else if (m_type == Text) {
    r.draw (*m_object.text, t, fill, contour, vertex, text);
  } else if (m_type == Edge) {
    r.draw (*m_object.edge, t, fill, contour, vertex, text);
  } else if (m_type == EdgePair) {
    r.draw (m_object.edge_pair->first (), t, fill, contour, vertex, text);
    r.draw (m_object.edge_pair->second (), t, fill, contour, vertex, text);
    geo::DPolygon poly = m_object.edge_pair->normalized ().to_polygon (0);
    r.draw (poly, t, fill, 0, 0, 0);
  }

}

}

