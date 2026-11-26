#ifndef HDR_dbOriginalLayerEdgePairs
#define HDR_dbOriginalLayerEdgePairs

#include "dbCommon.h"

#include "dbAsIfFlatEdgePairs.h"
#include "dbShapes.h"
#include "dbRecursiveShapeIterator.h"

namespace db {

/**
 *  @brief An original layerregion based on a RecursiveShapeIterator
 */
class DB_PUBLIC OriginalLayerEdgePairs
  : public AsIfFlatEdgePairs
{
public:
  OriginalLayerEdgePairs ();
  OriginalLayerEdgePairs (const OriginalLayerEdgePairs &other);
  OriginalLayerEdgePairs (const RecursiveShapeIterator &si);
  OriginalLayerEdgePairs (const RecursiveShapeIterator &si, const db::ICplxTrans &trans);
  virtual ~OriginalLayerEdgePairs ();

  EdgePairsDelegate *clone () const;

  virtual EdgePairsIteratorDelegate *begin () const;
  virtual std::pair<db::RecursiveShapeIterator, db::ICplxTrans> begin_iter () const;

  virtual bool empty () const;

  virtual const db::EdgePair *nth (size_t n) const;
  virtual bool has_valid_edge_pairs () const;

  virtual const db::RecursiveShapeIterator *iter () const;

  virtual bool equals (const EdgePairs &other) const;
  virtual bool less (const EdgePairs &other) const;

private:
  OriginalLayerEdgePairs &operator= (const OriginalLayerEdgePairs &other);

  mutable db::RecursiveShapeIterator m_iter;
  db::ICplxTrans m_iter_trans;

  void init ();
};

}

#endif

