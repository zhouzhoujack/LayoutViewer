#include "gsi.h"
#include "gsiSerialisation.h"

namespace gsi
{

class AdaptorSynchronizer
{
public:
  AdaptorSynchronizer (AdaptorBase *src, AdaptorBase *target, tl::Heap *heap)
    : mp_src (src), mp_target (target), mp_heap (heap)
  {
    //  .. nothing yet ..
  }

  ~AdaptorSynchronizer ()
  {
    mp_src->copy_to (mp_target, *mp_heap);
    delete mp_src;
    delete mp_target;
    mp_src = 0;
  }

private:
  AdaptorBase *mp_src, *mp_target;
  tl::Heap *mp_heap;
};

// -----------------------------------------------------------------------------
//  Serialisation implementation

AdaptorBase::AdaptorBase ()
{
  //  .. nothing yet ..
}

AdaptorBase::~AdaptorBase ()
{
  //  .. nothing yet ..
}

void AdaptorBase::tie_copies (AdaptorBase *target, tl::Heap &heap) 
{
  std::unique_ptr<AdaptorBase> t (target);
  copy_to (target, heap);

  //  This object (which will be destroyed before this is responsible for copying back 
  //  the contents of target into this once the heap goes out of scope)
  heap.push (new AdaptorSynchronizer (t.release (), this, &heap));
}

}

