#include "layFixedFont.h"
#include "layFixedFontData.h"

#include <algorithm>

namespace lay
{

int FixedFont::ms_default_font_size = 0;

// -----------------------------------------------------------------------------
//  FixedFont implementation

FixedFont::FixedFont (unsigned int h, unsigned int lh, unsigned int w, unsigned char c0, unsigned char nc, uint32_t *d, unsigned int stride)
  : m_height (h), m_line_height (lh), m_width (w), m_first_char (c0), m_n_chars (nc), mp_data (d), m_stride (stride)
{
  // .. nothing yet ..
}

int
FixedFont::font_sizes ()
{
  return ff_sizes;
}

const char *
FixedFont::font_size_name (int sz)
{
  return ff_size_name (sz);
}

void 
FixedFont::set_default_font_size (int fs)
{
  ms_default_font_size = std::min (ff_sizes - 1, std::max (0, fs));
}

const FixedFont &
FixedFont::get_font (double resolution)
{
  int fs = ms_default_font_size;
  int od = std::max (1, std::min (ff_resolutions, int (1.0 / resolution + 0.5))) - 1;
  return fonts [od * ff_sizes + fs];
}

}

