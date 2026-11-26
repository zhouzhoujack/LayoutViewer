#ifndef HDR_edtServiceImpl
#define HDR_edtServiceImpl

#include "layEdtService.h"
#include "detail/utilsVariant.h"
#include "layEdtConfig.h"

#include <memory>

namespace lay
{
  class CellView;
  class LayerPropertiesConstIterator;
}

namespace edt
{

/**
 *  @brief Implementation of the edt::Service for generic shape editing
 */
class ShapeEditService
  : public edt::Service
{
public:
  ShapeEditService (geo::Manager *manager, lay::LayoutView *view, geo::ShapeIterator::flags_type shape_types);
  
protected:
  void get_edit_layer ();

  const geo::VCplxTrans &trans () const { return m_trans; }
  unsigned int layer () const          { return m_layer; }
  unsigned int cv_index () const       { return m_cv_index; }
  geo::Cell &cell () const              { return *mp_cell; }
  geo::Layout &layout () const          { return *mp_layout; }

  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual void tap (const geo::DPoint &initial);

  virtual bool configure (const std::string &name, const std::string &value);

protected:
  std::pair <bool, geo::DPoint> interpolate (const geo::DPoint &m, const geo::DPoint &o, const geo::DPoint &p) const;
  void deliver_shape (const geo::Polygon &poly);
  void deliver_shape (const geo::Path &path);
  void deliver_shape (const geo::Box &box);
  virtual void current_layer_changed () { }

private:
  geo::VCplxTrans m_trans;
  unsigned int m_layer;
  unsigned int m_cv_index;
  geo::Cell *mp_cell;
  geo::Layout *mp_layout;
  combine_mode_type m_combine_mode;

  void update_edit_layer (const lay::LayerPropertiesConstIterator &iter);
};

/**
 *  @brief Implementation of edt::Service for polygon editing
 */
class PolygonService
  : public ShapeEditService
{
public:
  PolygonService (geo::Manager *manager, lay::LayoutView *view);
  
  virtual lay::PropertiesPage *properties_page (geo::Manager *manager, QWidget *parent);
  virtual void do_begin_edit (const geo::DPoint &p);
  virtual void do_mouse_move (const geo::DPoint &p);
  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual bool do_mouse_click (const geo::DPoint &p);
  virtual void do_finish_edit ();
  virtual void do_cancel_edit ();
  virtual bool selection_applies (const lay::ObjectInstPath &sel) const;

private:
  std::vector <geo::DPoint> m_points;
  bool m_closure_set;
  geo::DPoint m_closure;
  geo::DPoint m_last;

  void update_marker ();
  geo::Polygon get_polygon () const;
  void add_closure ();
  void set_last_point (const geo::DPoint &p);
};

/**
 *  @brief Implementation of edt::Service for box editing
 */
class BoxService
  : public ShapeEditService
{
public:
  BoxService (geo::Manager *manager, lay::LayoutView *view);
  
  virtual lay::PropertiesPage *properties_page (geo::Manager *manager, QWidget *parent);
  virtual void do_begin_edit (const geo::DPoint &p);
  virtual void do_mouse_move (const geo::DPoint &p);
  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual bool do_mouse_click (const geo::DPoint &p);
  virtual void do_finish_edit ();
  virtual void do_cancel_edit ();
  virtual bool selection_applies (const lay::ObjectInstPath &sel) const;

private:
  geo::DPoint m_p1, m_p2;

  void update_marker ();
  geo::Box get_box () const;
};

/**
 *  @brief Implementation of edt::Service for text editing
 */
class TextService
  : public ShapeEditService
{
public:
  TextService (geo::Manager *manager, lay::LayoutView *view);
  ~TextService ();
  
  virtual lay::PropertiesPage *properties_page (geo::Manager *manager, QWidget *parent);
  virtual void do_begin_edit (const geo::DPoint &p);
  virtual void do_mouse_transform (const geo::DPoint &p, geo::DFTrans trans);
  virtual void do_mouse_move (const geo::DPoint &p);
  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual bool do_mouse_click (const geo::DPoint &p);
  virtual void do_finish_edit ();
  virtual void do_cancel_edit ();
  virtual bool do_activated ();
  virtual bool selection_applies (const lay::ObjectInstPath &sel) const;

protected:
  virtual bool configure (const std::string &name, const std::string &value);

private:
  geo::DText m_text;
  unsigned int m_rot;

  void update_marker ();
  geo::Text get_text () const;
};

/**
 *  @brief Implementation of edt::Service for path editing
 */
class PathService
  : public ShapeEditService
{
public:
  PathService (geo::Manager *manager, lay::LayoutView *view);
  ~PathService ();
  
  virtual lay::PropertiesPage *properties_page (geo::Manager *manager, QWidget *parent);
  virtual void do_begin_edit (const geo::DPoint &p);
  virtual void do_mouse_move (const geo::DPoint &p);
  virtual bool do_mouse_click (const geo::DPoint &p);
  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual void do_finish_edit ();
  virtual void do_cancel_edit ();
  virtual bool do_activated ();
  virtual bool selection_applies (const lay::ObjectInstPath &sel) const;

protected:
  bool configure (const std::string &name, const std::string &value);
  void config_finalize ();

private:
  std::vector <geo::DPoint> m_points;
  double m_width, m_bgnext, m_endext;
  enum { Flush = 0, Square, Variable, Round } m_type;
  bool m_needs_update;
  geo::DPoint m_last;

  void update_marker ();
  geo::Path get_path () const;
  void set_last_point (const geo::DPoint &p);
};

/**
 *  @brief Implementation of edt::Service for instance editing
 */
class InstService
  : public edt::Service
{
public:
  InstService (geo::Manager *manager, lay::LayoutView *view);
  
  virtual lay::PropertiesPage *properties_page (geo::Manager *manager, QWidget *parent);
  virtual void do_begin_edit (const geo::DPoint &p);
  virtual void do_mouse_move_inactive (const geo::DPoint &p);
  virtual void do_mouse_move (const geo::DPoint &p);
  virtual bool do_mouse_click (const geo::DPoint &p);
  virtual void do_mouse_transform (const geo::DPoint &p, geo::DFTrans trans);
  virtual void do_finish_edit ();
  virtual void do_cancel_edit ();
  virtual bool do_activated ();
  virtual bool drag_enter_event (const geo::DPoint &p, const lay::DragDropDataBase *data);
  virtual bool drag_move_event (const geo::DPoint &p, const lay::DragDropDataBase *data);
  virtual void drag_leave_event ();
  virtual bool drop_event (const geo::DPoint &p, const lay::DragDropDataBase *data);
  virtual bool selection_applies (const lay::ObjectInstPath &sel) const;

protected:
  bool configure (const std::string &name, const std::string &value);
  void service_configuration_changed ();

  void config_finalize ();

private:
  double m_angle;
  double m_scale;
  bool m_mirror;
  geo::DPoint m_disp;
  std::string m_cell_or_pcell_name, m_lib_name;
  std::string m_cell_or_pcell_name_previous, m_lib_name_previous;
  std::map<std::string, geo::Variant> m_pcell_parameters;
  std::map<std::pair<std::string, std::string>, std::map<std::string, geo::Variant> > m_stored_pcell_parameters;
  bool m_is_pcell;
  bool m_array;
  unsigned int m_rows, m_columns;
  double m_row_x, m_row_y, m_column_x, m_column_y;
  bool m_place_origin;
  geo::Manager::transaction_id_t m_reference_transaction_id;
  bool m_needs_update, m_parameters_changed;
  bool m_has_valid_cell;
  bool m_in_drag_drop;
  geo::cell_index_type m_current_cell;
  geo::Layout *mp_current_layout;
  const geo::PCellDeclaration *mp_pcell_decl;
  int m_cv_index;
  geo::ICplxTrans m_trans;

  void update_marker ();
  bool get_inst (geo::CellInstArray &inst);
  std::pair<bool, geo::cell_index_type> make_cell (const lay::CellView &cv);
  geo::Variant get_default_layer_for_pcell ();
  void sync_to_config ();
  void switch_cell_or_pcell (bool switch_parameters);
};

}

#endif

