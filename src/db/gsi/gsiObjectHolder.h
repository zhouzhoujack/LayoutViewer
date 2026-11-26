#ifndef _HDR_gsiObjectHolder
#define _HDR_gsiObjectHolder

#include "gsiCommon.h"

namespace gsi
{

class ClassBase;

/**
 *  @brief Implements an object holder
 *  This is some kind of auto pointer which acts on generic objects based on the 
 *  gsi::ClassBase scheme.
 */
class GSI_PUBLIC ObjectHolder
{
public:
  /**
   *  @brief Constructor
   *  Creates a smart pointer holding an object of class "cls".
   */
  ObjectHolder (const gsi::ClassBase *cls, void *obj);

  /**
   *  @brief Destructor
   */
  ~ObjectHolder ();

  /**
   *  @brief Resets the holder to a new pointer
   */
  void reset (const gsi::ClassBase *cls, void *obj);

  /**
   *  @brief Gets the object while releasing the ownership
   */
  void *release ();

  /**
   *  @brief Gets the object
   */
  void *obj () const
  {
    return mp_obj;
  }

  /**
   *  @brief Gets the class
   */
  const gsi::ClassBase *cls () const
  {
    return mp_cls;
  }

private:
  const gsi::ClassBase *mp_cls;
  void *mp_obj;
};

}

#endif

