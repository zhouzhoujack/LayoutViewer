#include "layEdtPropertiesPageUtils.h"
#include "tlException.h"
#include "detail/utilsString.h"

#include "geoShapes.h"
#include "geoLayout.h"

#include <QLineEdit>

namespace edt
{

// -------------------------------------------------------------------------
//  CombinedChangeApplicator implementation

CombinedChangeApplicator::CombinedChangeApplicator ()
{
}
    
CombinedChangeApplicator::CombinedChangeApplicator (ChangeApplicator *a1)
{
  m_appl.push_back (a1);
}
    
CombinedChangeApplicator::CombinedChangeApplicator (ChangeApplicator *a1, ChangeApplicator *a2)
{
  m_appl.push_back (a1);
  m_appl.push_back (a2);
}
    
void CombinedChangeApplicator::add (ChangeApplicator *a)
{
  m_appl.push_back (a);
}
    
CombinedChangeApplicator::~CombinedChangeApplicator ()
{
  for (std::vector<ChangeApplicator *>::const_iterator a = m_appl.begin (); a != m_appl.end (); ++a) {
    delete *a;
  }
  m_appl.clear ();
}

bool CombinedChangeApplicator::supports_relative_mode () const 
{ 
  for (std::vector<ChangeApplicator *>::const_iterator a = m_appl.begin (); a != m_appl.end (); ++a) {
    if ((*a)->supports_relative_mode ()) {
      return true;
    }
  }
  return false; 
}

geo::Shape CombinedChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const 
{
  geo::Shape s = shape;
  for (std::vector<ChangeApplicator *>::const_iterator a = m_appl.begin (); a != m_appl.end (); ++a) {
    if (*a) {
      s = (*a)->do_apply (shapes, s, dbu, relative);
    }
  }
  return s;
}

geo::Instance CombinedChangeApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const 
{
  geo::Instance i = instance;
  for (std::vector<ChangeApplicator *>::const_iterator a = m_appl.begin (); a != m_appl.end (); ++a) {
    if (*a) {
      i = (*a)->do_apply_inst (cell, i, dbu, relative);
    }
  }
  return i;
}

// -------------------------------------------------------------------------
//  ChangePropertiesApplicator implementation

ChangePropertiesApplicator::ChangePropertiesApplicator (geo::properties_id_type prop_id) 
  : m_prop_id (prop_id)
{ 
  //  .. nothing yet ...
}

geo::Shape ChangePropertiesApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const 
{
  return shapes.replace_prop_id (shape, m_prop_id);
}

geo::Instance ChangePropertiesApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double /*dbu*/, bool /*relative*/) const 
{
  return cell.replace_prop_id (instance, m_prop_id);
}

// -------------------------------------------------------------------------
//  BoxDimensionsChangeApplicator implementation

BoxDimensionsChangeApplicator::BoxDimensionsChangeApplicator (geo::Coord dl, geo::Coord db, geo::Coord dr, geo::Coord dt, geo::Coord l, geo::Coord b, geo::Coord r, geo::Coord t)
  : m_dl (dl), m_db (db), m_dr (dr), m_dt (dt), m_l (l), m_b (b), m_r (r), m_t (t)
{
  //  .. nothing yet ..
}

