#include "layRenderer.h"
#include "detail/utilsVariant.h"

#include "layCanvasPlane.h"
#include "laySnap.h"

#include "tlAlgorithm.h"

#include <algorithm>

namespace lay 
{

// ----------------------------------------------------------------------------------------------
//  Renderer implementation

Renderer::Renderer (unsigned int width, unsigned int height, double resolution)
  : m_draw_texts (true),
    m_draw_properties (false),
    m_draw_description_property (false),
    m_default_text_size (16),
    m_default_text_size_dbl (16),
    m_apply_text_trans (true),
    m_precise (false),
    m_xfill (false),
    m_font (geo::DefaultFont),
    m_width (width), m_height (height),
    m_resolution (resolution)
{
  // .. nothing else ..
}

void 
Renderer::draw_propstring (const geo::Shape &shape, const geo::PropertiesRepository *prep, lay::CanvasPlane *text, const geo::CplxTrans &trans)
{
  if (! shape.has_prop_id ()) {
    return;
  }

  geo::DPoint dp;

  if (shape.is_text ()) {
    dp = trans * (geo::Point () + shape.text_trans ().disp ());
  } else if (shape.is_box ()) {
    dp = trans (shape.box ().p1 ());
  } else if (shape.is_polygon ()) {
    geo::Shape::polygon_edge_iterator e = shape.begin_edge (); 
    dp = trans ((*e).p1 ());
  } else if (shape.is_edge ()) {
    dp = trans (shape.edge ().p1 ());
  } else if (shape.is_path ()) {
    dp = trans (*shape.begin_point ());
  } else {
    return;
  }

  if (shape.has_prop_id () && prep && text && (m_draw_properties || m_draw_description_property)) {
    if (m_draw_properties) {
      draw_propstring (shape.prop_id (), prep, dp, text, trans);
    }
    if (m_draw_description_property) {
      draw_description_propstring (shape.prop_id (), prep, dp, text, trans);
    }
  }
}

void 
Renderer::draw_propstring (geo::properties_id_type id, 
                           const geo::PropertiesRepository *prep, const geo::DPoint &pref, 
                           lay::CanvasPlane *text, const geo::CplxTrans &trans)
{
  geo::DPoint tp1 (pref + geo::DVector (2.0, -2.0));
  geo::DPoint tp2 (pref + geo::DVector (2.0, -2.0 - trans.ctrans (m_default_text_size)));

  std::string ptext;

  const char *sep = "";
  const geo::PropertiesRepository::properties_set &props = prep->properties (id);
  for (geo::PropertiesRepository::properties_set::const_iterator p = props.begin (); p != props.end (); ++p) {
    ptext += sep;
    sep = "\n";
    ptext += prep->prop_name (p->first).to_string ();
    ptext += ": ";
    ptext += p->second.to_string ();
  }

  draw (geo::DBox (tp1, tp2), ptext, m_font,
          geo::HAlignLeft, geo::VAlignTop, 
          geo::DFTrans (geo::DFTrans::r0), 0, 0, 0, text);
}

void 
Renderer::draw_description_propstring (geo::properties_id_type id, 
                                       const geo::PropertiesRepository *prep, const geo::DPoint &pref, 
                                       lay::CanvasPlane *text, const geo::CplxTrans &trans)
{
  geo::DPoint tp1 (pref + geo::DVector (5.0, -5.0));
  geo::DPoint tp2 (pref + geo::DVector (5.0, -5.0 - trans.ctrans (m_default_text_size)));

  const geo::PropertiesRepository::properties_set &props = prep->properties (id);
  //  TODO: get rid of this const_cast hack (i.e. by a mutable definition inside the properties repository)
  geo::property_names_id_type dn = (const_cast<geo::PropertiesRepository *> (prep))->prop_name_id (geo::Variant ("description"));

  geo::PropertiesRepository::properties_set::const_iterator dv = props.find (dn);
  if (dv != props.end ()) {

    draw (geo::DBox (tp1, tp2), dv->second.to_string (), m_font,
            geo::HAlignLeft, geo::VAlignTop, 
            geo::DFTrans (geo::DFTrans::r0), 0, 0, 0, text);

  }
}

}


