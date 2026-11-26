#ifndef HDR_rdbTiledRdbOutputReceiver
#define HDR_rdbTiledRdbOutputReceiver

#include "rdb.h"

#include "dbTilingProcessor.h"

namespace rdb
{

/**
 *  @brief A helper class for the generic implementation of the insert functionality
 */
class RdbInserter
{
public:
  RdbInserter (rdb::Database *rdb, rdb::id_type cell_id, rdb::id_type category_id, const db::CplxTrans &trans);

  template <class T>
  void operator() (const T &t)
  {
    rdb::Item *item = mp_rdb->create_item (m_cell_id, m_category_id);
    item->add_value (t.transformed (m_trans));
  }

  void operator() (const db::SimplePolygon &t);

private:
  rdb::Database *mp_rdb;
  rdb::id_type m_cell_id, m_category_id;
  const db::CplxTrans m_trans;
};

/**
 *  @brief A receiver for the db::TilingProcessor putting the output to the given RDB
 */
class TiledRdbOutputReceiver
  : public db::TileOutputReceiver
{
public:
  TiledRdbOutputReceiver (rdb::Database *rdb, size_t cell_id, size_t category_id);

  void put (size_t ix, size_t iy, const db::Box &tile, size_t id, const tl::Variant &obj, double dbu, const db::ICplxTrans &trans, bool clip);

private:
  rdb::Database *mp_rdb;
  size_t m_cell_id, m_category_id;
};

}

#endif

