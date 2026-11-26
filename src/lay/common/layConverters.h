#ifndef HDR_layConverters
#define HDR_layConverters


#include <QColor>

namespace lay
{

/**
 *  @brief A color converter class for converting colors to strings and vice versa
 */
struct ColorConverter 
{
  std::string to_string (const QColor &c) const;
  void from_string (const std::string &s, QColor &c) const;
};

}

#endif

