#ifndef HDR_layFinder
#define HDR_layFinder


#include <vector>
#include "detail/utilsProgress.h"

#include "tlVector.h"
#include "layLayoutView.h"
#include "detail/geoBoxConvert.h"
#include "geoLayout.h"
#include "geoBox.h"
#include "detail/geoShape.h"
#include "layObjectInstPath.h"

namespace tl
{
  class AbsoluteProgress;
}

namespace lay
{

/**
 *  @brief A generic finder class
 *
 *  A finder traverses the hierarchy and calls the virtual "find"
 *  method on each cell.
 */
class Finder
{
public:
  /**
   *  @brief Constructor
   *
   *  The point_mode is true, if the finder is supposed to operate in "point mode".
   *  In point mode, the center of the search region is the reference point. In 
   *  non-point mode, every relevant found inside the search region will be
   *  recorded (also see point_mode method).
   *  The base class implementation just stores this flag and provides a read
   *  accessor with the point_mode () method.
   */
  Finder (bool point_mode, bool top_level_sel);

  /**
   *  @brief Gets a flag indicating whether point mode is enabled
   *  If point mode is enabled in the constructor, the first will check for objects overlapping the
   *  point (rather than being inside the box) and by default select a single object only.
   *  See also "set_catch_all".
   */
  bool point_mode () const
  {
    return m_point_mode;
  }

  /**
   *  @brief Gets a flag indicating the capture all founds even in point mode
   */
  bool catch_all () const
  {
    return m_catch_all;
  }

  /**
   *  @brief Sets a flag indicating the capture all founds even in point mode
   *  By default, in point mode only the closest found is returned. To catch all
   *  founds in point mode too, set this flag to true.
   */
  void set_catch_all (bool f)
  {
    m_catch_all = f;
  }

  /**
   *  @brief Destructor (just provided to please the compiler)
   */
  virtual ~Finder ();

  /**
   *  @brief Proximity getter
   *
   *  The "proximity" is the closest value passed to the "closer" method.
   *  It returns std::numeric_limits<double>::max () if nothing was found.
   */
  double proximity () const
  {
    return m_distance;
  }
  
protected:
  const std::vector<int> &layers () const
  {
    return m_layers;
  }

  const std::vector<geo::InstElement> &path () const
  {
    return m_path;
  }

  const geo::Layout &layout () const
  {
    return *mp_layout;
  }

  int min_level () const
  {
    return m_min_level;
  }

  int max_level () const
  {
    return m_max_level;
  }

  lay::LayoutView *view () const
  {
    return mp_view;
  }

  bool closer (double d);

  /** 
   *  @brief Start the scan with the given parameters
   *
   *  Starts the cell scan on the given layout object, with the given region,
   *  starting at the given cell, with the given range of hierarchy levels to 
   *  consider and using just the given layer or layers (unless the vector is empty, in which case all layers
   *  are used). For each matching cell, the "visit_cell" method is called. A 
   *  path of instantiations up to the top cell is maintained and accessible by
   *  the path() accessor.
   */
  void start (lay::LayoutView *view, const lay::CellView &cv, unsigned int cv_index, const std::vector<geo::ICplxTrans> &trans, const geo::Box &region, int min_level, int max_level, const std::vector<int> &layers = std::vector<int> ());

  /**
   *  @brief Provide a basic edge test facility
   *
   *  This method computes a "distance" of the edge to the reference point (the
   *  center of the search region). It updates "distance" if the computed distance
   *  is less than the one stored in "distance" or "match" is false. If the 
   *  distance is updated, match is set to true.
   *
   *  "trans" is the transformation to be applied to the edge before the test.
   *
   *  This method returns a mask indicating which point of the edge was matching.
   *  Bit 0 of this mask indicates the first point is matching, bit 1 indicates the
   *  second point is matching.
   */
  unsigned int test_edge (const geo::ICplxTrans &trans, const geo::Edge &edge, double &distance, bool &match);

private:
  void do_find (const geo::Cell &cell, int level, const geo::ICplxTrans &t);

  /**
   *  @brief Visitor sugar function
   *
   *  This method is supposed to do whatever the finder is supposed to do on the
   *  cell. It may use the "closer" method to determine if something is closer
   *  to whatever.
   */
  virtual void visit_cell (const geo::Cell &cell, const geo::Box &search_box, const geo::ICplxTrans &t, int level) = 0;

