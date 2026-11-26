#ifndef HDR_laySaltGrainDetailsTextWidget
#define HDR_laySaltGrainDetailsTextWidget

#include "laySaltGrain.h"

#include <QTextBrowser>
#include <memory>

namespace lay
{

class SaltGrain;

/**
 *  @brief A specialization of QTextBrowser that displays the details of the salt grain
 */
class SaltGrainDetailsTextWidget
  : public QTextBrowser
{
Q_OBJECT

public:
  /**
   *  @brief Constructor
   */
  SaltGrainDetailsTextWidget (QWidget *w);

  /**
   *  @brief Sets the grain whose details are to be shown
   */
  void set_grain (const SaltGrain *g);

protected:
  virtual QVariant loadResource (int type, const QUrl &url);

private slots:
  void open_link (const QUrl &url);

private:
  std::unique_ptr<lay::SaltGrain> mp_grain;

  QString details_text ();
};

}

#endif
