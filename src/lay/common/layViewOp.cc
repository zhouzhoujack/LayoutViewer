#include "layViewOp.h"
#include "layDitherPattern.h"

namespace lay 
{

// -----------------------------------------------------------------------------
//  ViewOp implementation

ViewOp::ViewOp () 
  : m_line_style_index (0), m_dither_index (0), m_dither_offset (0), m_shape (Rect), m_width (0), m_bitmap_index (0)
{ 
  init (0, Copy);
}

ViewOp::ViewOp (color_t color, Mode mode, unsigned int line_style_index, unsigned int dither_index, unsigned int dither_offset, Shape shape, int width, int bitmap_index)
  : m_line_style_index (line_style_index),
    m_dither_index (dither_index), m_dither_offset (dither_offset),
    m_shape (shape),
    m_width (width),
    m_bitmap_index (bitmap_index)
{ 
  init (color, mode);
}

void
ViewOp::init (color_t color, Mode mode)
{
  m_or  = (mode == Copy || mode == Or)  ? color : 0;
  m_and = (mode == Copy || mode == And) ? color : wordones;
  m_xor = (mode == Xor) ? color : 0;
}

} // namespace lay