geo::Shape BoxDimensionsChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool relative) const
{
  geo::Box org_box;
  shape.box (org_box);

  geo::Box new_box;
  if (relative) {
    new_box = geo::Box (org_box.left () + m_dl, 
                       org_box.bottom () + m_db,
                       org_box.right () + m_dr, 
                       org_box.top () + m_dt);
  } else {

    geo::Coord l = org_box.left ();
    geo::Coord r = org_box.right ();

    if (m_dl != 0 && m_dr == 0) {
      //  left side is fixed
      l = m_l;
    } else if (m_dl == 0 && m_dr != 0) {
      //  right side is fixed
      r = m_r;
    } else if (m_dl != 0 && m_dl == m_dr) {
      //  center is fixed
      l = (m_l + m_r) / 2 - org_box.width () / 2;
      r = (m_l + m_r) / 2 + org_box.width () / 2;
    } else if (m_dl != 0 && m_dl == -m_dr) {
      //  width is fixed
      l = org_box.center ().x () - (m_r - m_l) / 2;
      r = org_box.center ().x () + (m_r - m_l) / 2;
    } else if (m_dl != 0 && m_dr != 0) {
      //  both sides have changed
      l = m_l;
      r = m_r;
    }

    geo::Coord b = org_box.bottom ();
    geo::Coord t = org_box.top ();

    if (m_db != 0 && m_dt == 0) {
      //  left side is fixed
      b = m_b;
    } else if (m_db == 0 && m_dt != 0) {
      //  right side is fixed
      t = m_t;
    } else if (m_db != 0 && m_db == m_dt) {
      //  center is fixed
      b = (m_b + m_t) / 2 - org_box.height () / 2;
      t = (m_b + m_t) / 2 + org_box.height () / 2;
    } else if (m_db != 0 && m_db == -m_dt) {
      //  height is fixed
      b = org_box.center ().y () - (m_t - m_b) / 2;
      t = org_box.center ().y () + (m_t - m_b) / 2;
    } else if (m_db != 0 && m_dt != 0) {
      //  both sides have changed
      b = m_b;
      t = m_t;
    }

    new_box = geo::Box (l, b, r, t);

  }

  if (new_box != org_box) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_box);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PolygonChangeApplicator implementation

PolygonChangeApplicator::PolygonChangeApplicator (const geo::Polygon &poly, const geo::Polygon &org_poly)
  : m_poly (poly), m_org_poly (org_poly)
{
  //  .. nothing yet ..
}

geo::Shape PolygonChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool relative) const
{
  geo::Polygon org_poly;
  shape.polygon (org_poly);

  if (relative) {

    geo::Polygon new_poly = m_poly.moved (org_poly.box ().p1 () - m_poly.box ().p1 ());

    if (new_poly != org_poly) {
      //  shape changed - replace the old by the new one
      return shapes.replace (shape, new_poly);
    } else {
      //  shape did not change
      return shape;
    }

  } else if (m_poly != org_poly) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, m_poly);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextOrientationChangeApplicator implementation

TextOrientationChangeApplicator::TextOrientationChangeApplicator (const geo::FTrans &trans)
  : ChangeApplicator (), m_trans (trans)
{
  //  .. nothing yet ..
}

geo::Shape TextOrientationChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  new_text.trans (geo::Trans (org_text.trans ().disp ()) * geo::Trans (m_trans));

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextPositionChangeApplicator implementation

TextPositionChangeApplicator::TextPositionChangeApplicator (const geo::Vector &disp, const geo::Vector &org_disp)
  : ChangeApplicator (), m_disp (disp), m_org_disp (org_disp)
{
  //  .. nothing yet ..
}

geo::Shape TextPositionChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool relative) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  if (relative) {
    new_text.trans (geo::Trans (m_disp - m_org_disp) * org_text.trans ());
  } else {
    geo::Vector np = org_text.trans ().disp ();
    if (m_disp.x () != m_org_disp.x ()) {
      np.set_x (m_disp.x ());
    }
    if (m_disp.y () != m_org_disp.y ()) {
      np.set_y (m_disp.y ());
    }
    new_text.trans (geo::Trans (np - org_text.trans ().disp ()) * org_text.trans ());
  }

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextHAlignChangeApplicator implementation

TextHAlignChangeApplicator::TextHAlignChangeApplicator (geo::HAlign halign)
  : ChangeApplicator (), m_halign (halign)
{
  //  .. nothing yet ..
}

geo::Shape TextHAlignChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  new_text.halign (m_halign);

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextVAlignChangeApplicator implementation

TextVAlignChangeApplicator::TextVAlignChangeApplicator (geo::VAlign valign)
  : ChangeApplicator (), m_valign (valign)
{
  //  .. nothing yet ..
}

geo::Shape TextVAlignChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  new_text.valign (m_valign);

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextSizeChangeApplicator implementation

TextSizeChangeApplicator::TextSizeChangeApplicator (geo::Coord size)
  : ChangeApplicator (), m_size (size)
{
  //  .. nothing yet ..
}

