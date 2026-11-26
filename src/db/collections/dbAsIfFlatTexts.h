#ifndef HDR_dbAsIfFlatTexts
#define HDR_dbAsIfFlatTexts

#include "dbCommon.h"

#include "dbTextsDelegate.h"

namespace db {

class Region;

/**
 *  @brief Provides default flat implementations
 */
class DB_PUBLIC AsIfFlatTexts
  : public TextsDelegate
{
public:
  AsIfFlatTexts ();
  AsIfFlatTexts (const AsIfFlatTexts &other);
  virtual ~AsIfFlatTexts ();

  virtual size_t count () const;
  virtual size_t hier_count () const;
  virtual std::string to_string (size_t) const;
  virtual Box bbox () const;

  virtual TextsDelegate *filter_in_place (const TextFilterBase &filter)
  {
    return filtered (filter);
  }

  virtual TextsDelegate *filtered (const TextFilterBase &) const;

  virtual RegionDelegate *processed_to_polygons (const TextToPolygonProcessorBase &filter) const;

  virtual TextsDelegate *add_in_place (const Texts &other)
  {
    return add (other);
  }

  virtual TextsDelegate *add (const Texts &other) const;

  virtual RegionDelegate *polygons (db::Coord e) const;
  virtual EdgesDelegate *edges () const;

  virtual TextsDelegate *in (const Texts &, bool) const;

  virtual bool equals (const Texts &other) const;
  virtual bool less (const Texts &other) const;

  virtual void insert_into (Layout *layout, db::cell_index_type into_cell, unsigned int into_layer) const;
  virtual void insert_into_as_polygons (Layout *layout, db::cell_index_type into_cell, unsigned int into_layer, db::Coord enl) const;

  virtual RegionDelegate *pull_interacting (const Region &) const;
  virtual TextsDelegate *selected_interacting (const Region &other) const;
  virtual TextsDelegate *selected_not_interacting (const Region &other) const;

protected:
  void update_bbox (const db::Box &box);
  void invalidate_bbox ();
  AsIfFlatTexts &operator= (const AsIfFlatTexts &other);

private:

  mutable bool m_bbox_valid;
  mutable db::Box m_bbox;

  virtual db::Box compute_bbox () const;
  virtual TextsDelegate *selected_interacting_generic (const Region &other, bool inverse) const;
  virtual RegionDelegate *pull_generic (const Region &other) const;
};

}

#endif

