#include "geoBox.h"
#include "geoLayout.h"

#include "layObjectInstPath.h"
#include "layCellView.h"
#include "layLayoutView.h"
#include "tlException.h"

namespace lay {

// ----------------------------------------------------------------------
//  ObjectInstPath implementation

ObjectInstPath::ObjectInstPath ()
  : m_cv_index (0), m_topcell (0), m_layer (-1), m_seq (0)
{
  //  .. nothing yet ..
}

geo::cell_index_type 
ObjectInstPath::cell_index_tot () const
{
  if (m_path.empty ()) {
    return m_topcell;
  } else {
    return m_path.back ().inst_ptr.cell_index ();
  }
}

geo::ICplxTrans
ObjectInstPath::trans_tot () const
{
  geo::ICplxTrans t;
  iterator end = m_path.end ();
  for (iterator i = m_path.begin (); i != end; ++i) {
    t = t * i->complex_trans ();
  }
  return t;
}

geo::cell_index_type 
ObjectInstPath::cell_index () const
{
  if (m_path.empty ()) {
    return m_topcell;
  } else if (! is_cell_inst ()) {
    return m_path.back ().inst_ptr.cell_index ();
  } else {
    iterator end = m_path.end ();
    --end;
    if (end == m_path.begin ()) {
      return m_topcell;
    } else {
      --end;
      return end->inst_ptr.cell_index ();
    }
  }
}

geo::ICplxTrans
ObjectInstPath::trans () const
{
  geo::ICplxTrans t;
  //  For instances, the last element is the instance itself - do not count that in the transformation.
  iterator end = m_path.end ();
  if (is_cell_inst () && end != m_path.begin ()) {
    --end;
  }
  for (iterator i = m_path.begin (); i != end; ++i) {
    t = t * i->complex_trans ();
  }
  return t;
}

void 
ObjectInstPath::remove_front (unsigned int n)
{
  while (n > 0) {
    --n;
    tl_assert (! m_path.empty ());
    if (n == 0) {
      m_topcell = m_path.front ().inst_ptr.cell_index ();
    }
    m_path.erase (m_path.begin ());
  }
}

void 
ObjectInstPath::insert_front (geo::cell_index_type topcell, const geo::InstElement &elem)
{
  tl_assert (m_topcell == elem.inst_ptr.cell_index ());
  m_topcell = topcell;
  m_path.insert (m_path.begin (), elem);
}

bool 
ObjectInstPath::operator< (const ObjectInstPath &d) const
{
  if (is_cell_inst () != d.is_cell_inst ()) {
    return is_cell_inst () < d.is_cell_inst ();
  }
  if (! is_cell_inst ()) {
    if (m_layer != d.m_layer) {
      return m_layer < d.m_layer;
    }
    if (m_shape != d.m_shape) {
      return m_shape < d.m_shape;
    }
  }
  if (m_cv_index != d.m_cv_index) {
    return m_cv_index < d.m_cv_index;
  }
  if (m_topcell != d.m_topcell) {
    return m_topcell < d.m_topcell;
  }
  return m_path < d.m_path;
}

bool 
ObjectInstPath::operator== (const ObjectInstPath &d) const
{
  if (is_cell_inst () != d.is_cell_inst ()) {
    return false;
  }
  if (! is_cell_inst ()) {
    if (m_layer != d.m_layer) {
      return false;
    } 
    if (m_shape != d.m_shape) {
      return false;
    }
  }
  if (m_cv_index != d.m_cv_index) {
    return false;
  }
  if (m_topcell != d.m_topcell) {
    return false;
  }
  return m_path == d.m_path;
}

}