geo::Shape TextSizeChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  new_text.size (m_size);

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  TextStringChangeApplicatorimplementation

TextStringChangeApplicator::TextStringChangeApplicator (const std::string &string)
  : ChangeApplicator (), m_string (string)
{
  //  .. nothing yet ..
}

geo::Shape TextStringChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Text org_text;
  shape.text (org_text);

  geo::Text new_text = org_text;
  new_text.string (m_string);

  if (new_text != org_text) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_text);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PathPointsChangeApplicator implementation

PathPointsChangeApplicator::PathPointsChangeApplicator (const std::vector<geo::Point> &points, const std::vector<geo::Point> &org_points)
  : ChangeApplicator (), m_points (points), m_org_points (org_points)
{
  //  .. nothing yet ..
}

geo::Shape PathPointsChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool relative) const
{
  geo::Path org_path;
  shape.path (org_path);

  geo::Path new_path = org_path;
  if (relative && ! m_org_points.empty () && new_path.begin () != new_path.end ()) {
    std::vector<geo::Point> new_points = m_points;
    for (std::vector<geo::Point>::iterator p = new_points.begin (); p != new_points.end (); ++p) {
      *p += *new_path.begin () - m_org_points.front ();
    }
    new_path.assign (new_points.begin (), new_points.end ());
  } else {
    new_path.assign (m_points.begin (), m_points.end ());
  }

  if (new_path != org_path) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_path);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PathWidthChangeApplicator implementation

PathWidthChangeApplicator::PathWidthChangeApplicator (geo::Coord w, geo::Coord org_w)
  : ChangeApplicator (), m_width (w), m_org_width (org_w)
{
  //  .. nothing yet ..
}

geo::Shape PathWidthChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool relative) const
{
  geo::Path org_path;
  shape.path (org_path);

  geo::Path new_path = org_path;
  if (relative) {
    new_path.width (new_path.width () + m_width - m_org_width);
  } else {
    new_path.width (m_width);
  }

  //  Adjust extensions if equal to half width if that was the case before
  if (org_path.bgn_ext () == org_path.width () / 2) {
    new_path.bgn_ext (new_path.width () / 2);
  }
  if (org_path.end_ext () == org_path.width () / 2) {
    new_path.end_ext (new_path.width () / 2);
  }

  if (new_path != org_path) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_path);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PathStartExtensionChangeApplicator implementation

PathStartExtensionChangeApplicator::PathStartExtensionChangeApplicator (geo::Coord e)
  : ChangeApplicator (), m_ext (e)
{
  //  .. nothing yet ..
}

geo::Shape PathStartExtensionChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Path org_path;
  shape.path (org_path);

  geo::Path new_path = org_path;
  if (m_ext == std::numeric_limits <geo::Coord>::min ()) {
    new_path.bgn_ext (new_path.width () / 2);
  } else {
    new_path.bgn_ext (m_ext);
  }

  if (new_path != org_path) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_path);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PathEndExtensionChangeApplicator implementation

PathEndExtensionChangeApplicator::PathEndExtensionChangeApplicator (geo::Coord e)
  : ChangeApplicator (), m_ext (e)
{
  //  .. nothing yet ..
}

geo::Shape PathEndExtensionChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Path org_path;
  shape.path (org_path);

  geo::Path new_path = org_path;
  if (m_ext == std::numeric_limits <geo::Coord>::min ()) {
    new_path.end_ext (new_path.width () / 2);
  } else {
    new_path.end_ext (m_ext);
  }

  if (new_path != org_path) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_path);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  PathRoundEndChangeApplicator implementation

PathRoundEndChangeApplicator::PathRoundEndChangeApplicator (bool r)
  : ChangeApplicator (), m_round (r)
{
  //  .. nothing yet ..
}

geo::Shape PathRoundEndChangeApplicator::do_apply (geo::Shapes &shapes, const geo::Shape &shape, double /*dbu*/, bool /*relative*/) const
{
  geo::Path org_path;
  shape.path (org_path);

  geo::Path new_path = org_path;
  new_path.round (m_round);

  if (new_path != org_path) {
    //  shape changed - replace the old by the new one
    return shapes.replace (shape, new_path);
  } else {
    //  shape did not change
    return shape;
  }
}

