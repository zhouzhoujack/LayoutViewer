#ifndef HDR_dbOriginalLayerEdges
#define HDR_dbOriginalLayerEdges

#include "dbCommon.h"

#include "dbAsIfFlatEdges.h"
#include "dbShapes.h"
#include "dbRecursiveShapeIterator.h"

namespace db {

/**
 *  @brief An original layerregion based on a RecursiveShapeIterator
 */
class DB_PUBLIC OriginalLayerEdges
  : public AsIfFlatEdges
{
public:
  OriginalLayerEdges ();
  OriginalLayerEdges (const OriginalLayerEdges &other);
  OriginalLayerEdges (const RecursiveShapeIterator &si, bool is_merged = false);
  OriginalLayerEdges (const RecursiveShapeIterator &si, const db::ICplxTrans &trans, bool merged_semantics, bool is_merged = false);
  virtual ~OriginalLayerEdges ();

  EdgesDelegate *clone () const;

  virtual EdgesIteratorDelegate *begin () const;
  virtual EdgesIteratorDelegate *begin_merged () const;

  virtual std::pair<db::RecursiveShapeIterator, db::ICplxTrans> begin_iter () const;
  virtual std::pair<db::RecursiveShapeIterator, db::ICplxTrans> begin_merged_iter () const;

  virtual bool empty () const;

  virtual bool is_merged () const;

  virtual const db::Edge *nth (size_t n) const;
  virtual bool has_valid_edges () const;
  virtual bool has_valid_merged_edges () const;

  virtual const db::RecursiveShapeIterator *iter () const;

  virtual bool equals (const Edges &other) const;
  virtual bool less (const Edges &other) const;

protected:
  virtual void merged_semantics_changed ();

private:
  OriginalLayerEdges &operator= (const OriginalLayerEdges &other);

  bool m_is_merged;
  mutable db::Shapes m_merged_edges;
  mutable bool m_merged_edges_valid;
  mutable db::RecursiveShapeIterator m_iter;
  db::ICplxTrans m_iter_trans;

  void init ();
  void ensure_merged_edges_valid () const;
};

}

#endif

