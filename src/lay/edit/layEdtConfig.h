#ifndef HDR_edtConfig
#define HDR_edtConfig

#include <string>

#include "laySnap.h"
#include "layEdtCommon.h"
#include "tlString.h"
#include "detail/geoPoint.h"
#include "detail/geoHersheyFont.h"

namespace edt
{

/** 
 *  @brief Declaration of the configuration names
 */
extern EDT_PUBLIC std::string cfg_edit_grid;
extern EDT_PUBLIC std::string cfg_edit_snap_to_objects;
extern EDT_PUBLIC std::string cfg_edit_move_angle_mode;
extern EDT_PUBLIC std::string cfg_edit_connect_angle_mode;
extern EDT_PUBLIC std::string cfg_edit_text_string;
extern EDT_PUBLIC std::string cfg_edit_text_size;
extern EDT_PUBLIC std::string cfg_edit_text_halign;
extern EDT_PUBLIC std::string cfg_edit_text_valign;
extern EDT_PUBLIC std::string cfg_edit_path_width;
extern EDT_PUBLIC std::string cfg_edit_path_ext_type;
extern EDT_PUBLIC std::string cfg_edit_path_ext_var_begin;
extern EDT_PUBLIC std::string cfg_edit_path_ext_var_end;
extern EDT_PUBLIC std::string cfg_edit_global_grid;
extern EDT_PUBLIC std::string cfg_edit_show_shapes_of_instances;
extern EDT_PUBLIC std::string cfg_edit_max_shapes_of_instances;
extern EDT_PUBLIC std::string cfg_edit_inst_cell_name;
extern EDT_PUBLIC std::string cfg_edit_inst_lib_name;
extern EDT_PUBLIC std::string cfg_edit_inst_pcell_parameters;
extern EDT_PUBLIC std::string cfg_edit_inst_angle;
extern EDT_PUBLIC std::string cfg_edit_inst_mirror;
extern EDT_PUBLIC std::string cfg_edit_inst_scale;
extern EDT_PUBLIC std::string cfg_edit_inst_array;
extern EDT_PUBLIC std::string cfg_edit_inst_rows;
extern EDT_PUBLIC std::string cfg_edit_inst_row_x;
extern EDT_PUBLIC std::string cfg_edit_inst_row_y;
extern EDT_PUBLIC std::string cfg_edit_inst_columns;
extern EDT_PUBLIC std::string cfg_edit_inst_column_x;
extern EDT_PUBLIC std::string cfg_edit_inst_column_y;
extern EDT_PUBLIC std::string cfg_edit_inst_place_origin;
extern EDT_PUBLIC std::string cfg_edit_top_level_selection;
extern EDT_PUBLIC std::string cfg_edit_hier_copy_mode;
extern EDT_PUBLIC std::string cfg_edit_combine_mode;

// ------------------------------------------------------------
//  Helper functions to get and set the configuration

enum combine_mode_type { CM_Add = 0, CM_Merge, CM_Erase, CM_Mask, CM_Diff };

struct EDT_PUBLIC CMConverter
{
  std::string to_string (const edt::combine_mode_type &m);
  void from_string (const std::string &s, edt::combine_mode_type &m);
};

enum path_ext_type { Flush = 0, Square, Variable, Round, NumPEModes };

struct EDT_PUBLIC ACConverter
{
  std::string to_string (const lay::angle_constraint_type &m);
  void from_string (const std::string &s, lay::angle_constraint_type &m);
};

struct EDT_PUBLIC PathExtConverter
{
  std::string to_string (const edt::path_ext_type &m);
  void from_string (const std::string &s, edt::path_ext_type &m);
};

struct EDT_PUBLIC EditGridConverter
{
  std::string to_string (const geo::DVector &eg);
  void from_string (const std::string &s, geo::DVector &eg);
  void from_string_picky (const std::string &s, geo::DVector &eg);
};

struct EDT_PUBLIC HAlignConverter
{
  std::string to_string (geo::HAlign a);
  void from_string (const std::string &s, geo::HAlign &a);
};

struct EDT_PUBLIC VAlignConverter
{
  std::string to_string (geo::VAlign a);
  void from_string (const std::string &s, geo::VAlign &a);
};

}

#endif


