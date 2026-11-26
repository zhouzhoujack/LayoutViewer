#include "layDrawing.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  Drawing implementation

Drawing::Drawing (unsigned int num_planes, Drawings *drawings)
  : m_num_planes (num_planes)
{
  drawings->push_back (this);
}

Drawing::~Drawing ()
{
  // .. nothing yet ..
}

}

