#ifndef HDR_edtUtils
#define HDR_edtUtils

#include <limits>
#include "detail/utilsVariant.h"
#include <list>
#include <utility>
#include <vector>

#include <QtWidgets/QDialog>

#include "layObjectInstPath.h"

#include "detail/geoInstElement.h"
#include "detail/geoClipboardData.h"
#include "detail/geoClipboard.h"
#include "detail/geoPCellDeclaration.h"

namespace lay
{
  class LayoutView;
}

namespace edt {

class Service;

/**
 *  @brief Serializes PCell parameters to a string
 */
std::string pcell_parameters_to_string (const std::map<std::string, geo::Variant> &parameters);

/**
 *  @brief Deserializes PCell parameters from a string
 */
std::map<std::string, geo::Variant> pcell_parameters_from_string (const std::string &s);

/**
 *  @brief Fetch PCell parameters from a cell and merge the guiding shapes into them
 *
 *  @param layout The layout object
 *  @param cell_index The index of the cell from which to fetch the parameters
 *  @param parameters_for_pcell Will receive the parameters
 *  @return true, if the cell is a PCell and parameters have been fetched 
 */
bool
get_parameters_from_pcell_and_guiding_shapes (geo::Layout *layout, geo::cell_index_type cell_index, geo::pcell_parameters_type &parameters_for_pcell);

/**
 *  @brief A helper class that identifies clipboard data for edt::
 */
class ClipboardData
  : public geo::ClipboardData
{
public:
  ClipboardData () { }
};

/**
 *  @brief A cache for the transformation variants for a certain layer and cell view index for a lay::LayoutView
 */
class TransformationVariants
{
public:
  TransformationVariants (const lay::LayoutView *view, bool per_cv_and_layer = true, bool per_cv = true);

  const std::vector<geo::DCplxTrans> *per_cv_and_layer (unsigned int cv, unsigned int layer) const;
  const std::vector<geo::DCplxTrans> *per_cv (unsigned int cv) const;

private:
  std::map <unsigned int, std::vector<geo::DCplxTrans> > m_per_cv_tv;
  std::map < std::pair<unsigned int, unsigned int>, std::vector<geo::DCplxTrans> > m_per_cv_and_layer_tv;
};

/**
 *  @brief An iterator for the selected objects of all edt services in a layout view
 */
class SelectionIterator
{
public:
  typedef lay::ObjectInstPath value_type;
  typedef const lay::ObjectInstPath &reference;
  typedef const lay::ObjectInstPath *pointer;

  /**
   *  @brief Creates a new iterator iterating over all selected edt objects from the given view
   *
   *  If "including_transient" is true, the transient selection will be used as fallback.
   */
  SelectionIterator (lay::LayoutView *view, bool including_transient = true);

  /**
   *  @brief Returns a value indicating whether the transient selection is taken
   */
  bool is_transient () const
  {
    return m_transient_mode;
  }

  /**
   *  @brief Increments the iterator
   */
  void operator++ ()
  {
    inc ();
    next ();
  }

  /**
   *  @brief Dereferencing
   */
  const lay::ObjectInstPath &operator* () const
  {
    tl_assert (! at_end ());
    return *m_current_object;
  }

  /**
   *  @brief Arrow operator
   */
  const lay::ObjectInstPath *operator-> () const
  {
    return & operator* ();
  }

  /**
   *  @brief Returns a value indicating whether the iterator has finished
   */
  bool at_end () const;

private:
  void inc ();
  void next ();

private:
  std::vector<edt::Service *> mp_edt_services;
  std::vector<edt::Service *>::const_iterator m_current_service;
  std::set<lay::ObjectInstPath>::const_iterator m_current_object;
  bool m_transient_mode;
};

} // namespace edt

#endif