// -------------------------------------------------------------------------
//  ChangeTargetCellApplicator implementation

ChangeTargetCellApplicator::ChangeTargetCellApplicator (geo::cell_index_type cell_index)
  : m_cell_index (cell_index)
{
  //  .. nothing yet ..
}

geo::Instance ChangeTargetCellApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double /*dbu*/, bool /*relative*/) const 
{
  tl_assert (cell.layout ());

  //  detect recursions in the hierarchy
  std::set<geo::cell_index_type> called;
  cell.layout ()->cell (m_cell_index).collect_called_cells (called);
  if (m_cell_index == cell.cell_index () || called.find (cell.cell_index ()) != called.end ()) {
    throw tl::Exception (geo::to_string (QObject::tr ("Trying to build a recursive hierarchy")).c_str ());
  }

  geo::CellInstArray arr = instance.cell_inst ();
  arr.object ().cell_index (m_cell_index);
  return cell.replace (instance, arr);
}

// -------------------------------------------------------------------------
//  ChangeInstanceTransApplicator implementation

ChangeInstanceTransApplicator::ChangeInstanceTransApplicator (double a, double org_a, bool mirror, bool org_mirror, double m, double org_m, const geo::DVector &disp, const geo::DVector &org_disp)
  : m_angle (a), m_org_angle (org_a), m_mirror (mirror), m_org_mirror (org_mirror),
    m_mag (m), m_org_mag (org_m), m_disp (disp), m_org_disp (org_disp)
{
  //  .. nothing yet ..
}

geo::Instance ChangeInstanceTransApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const 
{
  geo::CellInstArray::complex_trans_type tr = instance.complex_trans ();

  if (relative) {

    tr = geo::CellInstArray::complex_trans_type (m_mag, m_angle, m_mirror, geo::Vector (m_disp * (1.0 / dbu))) * geo::CellInstArray::complex_trans_type (m_org_mag, m_org_angle, m_org_mirror, geo::Vector (m_org_disp * (1.0 / dbu))).inverted () * tr;

  } else {

    geo::Vector disp = tr.disp ();

    if (fabs (m_disp.x () - m_org_disp.x ()) > 1e-6) {
      disp.set_x (geo::coord_traits<geo::Coord>::rounded (m_disp.x () / dbu));
    }
    if (fabs (m_disp.y () - m_org_disp.y ()) > 1e-6) {
      disp.set_y (geo::coord_traits<geo::Coord>::rounded (m_disp.y () / dbu));
    }

    double mag = fabs (m_mag - m_org_mag) > 1e-6 ? m_mag : tr.mag ();
    double angle = fabs (m_angle - m_org_angle) > 1e-6 ? m_angle : tr.angle ();
    bool mirror = (m_mirror != m_org_mirror) ? m_mirror : tr.is_mirror ();

    tr = geo::CellInstArray::complex_trans_type (mag, angle, mirror, disp);

  }

  bool is_complex = (tr.is_mag () || ! tr.is_ortho ());

  geo::CellInstArray new_inst;

  geo::CellInstArray::vector_type a, b;
  unsigned long na = 0, nb = 0;

  if (instance.is_regular_array (a, b, na, nb)) {

    if (is_complex) {
      new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), tr, a, b, na, nb);
    } else {
      new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), geo::Trans (tr.rot (), tr.disp ()), a, b, na, nb);
    }

  } else {

    if (is_complex) {
      new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), tr);
    } else {
      new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), geo::Trans (tr.rot (), tr.disp ()));
    }

  }

  if (new_inst != instance.cell_inst ()) {
    return cell.replace (instance, new_inst);
  } else {
    return instance;
  }
}

// -------------------------------------------------------------------------
//  ChangeInstanceArrayApplicator implementation

ChangeInstanceArrayApplicator::ChangeInstanceArrayApplicator (const geo::DVector &a, bool set_a, const geo::DVector &b, bool set_b, unsigned long na, bool set_na, unsigned long nb, bool set_nb)
  : m_a (a), m_set_a (set_a), m_b (b), m_set_b (set_b),
    m_na (na), m_set_na (set_na), m_nb (nb), m_set_nb (set_nb)
{
  //  .. nothing yet ..
}

