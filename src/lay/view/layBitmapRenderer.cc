#include "layBitmapRenderer.h"
#include "layBitmap.h"

namespace lay
{

// ----------------------------------------------------------------------------------------------
//  BitmapRenderer implementation

BitmapRenderer::BitmapRenderer (unsigned int width, unsigned int height, double resolution)
  : Renderer (width, height, resolution),
    m_xmin (0.0), m_xmax (0.0), m_ymin (0.0), m_ymax (0.0),
    m_ortho (true)
{
  // .. nothing else ..
}

void 
BitmapRenderer::reserve_edges (size_t n)
{
  m_edges.reserve (n);
}

void 
BitmapRenderer::reserve_texts (size_t n)
{
  m_texts.reserve (n);
}

void 
BitmapRenderer::clear ()
{
  //  this implementation is efficient but does not free memory - 
  //  the idea is to let the BitmapRenderer object manage it's workspace.
  m_edges.erase (m_edges.begin (), m_edges.end ());
  //  might be manhattan
  m_ortho = true;
  //  see above
  m_texts.erase (m_texts.begin (), m_texts.end ());
}

void 
BitmapRenderer::insert (const geo::DBox &box, const std::string &text, geo::Font font, geo::HAlign halign, geo::VAlign valign, geo::DFTrans trans)
{
  m_texts.push_back (lay::RenderText ());
  m_texts.back ().b = box;
  m_texts.back ().text = text;
  m_texts.back ().font = font;
  m_texts.back ().halign = halign;
  m_texts.back ().valign = valign;
  m_texts.back ().trans = trans;
}

void 
BitmapRenderer::draw (const geo::DBox &box, const std::string &text, geo::Font font, geo::HAlign halign, geo::VAlign valign, geo::DFTrans trans, 
                     lay::CanvasPlane * /*fill*/, lay::CanvasPlane * /*frame*/, lay::CanvasPlane * /*vertices*/, lay::CanvasPlane *texts)
{
  clear ();
  insert (box, text, font, halign, valign, trans);
  if (texts) {
    render_texts (*texts);
  }
}

void 
BitmapRenderer::insert (const geo::Box &b, const geo::CplxTrans &t)
{
  if (t.is_ortho ()) {
    insert (t * b);
  } else {
    insert (t * geo::Edge (b.p1 (), geo::Point (b.p1 ().x (), b.p2 ().y ())));
    insert (t * geo::Edge (geo::Point (b.p1 ().x (), b.p2 ().y ()), b.p2 ()));
    insert (t * geo::Edge (b.p2 (), geo::Point (b.p2 ().x (), b.p1 ().y ())));
    insert (t * geo::Edge (geo::Point (b.p2 ().x (), b.p1 ().y ()), b.p1 ()));
  }
}

void 
BitmapRenderer::insert (const geo::DBox &b, const geo::DCplxTrans &t)
{
  if (t.is_ortho ()) {
    insert (t * b);
  } else {
    insert (t * geo::DEdge (b.p1 (), geo::DPoint (b.p1 ().x (), b.p2 ().y ())));
    insert (t * geo::DEdge (geo::DPoint (b.p1 ().x (), b.p2 ().y ()), b.p2 ()));
    insert (t * geo::DEdge (b.p2 (), geo::DPoint (b.p2 ().x (), b.p1 ().y ())));
    insert (t * geo::DEdge (geo::DPoint (b.p2 ().x (), b.p1 ().y ()), b.p1 ()));
  }
}

void 
BitmapRenderer::insert (const geo::DBox &b)
{
  geo::DEdge edges [] = {
    geo::DEdge (b.p1 (), geo::DPoint (b.p1 ().x (), b.p2 ().y ())),
    geo::DEdge (geo::DPoint (b.p1 ().x (), b.p2 ().y ()), b.p2 ()),
    geo::DEdge (b.p2 (), geo::DPoint (b.p2 ().x (), b.p1 ().y ())),
    geo::DEdge (geo::DPoint (b.p2 ().x (), b.p1 ().y ()), b.p1 ())
  };

  if (m_edges.begin () == m_edges.end ()) {
    m_xmin = b.left ();
    m_xmax = b.right ();
    m_ymin = b.bottom ();
    m_ymax = b.top ();
  } else {
    m_xmin = std::min (m_xmin, b.left ());
    m_xmax = std::max (m_xmax, b.right ());
    m_ymin = std::min (m_ymin, b.bottom ());
    m_ymax = std::max (m_ymax, b.top ());
  }

  m_edges.insert (m_edges.end (), edges, edges + 4);
}

void 
BitmapRenderer::insert (const geo::DEdge &e)
{
  if (m_edges.begin () == m_edges.end ()) {
    m_xmin = std::min (e.x1 (), e.x2 ());
    m_xmax = std::max (e.x1 (), e.x2 ());
    m_ymin = std::min (e.y1 (), e.y2 ());
    m_ymax = std::max (e.y1 (), e.y2 ());
  } else {
    m_xmin = std::min (m_xmin, std::min (e.x1 (), e.x2 ()));
    m_xmax = std::max (m_xmax, std::max (e.x1 (), e.x2 ()));
    m_ymin = std::min (m_ymin, std::min (e.y1 (), e.y2 ()));
    m_ymax = std::max (m_ymax, std::max (e.y1 (), e.y2 ()));
  }

  //  check, if the edge is neither horizontal nor vertical - 
  //  reset the orthogonal flag in this case.
  if (m_ortho && fabs (e.x1 () - e.x2 ()) > render_epsilon 
              && fabs (e.y1 () - e.y2 ()) > render_epsilon) {
    m_ortho = false;
  }

  m_edges.push_back (e);
}

static inline bool point_inside_box (const geo::DPoint &pt, const geo::DBox &box)
{
  return (! (geo::coord_traits<geo::DBox::coord_type>::equal (pt.x (), box.left ()) || geo::coord_traits<geo::DBox::coord_type>::equal (pt.x (), box.right ())) &&
          ! (geo::coord_traits<geo::DBox::coord_type>::equal (pt.y (), box.bottom ()) || geo::coord_traits<geo::DBox::coord_type>::equal (pt.y (), box.top ())));
}

void
BitmapRenderer::add_xfill ()
{
  geo::DBox box;
  for (std::vector<lay::RenderEdge>::const_iterator e = m_edges.begin (); e != m_edges.end (); ++e) {
    if (! e->is_ortho ()) {
      return;
    }
    box += e->p1 ();
    box += e->p2 ();
  }

  if (! box.empty () && box.area () > 0.0) {

    for (std::vector<lay::RenderEdge>::const_iterator e = m_edges.begin (); e != m_edges.end (); ++e) {
      if (point_inside_box (e->p1 (), box) || point_inside_box (e->p2 (), box)) {
        return;
      }
    }

    insert (geo::DEdge (box.p1 (), box.p2 ()));
    insert (geo::DEdge (box.lower_right (), box.upper_left ()));

  }
}

void 
BitmapRenderer::render_texts (lay::CanvasPlane &plane)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (&plane);
  for (std::vector<lay::RenderText>::const_iterator t = m_texts.begin (); t != m_texts.end (); ++t) {
    bitmap->render_text (*t);
  }
}

