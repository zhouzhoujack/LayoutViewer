#include "layRulerConfig.h"

namespace lay
{

// ------------------------------------------------------------
//  Helper functions to get and set the configuration

std::string 
ACConverter::to_string (const lay::angle_constraint_type &m)
{
  if (m == lay::AC_Any) {
    return "any";
  } else if (m == lay::AC_Diagonal) {
    return "diagonal";
  } else if (m == lay::AC_Ortho) {
    return "ortho";
  } else if (m == lay::AC_Horizontal) {
    return "horizontal";
  } else if (m == lay::AC_Vertical) {
    return "vertical";
  } else if (m == lay::AC_Global) {
    return "global";
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
  } else if (t == "horizontal") {
    m = lay::AC_Horizontal;
  } else if (t == "vertical") {
    m = lay::AC_Vertical;
  } else if (t == "global") {
    m = lay::AC_Global;
  } else {
    m = lay::AC_Any;
  }
}

std::string 
StyleConverter::to_string (RulerObject::style_type s)
{
  if (s == RulerObject::STY_ruler) {
    return "ruler";
  } else if (s == RulerObject::STY_arrow_end) {
    return "arrow_end";
  } else if (s == RulerObject::STY_arrow_start) {
    return "arrow_start";
  } else if (s == RulerObject::STY_arrow_both) {
    return "arrow_both";
  } else if (s == RulerObject::STY_cross_start) {
    return "cross_start";
  } else if (s == RulerObject::STY_cross_end) {
    return "cross_end";
  } else if (s == RulerObject::STY_cross_both) {
    return "cross_both";
  } else if (s == RulerObject::STY_line) {
    return "line";
  } else {
    return "";
  }
}

void  
StyleConverter::from_string (const std::string &tt, RulerObject::style_type &s)
{
  std::string t (tl::trim (tt));
  if (t == "ruler") {
    s = RulerObject::STY_ruler;
  } else if (t == "arrow_end") {
    s = RulerObject::STY_arrow_end;
  } else if (t == "arrow_start") {
    s = RulerObject::STY_arrow_start;
  } else if (t == "arrow_both") {
    s = RulerObject::STY_arrow_both;
  } else if (t == "cross_start") {
    s = RulerObject::STY_cross_start;
  } else if (t == "cross_end") {
    s = RulerObject::STY_cross_end;
  } else if (t == "cross_both") {
    s = RulerObject::STY_cross_both;
  } else if (t == "line") {
    s = RulerObject::STY_line;
  } else {
    s = RulerObject::STY_ruler;
  }
}

std::string  
OutlineConverter::to_string (RulerObject::outline_type o)
{
  if (o == RulerObject::OL_diag) {
    return "diag";
  } else if (o == RulerObject::OL_xy) {
    return "xy";
  } else if (o == RulerObject::OL_diag_xy) {
    return "diag_xy";
  } else if (o == RulerObject::OL_yx) {
    return "yx";
  } else if (o == RulerObject::OL_diag_yx) {
    return "diag_yx";
  } else if (o == RulerObject::OL_box) {
    return "box";
  } else if (o == RulerObject::OL_ellipse) {
    return "ellipse";
  } else {
    return "";
  }
}

void   
OutlineConverter::from_string (const std::string &s, RulerObject::outline_type &o)
{
  std::string t (tl::trim (s));
  if (t == "diag") {
    o = RulerObject::OL_diag;
  } else if (t == "xy") {
    o = RulerObject::OL_xy;
  } else if (t == "diag_xy") {
    o = RulerObject::OL_diag_xy;
  } else if (t == "yx") {
    o = RulerObject::OL_yx;
  } else if (t == "diag_yx") {
    o = RulerObject::OL_diag_yx;
  } else if (t == "box") {
    o = RulerObject::OL_box;
  } else if (t == "ellipse") {
    o = RulerObject::OL_ellipse;
  } else {
    o = RulerObject::OL_diag;
  }
}

std::string
PositionConverter::to_string (RulerObject::position_type p)
{
  if (p == RulerObject::POS_auto) {
    return "auto";
  } else if (p == RulerObject::POS_p1) {
    return "p1";
  } else if (p == RulerObject::POS_p2) {
    return "p2";
  } else if (p == RulerObject::POS_center) {
    return "center";
  } else {
    return "";
  }
}

void
PositionConverter::from_string (const std::string &s, RulerObject::position_type &p)
{
  std::string t (tl::trim (s));
  if (t == "auto") {
    p = RulerObject::POS_auto;
  } else if (t == "p1") {
    p = RulerObject::POS_p1;
  } else if (t == "p2") {
    p = RulerObject::POS_p2;
  } else if (t == "center") {
    p = RulerObject::POS_center;
  } else {
    p = RulerObject::POS_auto;
  }
}

std::string
AlignmentConverter::to_string (RulerObject::alignment_type a)
{
  if (a == RulerObject::AL_auto) {
    return "auto";
  } else if (a == RulerObject::AL_center) {
    return "center";
  } else if (a == RulerObject::AL_down) {
    return "down";
  } else if (a == RulerObject::AL_up) {
    return "up";
  } else {
    return "";
  }
}

void
AlignmentConverter::from_string (const std::string &s, RulerObject::alignment_type &a)
{
  std::string t (tl::trim (s));
  if (t == "auto") {
    a = RulerObject::AL_auto;
  } else if (t == "center") {
    a = RulerObject::AL_center;
  } else if (t == "down") {
    a = RulerObject::AL_down;
  } else if (t == "up") {
    a = RulerObject::AL_up;
  } else {
    a = RulerObject::AL_auto;
  }
}

std::string
RulerModeConverter::to_string (Template::ruler_mode_type m)
{
  if (m == Template::RulerNormal) {
    return "normal";
  } else if (m == Template::RulerSingleClick) {
    return "single_click";
  } else if (m == Template::RulerAutoMetric) {
    return "auto_metric";
  } else {
    return "normal";
  }
}

void
RulerModeConverter::from_string (const std::string &s, Template::ruler_mode_type &a)
{
  std::string t (tl::trim (s));
  if (t == "normal") {
    a = Template::RulerNormal;
  } else if (t == "single_click") {
    a = Template::RulerSingleClick;
  } else if (t == "auto_metric") {
    a = Template::RulerAutoMetric;
  } else {
    a = Template::RulerNormal;
  }
}

std::string
TemplatesConverter::to_string (const std::vector <Template> &t)
{
  return Template::to_string (t);
}

void 
TemplatesConverter::from_string (const std::string &s, std::vector <Template> &t)
{
  t = Template::from_string (s);
}


} // namespace ant