geo::Instance ChangeInstanceArrayApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool /*relative*/) const 
{
  geo::CellInstArray new_inst;

  geo::CellInstArray::vector_type a, b;
  unsigned long na = 0, nb = 0;
  if (! instance.is_regular_array (a, b, na, nb)) {
    na = nb = 1;
    a = geo::Vector (m_a * (1.0 / dbu));
    b = geo::Vector (m_b * (1.0 / dbu));
  }

  if (m_set_a) {
    a = geo::Vector (m_a * (1.0 / dbu));
  }
  if (m_set_na) {
    na = m_na;
  }
  if (m_set_b) {
    b = geo::Vector (m_b * (1.0 / dbu));
  }
  if (m_set_nb) {
    nb = m_nb;
  }

  if (instance.is_complex ()) {
    new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), instance.complex_trans (), a, b, na, nb);
  } else {
    new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), instance.front (), a, b, na, nb);
  }

  if (new_inst != instance.cell_inst ()) {
    return cell.replace (instance, new_inst);
  } else {
    return instance;
  }
}

// -------------------------------------------------------------------------
//  InstanceRemoveArrayApplicator implementation

InstanceRemoveArrayApplicator::InstanceRemoveArrayApplicator ()
{
  //  .. nothing yet ..
}

geo::Instance InstanceRemoveArrayApplicator::do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double /*dbu*/, bool /*relative*/) const 
{
  geo::CellInstArray new_inst;
  if (instance.is_complex ()) {
    new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), instance.complex_trans ());
  } else {
    new_inst = geo::CellInstArray (geo::CellInst (instance.cell_index ()), instance.front ());
  }

  if (new_inst != instance.cell_inst ()) {
    return cell.replace (instance, new_inst);
  } else {
    return instance;
  }
}

// -------------------------------------------------------------------------
//  helper functions to convert coordinates

std::string
coord_to_string (double dc, double dbu, bool du)
{
  if (du) {
    return geo::db_to_string (dc);
  } else {
    return geo::micron_to_string (dc * dbu);
  }
}

geo::DCoord
dcoord_from_dcoord (double d, double dbu, bool du, const geo::DCplxTrans &t)
{
  geo::DCoord dc = t.ctrans (d * (du ? 1.0 : 1.0 / dbu));
  return geo::Coord (floor (dc + 0.5)) * dbu;
}

geo::Coord
coord_from_dcoord (double d, double dbu, bool du, const geo::VCplxTrans &t)
{
  return t.ctrans (d * (du ? 1.0 : 1.0 / dbu));
}

geo::DPoint
dpoint_from_dpoint (const geo::DPoint &dp, double dbu, bool du, const geo::DCplxTrans &t)
{
  return (t * (dp * (du ? 1.0 : 1.0 / dbu))) * dbu;
}

geo::DVector
dvector_from_dvector (const geo::DVector &dp, double dbu, bool du, const geo::DCplxTrans &t)
{
  return (t * (dp * (du ? 1.0 : 1.0 / dbu))) * dbu;
}

geo::Point
point_from_dpoint (const geo::DPoint &dp, double dbu, bool du, const geo::VCplxTrans &t)
{
  return t * (dp * (du ? 1.0 : 1.0 / dbu));
}

geo::DCoord
dcoord_from_string (const char *txt, double dbu, bool du, const geo::DCplxTrans &t)
{
  double d = 0.0;
  geo::from_string (txt, d);
  return dcoord_from_dcoord (d, dbu, du, t);
}

geo::Coord
coord_from_string (const char *txt, double dbu, bool du, const geo::VCplxTrans &t)
{
  double d = 0.0;
  geo::from_string (txt, d);
  return coord_from_dcoord (d, dbu, du, t);
}

std::string
coords_to_string (const geo::DPoint &dp, double dbu, bool du, const char *sep)
{
  return coord_to_string (dp.x (), dbu, du) + sep + coord_to_string (dp.y (), dbu, du);
}

}