void 
BitmapRenderer::render_vertices (lay::CanvasPlane &plane, int mode)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (&plane);

  //  a basic shortcut if there are no edges to render
  if (m_edges.end () == m_edges.begin ()) {
    return;
  }

  //  basic optimization: just a dot
  if (floor (m_xmax + 0.5) == floor (m_xmin + 0.5) &&
      floor (m_ymax + 0.5) == floor (m_ymin + 0.5)) {
    if (m_xmin > -0.5 && m_ymin > -0.5 && 
        m_xmin < double (bitmap->width ()) - 0.5 &&
        m_ymin < double (bitmap->height ()) - 0.5) {
      unsigned int yint = (unsigned int) (m_ymin + 0.5);
      unsigned int xint = (unsigned int) (m_xmin + 0.5);
      bitmap->fill (yint, xint, xint + 1);
    }
    return;
  }

  bitmap->render_vertices (m_edges, mode);
}

void 
BitmapRenderer::render_contour (lay::CanvasPlane &plane)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (&plane);

  //  a basic shortcut if there are no edges to render
  if (m_edges.end () == m_edges.begin ()) {
    return;
  }

  //  basic optimizations: outside window
  if (m_xmax < -0.5 || m_xmin > bitmap->width () - 0.5 ||
      m_ymax < -0.5 || m_ymin > bitmap->height () - 0.5) {
    return;
  }

  //  basic optimization: just a line or a dot
  if (floor (m_xmax + 0.5) == floor (m_xmin + 0.5)) {
    unsigned int y1int = (unsigned int) (std::max (0.0, std::min (m_ymin + 0.5, double (bitmap->height () - 1))));
    unsigned int y2int = (unsigned int) (std::max (0.0, std::min (m_ymax + 0.5, double (bitmap->height () - 1))));
    unsigned int xint = (unsigned int) (std::max (0.0, std::min (m_xmin + 0.5, double (bitmap->width () - 1))));
    for (unsigned int y = y1int; y <= y2int; ++y) {
      bitmap->fill (y, xint, xint + 1);
    }
    return;
  }
    
  if (floor (m_ymax + 0.5) == floor (m_ymin + 0.5)) {
    unsigned int x1int = (unsigned int) (std::max (0.0, std::min (m_xmin + 0.5, double (bitmap->width () - 1))));
    unsigned int x2int = (unsigned int) (std::max (0.0, std::min (m_xmax + 0.5, double (bitmap->width () - 1))));
    unsigned int yint = (unsigned int) (std::max (0.0, std::min (m_ymin + 0.5, double (bitmap->height () - 1))));
    bitmap->fill (yint, x1int, x2int + 1);
    return;
  }
  
  if (! m_ortho) {
    bitmap->render_contour (m_edges);
  } else {
    bitmap->render_contour_ortho (m_edges);
  }
}

