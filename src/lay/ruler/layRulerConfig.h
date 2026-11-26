#ifndef HDR_antConfig
#define HDR_antConfig

#include <string>

#include "layRulerPlugin.h"
#include "laySnap.h"

namespace lay
{

/** 
 *  @brief Declaration of the configuration names
 */
constexpr const char* cfg_max_number_of_rulers   = "rulers";
constexpr const char* cfg_ruler_snap_range       = "ruler-snap-range";
constexpr const char* cfg_ruler_color            = "ruler-color";
constexpr const char* cfg_ruler_halo             = "ruler-halo";
constexpr const char* cfg_ruler_snap_mode        = "ruler-snap-mode";
constexpr const char* cfg_ruler_obj_snap         = "ruler-obj-snap";
constexpr const char* cfg_ruler_grid_snap        = "ruler-grid-snap";
constexpr const char* cfg_ruler_grid_micron      = "grid-micron";
constexpr const char* cfg_ruler_templates        = "ruler-templates";
constexpr const char* cfg_current_ruler_template = "current-ruler-template";

// ------------------------------------------------------------
//  Helper functions to get and set the configuration

struct ACConverter 
{
  std::string to_string (const lay::angle_constraint_type &m);
  void from_string (const std::string &s, lay::angle_constraint_type &m);
};

struct StyleConverter 
{
  std::string to_string (RulerObject::style_type s);
  void from_string (const std::string &s, RulerObject::style_type &style);
};

struct OutlineConverter 
{
  std::string to_string (RulerObject::outline_type s);
  void from_string (const std::string &s, RulerObject::outline_type &outline);
};

struct PositionConverter
{
  std::string to_string (RulerObject::position_type p);
  void from_string (const std::string &s, RulerObject::position_type &pos);
};

struct AlignmentConverter
{
  std::string to_string (RulerObject::alignment_type a);
  void from_string (const std::string &s, RulerObject::alignment_type &a);
};

struct RulerModeConverter
{
  std::string to_string (Template::ruler_mode_type a);
  void from_string (const std::string &s, Template::ruler_mode_type &a);
};

struct TemplatesConverter
{
  std::string to_string (const std::vector <Template> &t);
  void from_string (const std::string &s, std::vector <Template> &t);
};

}

#endif

