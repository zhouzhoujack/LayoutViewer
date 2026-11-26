#ifndef HDR_dbOriginalLayerTexts
#define HDR_dbOriginalLayerTexts

#include "dbCommon.h"

#include "dbAsIfFlatTexts.h"
#include "dbShapes.h"
#include "dbRecursiveShapeIterator.h"

namespace db {

/**
 *  @brief An original layer text collection based on a RecursiveShapeIterator
 */
class DB_PUBLIC OriginalLayerTexts
  : public AsIfFlatTexts
{
public:
  OriginalLayerTexts ();
  OriginalLayerTexts (const OriginalLayerTexts &other);
  OriginalLayerTexts (const RecursiveShapeIterator &si);
  OriginalLayerTexts (const RecursiveShapeIterator &si, const db::ICplxTrans &trans);
  virtual ~OriginalLayerTexts ();

  TextsDelegate *clone () const;

  virtual TextsIteratorDelegate *begin () const;
  virtual std::pair<db::RecursiveShapeIterator, db::ICplxTrans> begin_iter () const;

  virtual bool empty () const;

  virtual const db::Text *nth (size_t n) const;
  virtual bool has_valid_texts () const;

  virtual const db::RecursiveShapeIterator *iter () const;

  virtual bool equals (const Texts &other) const;
  virtual bool less (const Texts &other) const;

private:
  OriginalLayerTexts &operator= (const OriginalLayerTexts &other);

  mutable db::RecursiveShapeIterator m_iter;
  db::ICplxTrans m_iter_trans;

  void init ();
};

}

#endif