void 
BitmapRenderer::render_fill (lay::CanvasPlane &plane)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (&plane);

  //  a basic shortcut if there are no edges to render
  if (m_edges.end () == m_edges.begin ()) {
    return;
  }

  //  basic optimizations: outside window
  if (m_xmax < -0.5 || m_xmin > bitmap->width () - 0.5 ||
      m_ymax < -0.5 || m_ymin > bitmap->height () - 0.5) {
    return;
  }

  //  basic optimization: just a line or a dot
  if (floor (m_xmax + 0.5) == floor (m_xmin + 0.5)) {
    unsigned int y1int = (unsigned int) (std::max (0.0, std::min (m_ymin + 0.5, double (bitmap->height () - 1))));
    unsigned int y2int = (unsigned int) (std::max (0.0, std::min (m_ymax + 0.5, double (bitmap->height () - 1))));
    unsigned int xint = (unsigned int) (std::max (0.0, std::min (m_xmin + 0.5, double (bitmap->width () - 1))));
    for (unsigned int y = y1int; y <= y2int; ++y) {
      bitmap->fill (y, xint, xint + 1);
    }
    return;
  }
    
  if (floor (m_ymax + 0.5) == floor (m_ymin + 0.5)) {
    unsigned int x1int = (unsigned int) (std::max (0.0, std::min (m_xmin + 0.5, double (bitmap->width () - 1))));
    unsigned int x2int = (unsigned int) (std::max (0.0, std::min (m_xmax + 0.5, double (bitmap->width () - 1))));
    unsigned int yint = (unsigned int) (std::max (0.0, std::min (m_ymin + 0.5, double (bitmap->height () - 1))));
    bitmap->fill (yint, x1int, x2int + 1);
    return;
  }

  if (m_ortho) {
    bitmap->render_fill_ortho (m_edges);
  } else {
    bitmap->render_fill (m_edges);
  }
}

void 
BitmapRenderer::render_dot (double x, double y, lay::CanvasPlane *plane)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (plane);

  x += 0.5;
  y += 0.5;

  //  basic optimizations: outside window
  if (x < 0.0 || x >= bitmap->width () ||
      y < 0.0 || y >= bitmap->height ()) {
    return;
  }

  unsigned int yint = (unsigned int) y;
  unsigned int xint = (unsigned int) x;
  bitmap->fill (yint, xint, xint + 1);
}
    
void 
BitmapRenderer::render_box (double xmin, double ymin, double xmax, double ymax, lay::CanvasPlane *plane)
{
  lay::Bitmap *bitmap = static_cast<lay::Bitmap *> (plane);

  xmin += 0.5;
  xmax += 0.5;
  ymin += 0.5;
  ymax += 0.5;

  //  basic optimizations: outside window
  if (xmax < 0.0 || xmin >= bitmap->width () ||
      ymax < 0.0 || ymin >= bitmap->height ()) {
    return;
  }

  unsigned int y1int = (unsigned int) (std::max (0.0, std::min (ymin, double (bitmap->height () - 1))));
  unsigned int y2int = (unsigned int) (std::max (0.0, std::min (ymax, double (bitmap->height () - 1))));
  unsigned int x1int = (unsigned int) (std::max (0.0, std::min (xmin, double (bitmap->width () - 1))));
  unsigned int x2int = (unsigned int) (std::max (0.0, std::min (xmax, double (bitmap->width () - 1))));
  for (unsigned int y = y1int; y <= y2int; ++y) {
    bitmap->fill (y, x1int, x2int + 1);
  }
}

