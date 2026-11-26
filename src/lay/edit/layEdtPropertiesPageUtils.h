#ifndef HDR_edtPropertiesPageUtils
#define HDR_edtPropertiesPageUtils

#include "detail/geoPath.h"
#include "geoPolygon.h"
#include "geoBox.h"
#include "detail/geoText.h"
#include "detail/geoShape.h"
#include "geoCell.h"

#include <vector>

class QLineEdit;

namespace edt
{

// -------------------------------------------------------------------------
//  ChangeApplicator definition and implementation

/**
 *  @brief A change applicator strategy pattern implementation
 *  The change applicator is an object describing individual changes
 *  applied to shapes.
 */
class ChangeApplicator
{
public:
  ChangeApplicator () { }
  virtual ~ChangeApplicator () { }

  virtual bool supports_relative_mode () const
  {
    return false;
  }

  virtual geo::Shape do_apply (geo::Shapes & /*shapes*/, const geo::Shape & /*shape*/, double /*dbu*/, bool /*relative*/) const 
  { 
    return geo::Shape (); 
  }

  virtual geo::Instance do_apply_inst (geo::Cell & /*cell*/, const geo::Instance & /*instance*/, double /*dbu*/, bool /*relative*/) const 
  { 
    return geo::Instance ();
  }
};

/**
 *  @brief A combined applicator 
 *  This class combines the actions of multiple applicators into a single one.
 *  The combined applicator takes ownership over the given individual applicators.
 */
class CombinedChangeApplicator
  : public ChangeApplicator
{
public:
  CombinedChangeApplicator ();
  CombinedChangeApplicator (ChangeApplicator *a1);
  CombinedChangeApplicator (ChangeApplicator *a1, ChangeApplicator *a2);
  ~CombinedChangeApplicator ();
      
  void add (ChangeApplicator *a);

  bool supports_relative_mode () const;
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;

private:
  std::vector<ChangeApplicator *> m_appl;
};

/**
 *  @brief A property ID change applicator
 */
class ChangePropertiesApplicator
  : public ChangeApplicator
{
public:
  ChangePropertiesApplicator (geo::properties_id_type prop_id);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;

private:
  geo::properties_id_type m_prop_id;
};

/**
 *  @brief A box change applicator
 */
class BoxDimensionsChangeApplicator
  : public ChangeApplicator
{
public:
  BoxDimensionsChangeApplicator (geo::Coord dl, geo::Coord db, geo::Coord dr, geo::Coord dt, geo::Coord l, geo::Coord b, geo::Coord r, geo::Coord t);

  bool supports_relative_mode () const { return true; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Coord m_dl, m_db, m_dr, m_dt;
  geo::Coord m_l, m_b, m_r, m_t;
};

/**
 *  @brief A polygon change applicator
 */
class PolygonChangeApplicator
  : public ChangeApplicator
{
public:
  PolygonChangeApplicator (const geo::Polygon &poly, const geo::Polygon &org_poly);

  bool supports_relative_mode () const { return true; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Polygon m_poly, m_org_poly;
};

/**
 *  @brief An applicator changing the orientation of a text
 */
class TextOrientationChangeApplicator
  : public ChangeApplicator
{
public:
  TextOrientationChangeApplicator (const geo::FTrans &trans);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::FTrans m_trans;
};

/**
 *  @brief An applicator changing the position of a text
 */
class TextPositionChangeApplicator
  : public ChangeApplicator
{
public:
  TextPositionChangeApplicator (const geo::Vector &disp, const geo::Vector &org_disp);

  bool supports_relative_mode () const { return true; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Vector m_disp, m_org_disp;
};

/**
 *  @brief An applicator changing the horizontal alignment of a text
 */
class TextHAlignChangeApplicator
  : public ChangeApplicator
{
public:
  TextHAlignChangeApplicator (geo::HAlign halign);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::HAlign m_halign;
};

/**
 *  @brief An applicator changing the vertical alignment of a text
 */
class TextVAlignChangeApplicator
  : public ChangeApplicator
{
public:
  TextVAlignChangeApplicator (geo::VAlign valign);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::VAlign m_valign;
};

/**
 *  @brief An applicator changing the size alignment of a text
 */
class TextSizeChangeApplicator
  : public ChangeApplicator
{
public:
  TextSizeChangeApplicator (geo::Coord size);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Coord m_size;
};

/**
 *  @brief An applicator changing the string of a text
 */
class TextStringChangeApplicator
  : public ChangeApplicator
{
public:
  TextStringChangeApplicator (const std::string &string);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  std::string m_string;
};

/**
 *  @brief An applicator changing the points of a path
 */
class PathPointsChangeApplicator
  : public ChangeApplicator
{
public:
  PathPointsChangeApplicator (const std::vector<geo::Point> &points, const std::vector<geo::Point> &org_points);

  bool supports_relative_mode () const { return true; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  std::vector<geo::Point> m_points, m_org_points;
};

/**
 *  @brief An applicator changing the width of a path
 */
class PathWidthChangeApplicator
  : public ChangeApplicator
{
public:
  PathWidthChangeApplicator (geo::Coord w, geo::Coord org_w);

  bool supports_relative_mode () const { return true; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Coord m_width, m_org_width;
};

/**
 *  @brief An applicator changing the start extensions of a path
 */
class PathStartExtensionChangeApplicator
  : public ChangeApplicator
{
public:
  PathStartExtensionChangeApplicator (geo::Coord e);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Coord m_ext;
};

/**
 *  @brief An applicator changing the end extensions of a path
 */
class PathEndExtensionChangeApplicator
  : public ChangeApplicator
{
public:
  PathEndExtensionChangeApplicator (geo::Coord e);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  geo::Coord m_ext;
};

/**
 *  @brief An applicator changing the round end flag of a path
 */
class PathRoundEndChangeApplicator
  : public ChangeApplicator
{
public:
  PathRoundEndChangeApplicator (bool r);

  bool supports_relative_mode () const { return false; }
  geo::Shape do_apply (geo::Shapes &shapes, const geo::Shape &shape, double dbu, bool relative) const;

private:
  bool m_round;
};

/**
 *  @brief An applicator changing the target cell of an instance
 */
class ChangeTargetCellApplicator
  : public ChangeApplicator
{
public:
  ChangeTargetCellApplicator (geo::cell_index_type cell_index);

  bool supports_relative_mode () const { return false; }
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;

private:
  geo::cell_index_type m_cell_index;
};

/**
 *  @brief An applicator changing the transformation properties of an instance
 */
class ChangeInstanceTransApplicator
  : public ChangeApplicator
{
public:
  ChangeInstanceTransApplicator (double a, double org_a, bool mirror, bool org_mirror, double m, double org_m, const geo::DVector &disp, const geo::DVector &org_disp);

  bool supports_relative_mode () const { return true; }
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;

private:
  double m_angle, m_org_angle;
  bool m_mirror, m_org_mirror;
  double m_mag, m_org_mag;
  geo::DVector m_disp, m_org_disp;
};

/**
 *  @brief An applicator changing the array properties of an instance
 */
class ChangeInstanceArrayApplicator
  : public ChangeApplicator
{
public:
  ChangeInstanceArrayApplicator (const geo::DVector &a, bool set_a, const geo::DVector &b, bool set_b, unsigned long na, bool set_na, unsigned long nb, bool set_nb);

  bool supports_relative_mode () const { return false; }
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;

private:
  geo::DVector m_a;
  bool m_set_a;
  geo::DVector m_b;
  bool m_set_b;
  unsigned long m_na;
  bool m_set_na;
  unsigned long m_nb;
  bool m_set_nb;
};

/**
 *  @brief An applicator removing the array properties of an instance
 */
class InstanceRemoveArrayApplicator
  : public ChangeApplicator
{
public:
  InstanceRemoveArrayApplicator ();

  bool supports_relative_mode () const { return false; }
  geo::Instance do_apply_inst (geo::Cell &cell, const geo::Instance &instance, double dbu, bool relative) const;
};

// -------------------------------------------------------------------------
//  helper functions to convert coordinates to and from a string

/**
 *  @brief Converts a DBU coordinate to a string
 *
 *  @param dc The coordinate in DBU units
 *  @param dbu The database unit
 *  @param du A flag indicating whether the value shall be given in database units (du = true) or micron (du = false)
 */
std::string coord_to_string (double dc, double dbu, bool du);

/**
 *  @brief Converts a DBU point to a string
 *
 *  @param dp The point in DBU units
 *  @param dbu The database unit
 *  @param du A flag indicating whether the value shall be given in database units (du = true) or micron (du = false)
 */
std::string coords_to_string (const geo::DPoint &dp, double dbu, bool du, const char *sep = "\t");

/**
 *  @brief Converts a micron or DBU value to a micron value
 *
 *  @param d The value to convert
 *  @param dbu The database unit
 *  @param du A flag indicating whether the value is given in database units (du = true) or micron (du = false)
 *  @param t A transformation (in DBU space) to apply to the value (which has to be a dimension)
 *  @return The micron-units dimension
 *
 *  The transformation is intended to be a global-to-local transformation so the output value is
 *  a dimension of a shape in local-cell micron units.
 */
geo::DCoord dcoord_from_dcoord (double d, double dbu, bool du, const geo::CplxTrans &t);

/**
 *  @brief Converts a micron or DBU point to a micron point
 *
 *  @param dp The point to convert
 *  @param dbu The database unit
 *  @param du A flag indicating whether the input point is given in database units (du = true) or micron (du = false)
 *  @param t A transformation (in DBU space) to apply to the point
 *  @return The micron-unit point
 *
 *  The transformation is intended to be a global-to-local transformation so the output value is
 *  a point in local-cell micron units.
 */
geo::DPoint dpoint_from_dpoint (const geo::DPoint &dp, double dbu, bool du, const geo::DCplxTrans &t);

/**
 *  @brief Converts a micron or DBU vector to a micron point
 *
 *  @param dp The point to convert
 *  @param dbu The database unit
 *  @param du A flag indicating whether the input point is given in database units (du = true) or micron (du = false)
 *  @param t A transformation (in DBU space) to apply to the point
 *  @return The micron-unit point
 *
 *  The transformation is intended to be a global-to-local transformation so the output value is
 *  a point in local-cell micron units.
 */
geo::DVector dvector_from_dvector (const geo::DVector &dp, double dbu, bool du, const geo::DCplxTrans &t);

/**
 *  @brief Gets a dimension value from a string
 *
 *  See dcoord_from_dcoord for a description of the arguments.
 */
geo::DCoord dcoord_from_string (const char *txt, double dbu, bool du, const geo::CplxTrans &t);

/**
 *  @brief Converts a micron or DBU value to a DBU value
 *
 *  @param d The value to convert
 *  @param dbu The database unit
 *  @param du A flag indicating whether the value is given in database units (du = true) or micron (du = false)
 *  @param t A transformation (in DBU space) to apply to the value (which has to be a dimension)
 *  @return The DBU units dimension
 *
 *  The transformation is intended to be a global-to-local transformation so the output value is
 *  a dimension of a shape in local-cell DBU units.
 */
geo::Coord coord_from_dcoord (double d, double dbu, bool du, const geo::CplxTrans &t);

/**
 *  @brief Converts a micron or DBU point to a DBU point
 *
 *  @param dp The point to convert
 *  @param dbu The database unit
 *  @param du A flag indicating whether the input point is given in database units (du = true) or micron (du = false)
 *  @param t A transformation (in DBU space) to apply to the point
 *  @return The DBU unit point
 *
 *  The transformation is intended to be a global-to-local transformation so the output value is
 *  a point in local-cell DBU units.
 */
geo::Point point_from_dpoint (const geo::DPoint &dp, double dbu, bool du, const geo::VCplxTrans &t);

/**
 *  @brief Gets a dimension value from a string
 *
 *  See coord_from_dcoord for a description of the arguments.
 */
geo::Coord coord_from_string (const char *txt, double dbu, bool du, const geo::VCplxTrans &t);

}

#endif

