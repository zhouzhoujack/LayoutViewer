#include "layRdbInfoWidget.h"

#include <stdio.h>

namespace rdb
{

// -----------------------------------------------------------------------------
//  RdbInfoWidget implementation

InfoWidget::InfoWidget (QWidget *parent)
  : QTextBrowser (parent)
{
#if QT_VERSION >= 0x040300
  setOpenLinks (false);
#endif
}

void 
InfoWidget::set_image (const QImage &image)
{
  m_image = image;

  int overview_width = 200;

  int w = std::min (image.width (), overview_width);
  int h = (image.height () * w) / image.width ();

  m_overview_image = image.scaled (QSize (w, h), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QVariant 
InfoWidget::loadResource (int type, const QUrl &name)
{
  if (type == QTextDocument::ImageResource && name.isRelative () && name.path () == QString::fromUtf8 ("item.image")) {
    return QVariant (m_image);
  } else if (type == QTextDocument::ImageResource && name.isRelative () && name.path () == QString::fromUtf8 ("item.overview-image")) {
    return QVariant (m_overview_image);
  } else {
    return QTextBrowser::loadResource (type, name);
  }
}

}