void 
BitmapRenderer::draw (const geo::Shape &shape, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  if (shape.is_text ()) {
    
    geo::Point p = geo::Point () + shape.text_trans ().disp ();
    geo::DPoint dp = trans * p;

    if ((vertices || frame) &&
        dp.x () < m_width - 0.5 && dp.x () > -0.5 && 
        dp.y () < m_height - 0.5 && dp.y () > -0.5) {

      clear ();

      geo::Point pp = geo::Point (dp);

      //  generate a dot (cross, if marked) at the text's position
      if (vertices) {
        vertices->pixel (pp.x (), pp.y ());
      }
      if (frame) {
        frame->pixel (pp.x (), pp.y ());
      }

    }

    if (m_draw_texts && text) {

      geo::DFTrans fp (geo::DFTrans::r0);
      geo::DCoord h = trans.ctrans (m_default_text_size);
      geo::Font font = shape.text_font () == geo::NoFont ? m_font : shape.text_font ();

      if (m_apply_text_trans && font != geo::NoFont && font != geo::DefaultFont) {
        fp = geo::DFTrans (trans.fp_trans () * shape.text_trans ());
        h = trans.ctrans (shape.text_size () > 0 ? shape.text_size () : m_default_text_size);
      }

      geo::HAlign halign = shape.text_halign ();
      geo::VAlign valign = shape.text_valign ();

      double fy = 0.0;
      if (valign == geo::VAlignBottom || valign == geo::NoVAlign) {
        fy = 1.0;
      } else if (valign == geo::VAlignTop) {
        fy = -1.0;
      }

      double fx = 0.0;
      if (halign == geo::HAlignLeft || halign == geo::NoHAlign) {
        fx = 1.0;
      } else if (halign == geo::HAlignRight) {
        fx = -1.0;
      }

      geo::DVector tp1 (geo::DVector (fx * 2.0, fy * 2.0 + (fy - 1) * 0.5 * h));
      geo::DVector tp2 (geo::DVector (fx * 2.0, fy * 2.0 + (fy + 1) * 0.5 * h));

      clear ();

      insert (geo::DBox (dp + fp (tp1), dp + fp (tp2)), shape.text_string (), font, halign, valign, fp);

      render_texts (*text);

    }

  } else {

    geo::Box bbox = shape.bbox ();
    double threshold = 1.0 / trans.mag ();

    if (bbox.width () <= threshold && bbox.height () <= threshold) {

      geo::DPoint dc = trans * bbox.center ();
      if (fill && ! shape.is_edge ()) {
        render_dot (dc.x (), dc.y (), fill);
      } 
      if (frame) {
        render_dot (dc.x (), dc.y (), frame);
      } 
      if (vertices) {
        render_dot (dc.x (), dc.y (), vertices);
      }

    } else if (shape.is_box ()) {

      draw (bbox, trans, fill, frame, vertices, text);

    } else if (shape.is_polygon ()) {

      //  simplify to a rectangle if possible
      geo::Box b (shape.bbox ());
      if (simplify_box (b, trans)) {
        draw (b, trans, fill, frame, vertices, text);
      } else {

        clear ();
        geo::Shape::polygon_edge_iterator e = shape.begin_edge (); 
        for ( ; ! e.at_end (); ++e) {
          insert (trans * *e);
        }

        if (vertices) {
          render_vertices (*vertices, 1);
        }
        if (fill) {
          render_fill (*fill);
        }
        if (frame) {
          if (m_xfill) {
            add_xfill ();
          }
          render_contour (*frame);
        }

      }

    } else if (shape.is_edge ()) {

      draw (shape.edge (), trans, fill, frame, vertices, text);

    } else if (shape.is_path ()) {

      //  simplify to a rectangle if possible
      geo::Box b (shape.bbox ());
      if (simplify_box (b, trans)) {
        draw (b, trans, fill, frame, vertices, text);
      } else {

        //  instantiate the path and draw
        geo::Path path;
        shape.path (path);
        draw (path, trans, fill, frame, vertices, text);

      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::Polygon &poly, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  //  simplify to a rectangle if possible
  geo::Box b (poly.box ());

  double threshold = 1.0 / trans.mag ();
  if (b.width () < threshold && b.height () < threshold) {

    geo::DPoint dp = trans * b.center ();
    if (fill) {
      render_dot (dp.x (), dp.y (), fill);
    }
    if (frame) {
      render_dot (dp.x (), dp.y (), frame);
    }
    if (vertices) {
      render_dot (dp.x (), dp.y (), vertices);
    }

  } else {

    clear ();

    bool xfill = m_xfill;

    if (simplify_box (b, trans)) {
      xfill = false;
      insert (trans * b);
    } else {
      for (geo::Polygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
        insert (trans * *e);
      }
    }

    if (vertices) {
      render_vertices (*vertices, 1);
    }
    if (fill) {
      render_fill (*fill);
    }
    if (frame) {
      if (xfill) {
        add_xfill ();
      }
      render_contour (*frame);
    }

  }
}

void 
BitmapRenderer::draw (const geo::DPolygon &poly, const geo::DCplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  //  simplify to a rectangle or dot if possible
  geo::DBox b (poly.box ());

  double threshold = 1.0 / trans.mag ();
  if (b.width () < threshold && b.height () < threshold) {

    geo::DPoint dp = trans * b.center ();
    if (fill) {
      render_dot (dp.x (), dp.y (), fill);
    }
    if (frame) {
      render_dot (dp.x (), dp.y (), frame);
    }
    if (vertices) {
      render_dot (dp.x (), dp.y (), vertices);
    }

  } else {

    clear ();

    bool xfill = m_xfill;

    if (simplify_box (b, trans)) {
      xfill = false;
      insert (trans * b);
    } else {
      for (geo::DPolygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
        insert (trans * *e);
      }
    }

    if (vertices) {
      render_vertices (*vertices, 1);
    }
    if (fill) {
      render_fill (*fill);
    }
    if (frame) {
      if (xfill) {
        add_xfill ();
      }
      render_contour (*frame);
    }

  }
}

void 
BitmapRenderer::draw (const geo::DPolygon &poly, 
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  //  simplify to a rectangle or dot if possible
  geo::DBox b (poly.box ());

  if (b.width () < 1.0 && b.height () < 1.0) {

    geo::DPoint dp = b.center ();
    if (fill) {
      render_dot (dp.x (), dp.y (), fill);
    }
    if (frame) {
      render_dot (dp.x (), dp.y (), frame);
    }
    if (vertices) {
      render_dot (dp.x (), dp.y (), vertices);
    }

  } else {

    clear ();

    bool xfill = m_xfill;

    if (simplify_box (b, geo::DCplxTrans ())) {
      xfill = false;
      insert (b);
    } else {
      for (geo::DPolygon::polygon_edge_iterator e = poly.begin_edge (); ! e.at_end (); ++e) {
        insert (*e);
      }
    }

    if (vertices) {
      render_vertices (*vertices, 1);
    }
    if (fill) {
      render_fill (*fill);
    }
    if (frame) {
      if (xfill) {
        add_xfill ();
      }
      render_contour (*frame);
    }

  }
}

void 
BitmapRenderer::draw (const geo::ShortBox &box, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  if (! box.empty ()) {

    double threshold = 1.0 / trans.mag ();
    if (box.width () < threshold && box.height () < threshold) {

      geo::DPoint dp = trans * box.center ();
      if (fill) {
        render_dot (dp.x (), dp.y (), fill);
      }
      if (frame && frame != fill) {
        render_dot (dp.x (), dp.y (), frame);
      }
      if (vertices && vertices != fill) {
        render_dot (dp.x (), dp.y (), vertices);
      }

    } else {

      clear ();
      insert (geo::Box (box), trans);

      if (vertices) {
        render_vertices (*vertices, 2);
      }
      if (fill && (fill != frame || (box.width () > threshold && box.height () > threshold))) {
        render_fill (*fill);
      }
      if (frame) {
        if (m_xfill) {
          insert (trans * geo::Edge (box.p1 (), box.p2 ()));
          insert (trans * geo::Edge (box.lower_right (), box.upper_left ()));
        }
        render_contour (*frame);
      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::Box &box, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  if (! box.empty ()) {
  
    double threshold = 1.0 / trans.mag ();
    if (box.width () < threshold && box.height () < threshold) {

      geo::DPoint dp = trans * box.center ();
      if (fill) {
        render_dot (dp.x (), dp.y (), fill);
      }
      if (frame && frame != fill) {
        render_dot (dp.x (), dp.y (), frame);
      }
      if (vertices && vertices != fill) {
        render_dot (dp.x (), dp.y (), vertices);
      }

    } else {

      clear ();
      insert (box, trans);

      if (vertices) {
        render_vertices (*vertices, 2);
      }
      if (fill && (fill != frame || (box.width () > threshold && box.height () > threshold))) {
        render_fill (*fill);
      }
      if (frame) {
        if (m_xfill) {
          insert (trans * geo::Edge (box.p1 (), box.p2 ()));
          insert (trans * geo::Edge (box.lower_right (), box.upper_left ()));
        }
        render_contour (*frame);
      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::DBox &box, 
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  if (! box.empty ()) {
  
    if (box.width () < 1.0 && box.height () < 1.0) {

      geo::DPoint dp = box.center ();
      if (fill) {
        render_dot (dp.x (), dp.y (), fill);
      }
      if (frame && frame != fill) {
        render_dot (dp.x (), dp.y (), frame);
      }
      if (vertices && vertices != fill) {
        render_dot (dp.x (), dp.y (), vertices);
      }

    } else {

      clear ();
      insert (box);

      if (vertices) {
        render_vertices (*vertices, 2);
      }
      if (fill && (fill != frame || (box.width () > 1.0 && box.height () > 1.0))) {
        render_fill (*fill);
      }
      if (frame) {
        if (m_xfill) {
          insert (geo::DEdge (box.p1 (), box.p2 ()));
          insert (geo::DEdge (box.lower_right (), box.upper_left ()));
        }
        render_contour (*frame);
      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::DBox &box, const geo::DCplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  if (! box.empty ()) {
  
    double threshold = 1.0 / trans.mag ();
    if (box.width () < threshold && box.height () < threshold) {

      geo::DPoint dp = trans * box.center ();
      if (fill) {
        render_dot (dp.x (), dp.y (), fill);
      }
      if (frame && frame != fill) {
        render_dot (dp.x (), dp.y (), frame);
      }
      if (vertices && vertices != fill) {
        render_dot (dp.x (), dp.y (), vertices);
      }

    } else {

      clear ();
      insert (box, trans);

      if (vertices) {
        render_vertices (*vertices, 2);
      }
      if (fill && (fill != frame || (box.width () > threshold && box.height () > threshold))) {
        render_fill (*fill);
      }
      if (frame) {
        if (m_xfill) {
          insert (trans * geo::DEdge (box.p1 (), box.p2 ()));
          insert (trans * geo::DEdge (box.lower_right (), box.upper_left ()));
        }
        render_contour (*frame);
      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::Path &path, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  //  simplify to a rectangle or dot if possible 
  geo::Box b (path.box ());

  double threshold = 1.0 / trans.mag ();
  if (b.width () < threshold && b.height () < threshold) {

    geo::DPoint dp = trans * b.center ();
    if (fill) {
      render_dot (dp.x (), dp.y (), fill);
    }
    if (frame) {
      render_dot (dp.x (), dp.y (), frame);
    }
    if (vertices) {
      render_dot (dp.x (), dp.y (), vertices);
    }

  } else {

    clear ();

    if (simplify_box (b, trans)) {
      draw (b, trans, fill, frame, vertices, text);
      return;
    } 

    //  generate the hull and produce the edges from this only if the path is considerably wide
    //  otherwise just render the spine
    
    double w = trans.ctrans (path.width ());

    bool thin = (w < 0.5);
    bool quite_thin = (w < 3.0);

    //  render the border
    if (! thin) {

      geo::DPath::pointlist_type pts;
      path.transformed (trans).hull (pts);

      geo::DPath::pointlist_type::const_iterator p = pts.begin ();
      if (p != pts.end ()) {

        geo::DPath::pointlist_type::const_iterator pp = p;
        ++pp;

        while (pp != pts.end ()) {
          insert (geo::DEdge (*p, *pp));
          p = pp;
          ++pp;
        }

        insert (geo::DEdge (*p, pts [0]));
      }

      if (fill) {
        render_fill (*fill);
      }
      if (frame) {
        if (m_xfill) {
          add_xfill ();
        }
        render_contour (*frame);
      }

    }

    //  render the spine edges
    if (! quite_thin || thin) {

      clear ();

      geo::Path::iterator q = path.begin ();
      if (q != path.end ()) {

        geo::Path::iterator qq = q;
        ++qq;

        //  draw single-point as single-point edge to get the vertices
        if (qq == path.end ()) {
          insert (trans * geo::Edge (*q, *q));
        }

        bool first = true;
        while (qq != path.end ()) {

          geo::Edge seg (*q, *qq);
          q = qq;
          ++qq;

          //  If the path is simplified, the spine is drawn instead of the contour.
          //  We must apply the begin/end extensions then.
          
          if (thin && qq == path.end ()) {
            if (path.extensions ().second != 0 && (seg.dx () != 0 || seg.dy () != 0)) {
              geo::DVector ed (seg.p2 () - seg.p1 ());
              ed *= 1.0 / ed.double_length ();
              seg = geo::Edge (seg.p1 (), seg.p2 () + geo::Vector (ed * double (path.extensions ().second)));
            }
          } 

          if (first) {
            first = false;
            if (thin && path.extensions ().first != 0 && (seg.dx () != 0 || seg.dy () != 0)) {
              geo::DVector ed (seg.p2 () - seg.p1 ());
              ed *= 1.0 / ed.double_length ();
              seg = geo::Edge (seg.p1 () - geo::Vector (ed * double (path.extensions ().first)), seg.p2 ());
            }
          }
            
          insert (trans * seg);

        }

      }

      if (vertices) {
        render_vertices (*vertices, 0);
      }
      if (frame) {
        render_contour (*frame);
      }

    }

  }
}

void 
BitmapRenderer::draw (const geo::DPath &path, 
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  draw (path, geo::DCplxTrans (), fill, frame, vertices, text);
}

void 
BitmapRenderer::draw (const geo::DPath &path, const geo::DCplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  //  simplify to a rectangle if possible
  geo::DBox b (path.box ());

  double threshold = 1.0 / trans.mag ();
  if (b.width () < threshold && b.height () < threshold) {

    geo::DPoint dp = trans * b.center ();
    if (fill) {
      render_dot (dp.x (), dp.y (), fill);
    }
    if (frame) {
      render_dot (dp.x (), dp.y (), frame);
    }
    if (vertices) {
      render_dot (dp.x (), dp.y (), vertices);
    }

  } else {

    clear ();

    if (simplify_box (b, trans)) {
      draw (b, trans, fill, frame, vertices, text);
      return;
    } 

    //  generate the hull and produce the edges from this 

    geo::DPath::pointlist_type pts;
    path.hull (pts);

    geo::DPath::pointlist_type::const_iterator p = pts.begin ();
    if (p != pts.end ()) {

      geo::DPath::pointlist_type::const_iterator pp = p;
      ++pp;

      while (pp != pts.end ()) {
        insert (trans * geo::DEdge (*p, *pp));
        p = pp;
        ++pp;
      }

      insert (trans * geo::DEdge (*p, pts [0]));
    }

    if (fill) {
      render_fill (*fill);
    }
    if (frame) {
      if (m_xfill) {
        add_xfill ();
      }
      render_contour (*frame);
    }

    //  render the spine edges

    clear ();

    geo::DPath::iterator q = path.begin ();
    if (q != path.end ()) {

      geo::DPath::iterator qq = q;
      ++qq;

      //  draw single-point as single-point edge to get the vertices
      if (qq == path.end ()) {
        insert (trans * geo::DEdge (*q, *q));
      }

      while (qq != path.end ()) {
        insert (trans * geo::DEdge (*q, *qq));
        q = qq;
        ++qq;
      }

    }

    if (vertices) {
      render_vertices (*vertices, 0);
    }
    if (frame) {
      render_contour (*frame);
    }

  }
}

void 
BitmapRenderer::draw (const geo::Text &txt, const geo::CplxTrans &trans,
                      lay::CanvasPlane * /*fill*/, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  geo::Point p = geo::Point () + txt.trans ().disp ();
  geo::DPoint dp = trans * p;

  if ((vertices || frame) &&
      dp.x () < m_width - 0.5 && dp.x () > -0.5 && 
      dp.y () < m_height - 0.5 && dp.y () > -0.5) {

    clear ();

    geo::Point pp = geo::Point (dp);

    //  generate a dot (cross, if marked) at the text's position
    if (vertices) {
      vertices->pixel (pp.x (), pp.y ());
    }
    if (frame) {
      frame->pixel (pp.x (), pp.y ());
    }

  }

  if (m_draw_texts && text) {

    geo::DFTrans fp (geo::DFTrans::r0);
    geo::DCoord h = trans.ctrans (m_default_text_size);
    geo::Font font = txt.font () == geo::NoFont ? m_font : txt.font ();

    if (m_apply_text_trans && font != geo::NoFont && font != geo::DefaultFont) {
      fp = geo::DFTrans (trans.fp_trans () * txt.trans ());
      h = trans.ctrans (txt.size () > 0 ? txt.size () : m_default_text_size);
    }

    double fy = 0.0;
    if (txt.valign () == geo::VAlignBottom || txt.valign () == geo::NoVAlign) {
      fy = 1.0;
    } else if (txt.valign () == geo::VAlignTop) {
      fy = -1.0;
    }

    double fx = 0.0;
    if (txt.halign () == geo::HAlignLeft || txt.halign () == geo::NoHAlign) {
      fx = 1.0;
    } else if (txt.halign () == geo::HAlignRight) {
      fx = -1.0;
    }

    //  impose a 2 pixel offset to separate the text from the origin
    geo::DVector tp1 (geo::DVector (fx * 2.0, fy * 2.0 + (fy - 1) * 0.5 * h));
    geo::DVector tp2 (geo::DVector (fx * 2.0, fy * 2.0 + (fy + 1) * 0.5 * h));

    clear ();

    insert (geo::DBox (dp + fp (tp1), dp + fp (tp2)), txt.string (), font, txt.halign (), txt.valign (), fp);
    
    render_texts (*text);

  }

}

void 
BitmapRenderer::draw (const geo::DText &txt, 
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  draw (txt, geo::DCplxTrans (), fill, frame, vertices, text);
}

void 
BitmapRenderer::draw (const geo::DText &txt, const geo::DCplxTrans &trans,
                      lay::CanvasPlane * /*fill*/, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  geo::DPoint p = geo::DPoint () + txt.trans ().disp ();
  geo::DPoint dp = trans * p;

  if ((vertices || frame) &&
      dp.x () < m_width - 0.5 && dp.x () > -0.5 && 
      dp.y () < m_height - 0.5 && dp.y () > -0.5) {

    clear ();

    geo::Point pp (dp);

    //  generate a dot (cross, if marked) at the text's position
    if (vertices) {
      vertices->pixel (pp.x (), pp.y ());
    }
    if (frame) {
      frame->pixel (pp.x (), pp.y ());
    }

  }

  if (m_draw_texts && text) {

    geo::DFTrans fp (geo::DFTrans::r0);
    geo::DCoord h = trans.ctrans (m_default_text_size_dbl);
    geo::Font font = txt.font () == geo::NoFont ? m_font : txt.font ();

    if (m_apply_text_trans && font != geo::NoFont && font != geo::DefaultFont) {
      fp = trans.fp_trans () * geo::DFTrans (txt.trans ());
      h = trans.ctrans (txt.size () > 0 ? txt.size () : m_default_text_size_dbl);
    }

    double fy = 0.0;
    if (txt.valign () == geo::VAlignBottom || txt.valign () == geo::NoVAlign) {
      fy = 1.0;
    } else if (txt.valign () == geo::VAlignTop) {
      fy = -1.0;
    }

    double fx = 0.0;
    if (txt.halign () == geo::HAlignLeft || txt.halign () == geo::NoHAlign) {
      fx = 1.0;
    } else if (txt.halign () == geo::HAlignRight) {
      fx = -1.0;
    }

    geo::DVector tp1 (geo::DVector (fx * 2.0, fy * 2.0 + (fy - 1) * 0.5 * h));
    geo::DVector tp2 (geo::DVector (fx * 2.0, fy * 2.0 + (fy + 1) * 0.5 * h));

    clear ();

    insert (geo::DBox (dp + fp (tp1), dp + fp (tp2)), txt.string (), font, txt.halign (), txt.valign (), fp);
    
    render_texts (*text);

  }

}

void 
BitmapRenderer::draw (const geo::DEdge &edge, 
                      lay::CanvasPlane * /*fill*/, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane * /*text*/)
{
  if (fabs (edge.dy ()) < 1.0 && fabs (edge.dx ()) < 1.0) {

    double x = (edge.p1 ().x () + edge.p2 ().x ()) * 0.5;
    double y = (edge.p1 ().y () + edge.p2 ().y ()) * 0.5;
    if (frame) {
      render_dot (x, y, frame);
    }
    if (vertices) {
      render_dot (x, y, vertices);
    }

  } else {

    clear ();

    insert (edge);

    if (vertices) {
      render_vertices (*vertices, 0);
    }
    if (frame) {
      render_contour (*frame);
    }

  }
}

void 
BitmapRenderer::draw (const geo::Edge &edge, const geo::CplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  draw (trans * edge, fill, frame, vertices, text);
}

void 
BitmapRenderer::draw (const geo::DEdge &edge, const geo::DCplxTrans &trans,
                      lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertices, lay::CanvasPlane *text)
{
  draw (trans * edge, fill, frame, vertices, text);
}

template <class Box, class Trans>
bool 
BitmapRenderer::simplify_box (Box &b, const Trans &trans)
{
  const double small_size_threshold = 1.0;

  bool ortho = trans.is_ortho ();
  if (! m_precise && ((ortho && trans.ctrans (std::min (b.width (), b.height ())) < small_size_threshold) || 
                      (! ortho && trans.ctrans (std::max (b.width (), b.height ())) < small_size_threshold))) {

    if (trans.ctrans (b.width ()) < small_size_threshold) {
      typename Box::coord_type c = b.center ().x ();
      b.set_left (c);
      b.set_right (c);
    } 

    if (trans.ctrans (b.height ()) < small_size_threshold) {
      typename Box::coord_type c = b.center ().y ();
      b.set_top (c);
      b.set_bottom (c);
    } 

    return true;

  } else {
    return false;
  }
}

}

