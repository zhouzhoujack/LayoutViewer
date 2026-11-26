#ifndef HDR_dbShapeFlags
#define HDR_dbShapeFlags

#include "dbCommon.h"
#include "dbShapes.h"

namespace db
{

template <class T> unsigned int shape_flags ();
template <class T> unsigned int shape_flags_pure ();

template <> inline unsigned int shape_flags<db::PolygonRef> ()      { return 1 << db::ShapeIterator::PolygonRef; }
template <> inline unsigned int shape_flags_pure<db::PolygonRef> () { return 1 << db::ShapeIterator::PolygonRef; }

template <> inline unsigned int shape_flags<db::TextRef> ()         { return 1 << db::ShapeIterator::TextRef; }
template <> inline unsigned int shape_flags_pure<db::TextRef> ()    { return 1 << db::ShapeIterator::TextRef; }

template <> inline unsigned int shape_flags<db::Box> ()             { return db::ShapeIterator::Boxes; }
template <> inline unsigned int shape_flags_pure<db::Box> ()        { return 1 << db::ShapeIterator::Box; }

template <> inline unsigned int shape_flags<db::Path> ()            { return db::ShapeIterator::Paths; }
template <> inline unsigned int shape_flags_pure<db::Path> ()       { return 1 << db::ShapeIterator::Path; }

template <> inline unsigned int shape_flags<db::Polygon> ()         { return db::ShapeIterator::Polygons; }
template <> inline unsigned int shape_flags_pure<db::Polygon> ()    { return 1 << db::ShapeIterator::Polygon; }

template <> inline unsigned int shape_flags<db::Edge> ()            { return db::ShapeIterator::Edges; }
template <> inline unsigned int shape_flags_pure<db::Edge> ()       { return 1 << db::ShapeIterator::Edge; }

template <> inline unsigned int shape_flags<db::EdgePair> ()        { return db::ShapeIterator::EdgePairs; }
template <> inline unsigned int shape_flags_pure<db::EdgePair> ()   { return 1 << db::ShapeIterator::EdgePair; }

template <> inline unsigned int shape_flags<db::Text> ()            { return db::ShapeIterator::Texts; }
template <> inline unsigned int shape_flags_pure<db::Text> ()       { return 1 << db::ShapeIterator::Text; }


template <class T>
struct DB_PUBLIC shape_to_object
{
  void set (const db::Shape &) { }
  const T *get (const db::Shape &s) const { return s.basic_ptr (typename T::tag ()); }
};


template <>
struct DB_PUBLIC shape_to_object<db::Polygon>
{
  typedef db::Polygon value_type;

  void set (const db::Shape &s) { s.polygon (m_shape); }
  const value_type *get (const db::Shape &) const { return &m_shape; }

private:
  value_type m_shape;
};

template <>
struct DB_PUBLIC shape_to_object<db::SimplePolygon>
{
  typedef db::SimplePolygon value_type;

  void set (const db::Shape &s) { s.simple_polygon (m_shape); }
  const value_type *get (const db::Shape &) const { return &m_shape; }

private:
  value_type m_shape;
};

template <>
struct DB_PUBLIC shape_to_object<db::Path>
{
  typedef db::Path value_type;

  void set (const db::Shape &s) { s.path (m_shape); }
  const value_type *get (const db::Shape &) const { return &m_shape; }

private:
  value_type m_shape;
};

template <>
struct DB_PUBLIC shape_to_object<db::Text>
{
  typedef db::Text value_type;

  void set (const db::Shape &s) { s.text (m_shape); }
  const value_type *get (const db::Shape &) const { return &m_shape; }

private:
  value_type m_shape;
};

template <>
struct DB_PUBLIC shape_to_object<db::Box>
{
  typedef db::Box value_type;

  void set (const db::Shape *s) { s->box (m_shape); }
  const value_type *get (const db::Shape *) const { return &m_shape; }

private:
  value_type m_shape;
};

}

#endif