  int m_min_level, m_max_level;
  std::vector<geo::InstElement> m_path;
  const geo::Layout *mp_layout;
  lay::LayoutView *mp_view;
  unsigned int m_cv_index;
  geo::Box m_region;
  std::vector<int> m_layers;
  double m_distance;
  bool m_point_mode;
  bool m_catch_all;
  bool m_top_level_sel;
  geo::box_convert <geo::CellInst> m_box_convert;
  geo::box_convert <geo::Cell> m_cell_box_convert;
};

/**
 *  @brief Shape finder utility class
 *
 *  This class specializes the finder to finding shapes.
 */
class ShapeFinder
  : public Finder
{
public:
  struct StopException { };

  typedef std::vector<lay::ObjectInstPath> founds_vector_type;
  typedef founds_vector_type::const_iterator iterator;

  ShapeFinder (bool point_mode, bool top_level_sel, geo::ShapeIterator::flags_type flags, const std::set<lay::ObjectInstPath> *excludes = 0);

  bool find (lay::LayoutView *view, const lay::LayerProperties &lprops, const geo::DBox &region_mu);
  bool find (lay::LayoutView *view, const geo::DBox &region_mu);

  iterator begin () const
  {
    return m_founds.begin ();
  }

  iterator end () const
  {
    return m_founds.end ();
  }

protected:
  geo::ShapeIterator::flags_type flags () const 
  {
    return m_flags;
  }

  unsigned int cv_index () const 
  {
    return m_cv_index;
  }

  const std::set<geo::properties_id_type> *prop_sel () const 
  {
    return mp_prop_sel;
  }

  bool inv_prop_sel () const 
  {
    return m_inv_prop_sel;
  }

  geo::cell_index_type topcell () const
  {
    return m_topcell;
  }

  void set_test_count (int n)
  {
    m_tries = n;
  }

  void checkpoint ();

private:
  virtual void visit_cell (const geo::Cell &cell, const geo::Box &search_box, const geo::ICplxTrans &t, int /*level*/);
  bool find_internal (lay::LayoutView *view, 
                      unsigned int cv_index, 
                      const std::set<geo::properties_id_type> *prop_sel, 
                      bool inv_prop_sel, 
                      const lay::HierarchyLevelSelection &hier_sel,
                      const std::vector<geo::DCplxTrans> &trans_mu,
                      const std::vector<int> &layers, 
                      const geo::DBox &region_mu);

  const std::set<lay::ObjectInstPath> *mp_excludes;
  std::vector<lay::ObjectInstPath> m_founds;
  geo::ShapeIterator::flags_type m_flags;
  unsigned int m_cv_index;
  geo::cell_index_type m_topcell;
  const std::set<geo::properties_id_type> *mp_prop_sel;
  bool m_inv_prop_sel;
  int m_tries;
  geo::AbsoluteProgress *mp_progress;
  std::vector<int> m_context_layers;
  std::map<geo::cell_index_type, bool> m_cells_with_context;
};

/**
 *  @brief Instance finder utility class
 *
 *  This class specializes the finder to finding instanced.
 */
class InstFinder
  : public lay::Finder
{
public:
  struct StopException { };

  typedef std::vector<lay::ObjectInstPath> founds_vector_type;
  typedef founds_vector_type::const_iterator iterator;

  InstFinder (bool point_mode, bool top_level_sel, bool full_arrays, bool enclose_inst = true, const std::set<lay::ObjectInstPath> *excludes = 0, bool visible_layers = false);

  bool find (lay::LayoutView *view, unsigned int cv_index, const geo::DCplxTrans &trans, const geo::DBox &region_mu);
  bool find (lay::LayoutView *view, const geo::DBox &region_mu);
 
  iterator begin () const
  {
    return m_founds.begin ();
  }

  iterator end () const
  {
    return m_founds.end ();
  }

private:
  virtual void visit_cell (const geo::Cell &cell, const geo::Box &search_box, const geo::ICplxTrans &t, int level);
  bool find_internal (lay::LayoutView *view, unsigned int cv_index, const geo::DCplxTrans &trans_mu, const geo::DBox &region_mu);

  unsigned int m_cv_index;
  geo::cell_index_type m_topcell;
  const std::set<lay::ObjectInstPath> *mp_excludes;
  std::vector<lay::ObjectInstPath> m_founds;
  int m_tries;
  bool m_full_arrays;
  bool m_enclose_insts;
  bool m_visible_layers;
  std::vector<int> m_visible_layer_indexes;
  lay::LayoutView *mp_view;
  geo::AbsoluteProgress *mp_progress;
};

}

#endif

