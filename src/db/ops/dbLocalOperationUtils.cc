#include "dbLocalOperationUtils.h"
#include "dbPolygonTools.h"

namespace db
{

// -----------------------------------------------------------------------------------------------
//  class EdgeToEdgeSetGenerator

// -----------------------------------------------------------------------------
//  LocalOperationUtils implementation

EdgeToEdgeSetGenerator::EdgeToEdgeSetGenerator (std::unordered_set<db::Edge> &edges)
  : mp_edges (&edges)
{
  //  .. nothing yet ..
}

void EdgeToEdgeSetGenerator::put (const db::Edge &edge)
{
  mp_edges->insert (edge);
}

// -----------------------------------------------------------------------------------------------
//  class PolygonRefGenerator

PolygonRefToShapesGenerator::PolygonRefToShapesGenerator (db::Layout *layout, db::Shapes *shapes)
  : PolygonSink (), mp_layout (layout), mp_shapes (shapes)
{
  //  .. nothing yet ..
}

void PolygonRefToShapesGenerator::put (const db::Polygon &polygon)
{
  tl::MutexLocker locker (&mp_layout->lock ());
  mp_shapes->insert (db::PolygonRef (polygon, mp_layout->shape_repository ()));
}

// -----------------------------------------------------------------------------------------------
//  class PolygonSplitter

PolygonSplitter::PolygonSplitter (PolygonSink &sink, double max_area_ratio, size_t max_vertex_count)
  : mp_sink (&sink), m_max_area_ratio (max_area_ratio), m_max_vertex_count (max_vertex_count)
{
  //  .. nothing yet ..
}

void
PolygonSplitter::put (const db::Polygon &poly)
{
  if ((m_max_vertex_count > 0 && poly.vertices () > m_max_vertex_count) || (m_max_area_ratio > 0.0 && poly.area_ratio () > m_max_area_ratio)) {

    std::vector <db::Polygon> split_polygons;
    db::split_polygon (poly, split_polygons);
    for (std::vector <db::Polygon>::const_iterator sp = split_polygons.begin (); sp != split_polygons.end (); ++sp) {
      put (*sp);
    }

  } else {
    mp_sink->put (poly);
  }
}

}
