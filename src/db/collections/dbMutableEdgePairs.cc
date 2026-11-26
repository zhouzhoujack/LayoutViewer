#include "dbMutableEdgePairs.h"

namespace db
{

// -------------------------------------------------------------------------------------------------------------
//  MutableEdgePairs implementation

MutableEdgePairs::MutableEdgePairs ()
  : AsIfFlatEdgePairs ()
{
  //  .. nothing yet ..
}

MutableEdgePairs::MutableEdgePairs (const MutableEdgePairs &other)
  : AsIfFlatEdgePairs (other)
{
  //  .. nothing yet ..
}

MutableEdgePairs::~MutableEdgePairs ()
{
  //  .. nothing yet ..
}

void
MutableEdgePairs::insert (const db::Shape &shape)
{
  if (shape.is_edge_pair ()) {
    insert (shape.edge_pair ());
  }
}

}

