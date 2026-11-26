#ifndef HDR_tlHeap
#define HDR_tlHeap

#include "tlCommon.h"

#include "tlAssert.h"

#include <list>

namespace tl
{

/**
 *  @brief A helper class that provides a base class for wrapped pointers for HeapObject
 */
class HeapObjectBase 
{
public:
  virtual ~HeapObjectBase () { }
};

/**
 *  @brief A helper class that provides a wrapper to a custom type for HeapObject
 */
template <class X>
class HeapObjectCont : public HeapObjectBase 
{
public:
  HeapObjectCont (X *x) : mp_x (x) { }
  ~HeapObjectCont () { delete mp_x; }
private:
  X *mp_x;
};

/**
 *  @brief A simple autopointer class to hold references to temporary objects
 */
class TL_PUBLIC HeapObject
{
public:
  HeapObject ();

  ~HeapObject ();

  template <class X>
  void set (X *x) 
  { 
    tl_assert (mp_b == 0);
    mp_b = new HeapObjectCont<X> (x); 
  }

private:
  HeapObjectBase *mp_b;
};

/**
 *  @brief A heap holding objects of an arbitrary type
 *
 *  The heap is basically a storage for temporary objects. 
 *  Such objects are created on the heap and are destroyed automatically
 *  when the heap goes out of scope.
 *
 *  It is guaranteed that objects created on the heap will be 
 *  destroyed in the reverse order they are created.
 */
class TL_PUBLIC Heap
{
public:
  /**
   *  @brief Constructor
   */
  Heap ();

  /**
   *  @brief Destructor
   */
  ~Heap ();

  /**
   *  @brief Register a new object
   */
  template <class X>
  void push (X *x)
  {
    m_objects.push_back (HeapObject ());
    m_objects.back ().set (x);
  }

  /**
   *  @brief Convenience method: create and register an object of type X
   */
  template <class X>
  X *create () 
  {
    X *x = new X ();
    push (x);
    return x;
  }

  /**
   *  @brief Returns a value indicating whether the heap is empty
   */
  bool empty () const
  {
    return m_objects.empty ();
  }

private:
  std::list<HeapObject> m_objects;
};

}

#endif

