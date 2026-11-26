#include "layEdtConfig.h"
#include "tlException.h"
#include "detail/utilsString.h"
#include "tlString.h"
#include "tlInternational.h"

namespace edt
{

// -----------------------------------------------------------------------------

std::string cfg_edit_grid ("edit-grid");
std::string cfg_edit_snap_to_objects ("edit-snap-to-objects");
std::string cfg_edit_move_angle_mode ("edit-move-angle-mode");
std::string cfg_edit_connect_angle_mode ("edit-connect-angle-mode");
std::string cfg_edit_text_string ("edit-text-string");
std::string cfg_edit_text_size ("edit-text-size");
std::string cfg_edit_text_halign ("edit-text-halign");
std::string cfg_edit_text_valign ("edit-text-valign");
std::string cfg_edit_path_width ("edit-path-width");
std::string cfg_edit_path_ext_type ("edit-path-ext-type");
std::string cfg_edit_path_ext_var_begin ("edit-path-ext-var-begin");
std::string cfg_edit_path_ext_var_end ("edit-path-ext-var-end");
std::string cfg_edit_inst_cell_name ("edit-inst-cell-name");
std::string cfg_edit_inst_lib_name ("edit-inst-lib-name");
std::string cfg_edit_inst_pcell_parameters ("edit-inst-pcell-parameters");
std::string cfg_edit_inst_angle ("edit-inst-angle");
std::string cfg_edit_inst_mirror ("edit-inst-mirror");
std::string cfg_edit_inst_scale ("edit-inst-scale");
std::string cfg_edit_inst_array ("edit-inst-array");
std::string cfg_edit_inst_rows ("edit-inst-rows");
std::string cfg_edit_inst_row_x ("edit-inst-row_x");
std::string cfg_edit_inst_row_y ("edit-inst-row_y");
std::string cfg_edit_inst_columns ("edit-inst-columns");
std::string cfg_edit_inst_column_x ("edit-inst-column_x");
std::string cfg_edit_inst_column_y ("edit-inst-column_y");
std::string cfg_edit_inst_place_origin ("edit-inst-place-origin");
std::string cfg_edit_top_level_selection ("edit-top-level-selection");
std::string cfg_edit_hier_copy_mode ("edit-hier-copy-mode");
std::string cfg_edit_show_shapes_of_instances ("edit-show-shapes-of-instances");
std::string cfg_edit_max_shapes_of_instances ("edit-max-shapes-of-instances");
std::string cfg_edit_global_grid ("grid-micron");
std::string cfg_edit_combine_mode ("combine-mode");

// -----------------------------------------------------------------------------
//  CMConverter implementation

std::string 
CMConverter::to_string (const edt::combine_mode_type &m)
{
  if (m == edt::CM_Add) {
    return "add";
  } else if (m == edt::CM_Merge) {
    return "merge";
  } else if (m == edt::CM_Erase) {
    return "erase";
  } else if (m == edt::CM_Mask) {
    return "mask";
  } else if (m == edt::CM_Diff) {
    return "diff";
  } else {
    return "";
  }
}

void 
CMConverter::from_string (const std::string &s, edt::combine_mode_type &m)
{
  std::string t (tl::trim (s));
  if (t == "add") {
    m = edt::CM_Add;
  } else if (t == "merge") {
    m = edt::CM_Merge;
  } else if (t == "erase") {
    m = edt::CM_Erase;
  } else if (t == "mask") {
    m = edt::CM_Mask;
  } else if (t == "diff") {
    m = edt::CM_Diff;
  } else {
    m = edt::CM_Add;
  }
}

// -----------------------------------------------------------------------------
//  ACConverter implementation

std::string 
ACConverter::to_string (const lay::angle_constraint_type &m)
{
  if (m == lay::AC_Any) {
    return "any";
  } else if (m == lay::AC_Diagonal) {
    return "diagonal";
  } else if (m == lay::AC_Ortho) {
    return "ortho";
  } else {
    return "";
  }
}

void 
ACConverter::from_string (const std::string &tt, lay::angle_constraint_type &m)
{
  std::string t (tl::trim (tt));
  if (t == "any") {
    m = lay::AC_Any;
  } else if (t == "diagonal") {
    m = lay::AC_Diagonal;
  } else if (t == "ortho") {
    m = lay::AC_Ortho;
  } else {
    m = lay::AC_Any;
  }
}

// -----------------------------------------------------------------------------
//  PathExtConverter implementation

std::string 
PathExtConverter::to_string (const edt::path_ext_type &m)
{
  if (m == edt::Flush) {
    return "flush";
  } else if (m == edt::Square) {
    return "square";
  } else if (m == edt::Variable) {
    return "variable";
  } else if (m == edt::Round) {
    return "round";
  } else {
    return "";
  }
}

void 
PathExtConverter::from_string (const std::string &tt, edt::path_ext_type &m)
{
  std::string t (tl::trim (tt));
  if (t == "flush") {
    m = edt::Flush;
  } else if (t == "square") {
    m = edt::Square;
  } else if (t == "variable") {
    m = edt::Variable;
  } else if (t == "round") {
    m = edt::Round;
  } else {
    m = edt::Flush;
  }
}

// -----------------------------------------------------------------------------
//  HAlignConverter implementation

std::string 
HAlignConverter::to_string (geo::HAlign a)
{
  if (a == geo::HAlignCenter) {
    return "center";
  } else if (a == geo::HAlignLeft) {
    return "left";
  } else if (a == geo::HAlignRight) {
    return "right";
  } else {
    return "";
  }
}

void 
HAlignConverter::from_string (const std::string &tt, geo::HAlign &a)
{
  std::string t (tl::trim (tt));
  if (t == "center") {
    a = geo::HAlignCenter;
  } else if (t == "left") {
    a = geo::HAlignLeft;
  } else if (t == "right") {
    a = geo::HAlignRight;
  } else {
    a = geo::NoHAlign;
  }
}

// -----------------------------------------------------------------------------
//  VAlignConverter implementation

std::string 
VAlignConverter::to_string (geo::VAlign a)
{
  if (a == geo::VAlignCenter) {
    return "center";
  } else if (a == geo::VAlignBottom) {
    return "bottom";
  } else if (a == geo::VAlignTop) {
    return "top";
  } else {
    return "";
  }
}

void 
VAlignConverter::from_string (const std::string &tt, geo::VAlign &a)
{
  std::string t (tl::trim (tt));
  if (t == "center") {
    a = geo::VAlignCenter;
  } else if (t == "bottom") {
    a = geo::VAlignBottom;
  } else if (t == "top") {
    a = geo::VAlignTop;
  } else {
    a = geo::NoVAlign;
  }
}

// -----------------------------------------------------------------------------
//  EditGridConverter implementation

std::string 
EditGridConverter::to_string (const geo::DVector &eg)
{
  if (eg == geo::DVector ()) {
    return "global";
  } else if (eg.x () < 1e-6) {
    return "none";
  } else if (fabs (eg.x () - eg.y ()) < 1e-6) {
    return geo::to_string (eg.x ());
  } else {
    return geo::to_string (eg.x ()) + "," + geo::to_string (eg.y ());
  }
}

void 
EditGridConverter::from_string (const std::string &s, geo::DVector &eg)
{
  geo::Extractor ex (s.c_str ());

  double x = 0, y = 0;
  if (ex.test ("global")) {
    eg = geo::DVector ();
  } else if (ex.test ("none")) {
    eg = geo::DVector (-1.0, -1.0);
  } else if (ex.try_read (x)) {
    y = x;
    if (ex.test (",")) {
      ex.try_read (y);
    }
    eg = geo::DVector (x, y);
  }
}

void 
EditGridConverter::from_string_picky (const std::string &s, geo::DVector &eg)
{
  geo::Extractor ex (s.c_str ());

  if (ex.test ("global")) {
    eg = geo::DVector ();
  } else if (ex.test ("none")) {
    eg = geo::DVector (-1.0, -1.0);
  } else {
    double x = 0.0, y = 0.0;
    ex.read (x);
    if (ex.test (",")) {
      ex.read (y);
    } else {
      y = x;
    }
    if (x < 1e-6 || y < 1e-6) {
      throw tl::Exception (geo::to_string (QObject::tr ("The grid must be larger than zero")));
    }
    eg = geo::DVector (x, y);  
  }
  ex.expect_end ();
}

}

