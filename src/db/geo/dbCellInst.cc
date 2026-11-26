#include "dbCellInst.h"
#include "dbLayout.h"

namespace db
{

// -----------------------------------------------------------------------------
//  CellInst implementation

CellInst::box_type 
CellInst::bbox (const db::Layout &g) const
{
  return g.cell (m_cell_index).bbox ();
}

CellInst::box_type
CellInst::bbox (const db::Layout &g, unsigned int l) const
{
  return g.cell (m_cell_index).bbox (l);
}

std::string 
CellInst::to_string () const
{
  return "[" + tl::to_string (m_cell_index) + "]";
}

}

