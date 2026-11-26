#ifndef _HDR_dbShapeCollection
#define _HDR_dbShapeCollection

#include "dbCommon.h"
#include "dbDeepShapeStore.h"
#include "tlUniqueId.h"
#include "gsiObject.h"

namespace db
{

/**
 *  @brief A base class for the deep collection delegates
 */
class DB_PUBLIC DeepShapeCollectionDelegateBase
{
public:
  DeepShapeCollectionDelegateBase () { }

  DeepShapeCollectionDelegateBase (const DeepShapeCollectionDelegateBase &other)
  {
    m_deep_layer = other.m_deep_layer.copy ();
  }

  DeepShapeCollectionDelegateBase &operator= (const DeepShapeCollectionDelegateBase &other)
  {
    if (this != &other) {
      m_deep_layer = other.m_deep_layer.copy ();
    }
    return *this;
  }

  const db::DeepLayer &deep_layer () const
  {
    return m_deep_layer;
  }

  db::DeepLayer &deep_layer ()
  {
    return m_deep_layer;
  }

protected:
  virtual void set_deep_layer (const db::DeepLayer &dl)
  {
    m_deep_layer = dl;
  }

private:
  db::DeepLayer m_deep_layer;
};

/**
 *  @brief A base class for the shape collection delegates
 */
class DB_PUBLIC ShapeCollectionDelegateBase
  : public tl::UniqueId
{
public:
  ShapeCollectionDelegateBase () { }
  virtual ~ShapeCollectionDelegateBase () { }

  virtual DeepShapeCollectionDelegateBase *deep () { return 0; }
};

/**
 *  @brief A base class for the shape collections such as Region, Edges, EdgePairs etc.
 */
class DB_PUBLIC ShapeCollection
  : public gsi::ObjectBase
{
public:
  ShapeCollection () { }
  virtual ~ShapeCollection () { }

  virtual ShapeCollectionDelegateBase *get_delegate () const = 0;
};

}

namespace tl
{

template<> struct type_traits<db::ShapeCollection> : public tl::type_traits<void>
{
  //  mark "NetlistDeviceExtractor" as having a default ctor and no copy ctor
  typedef tl::false_tag has_copy_constructor;
  typedef tl::false_tag has_default_constructor;
};

}

#endif
