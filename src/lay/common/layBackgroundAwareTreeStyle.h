#ifndef HDR_layBackgroundAwareTreeStyle
#define HDR_layBackgroundAwareTreeStyle

#include <QProxyStyle>

namespace lay
{

/**
 *  @brief A style tailoring the drawing of the branch indicator
 *  This proxy style is making the branch indicator a triangle and aware of the
 *  palette of the tree.
 *  The default Gtk style is not, hence making the background dark means the
 *  triangles become invisible.
 */
class BackgroundAwareTreeStyle
  : public QProxyStyle
{
public:
  BackgroundAwareTreeStyle (QStyle *org_style);
  void drawPrimitive (PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
};

} // namespace lay

#endif

