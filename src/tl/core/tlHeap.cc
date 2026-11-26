#include "tlHeap.h"

namespace tl
{

// ----------------------------------------------------------
//  HeapObject implementation

HeapObject::HeapObject () 
  : mp_b (0) 
{ 
  //  .. nothing yet ..
}

HeapObject::~HeapObject () 
{
  if (mp_b) {
    delete mp_b;
  }
  mp_b = 0;
}

// ----------------------------------------------------------
//  Heap implementation

Heap::Heap ()
{
  //  .. nothing yet ..
}

Heap::~Heap ()
{
  //  this scheme ensures we delete the objects in the reverse order they are created.
  while (! m_objects.empty ()) {
    m_objects.pop_back ();
  }
}

}

