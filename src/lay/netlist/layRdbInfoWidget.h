#ifndef HDR_rdbInfoWidget
#define HDR_rdbInfoWidget

#include <QTextBrowser>
#include <QImage>

#include "tlDeferredExecution.h"

namespace rdb
{

class InfoWidget
  : public QTextBrowser 
{
public:
  InfoWidget (QWidget *parent);

  virtual QVariant loadResource (int type, const QUrl &name);

  void set_image (const QImage &image);

private:
  QImage m_image;
  QImage m_overview_image;
};

}

#endif

