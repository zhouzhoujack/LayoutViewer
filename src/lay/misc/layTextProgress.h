#ifndef HDR_layTextProgress
#define HDR_layTextProgress

#include "layProgress.h"
#include "detail/utilsProgress.h"

#include <string>

namespace lay
{

/**
 *  @brief A helper to show the progress in the terminal
 */
class TextProgress
  : public lay::ProgressBar
{
public:
  /**
   *  @brief Constructor
   *  @param verbosity The verbosity threshold above (or equal to) which the progress is printed
   */
  TextProgress (int verbosity);

  virtual void update_progress (geo::Progress *progress);
  virtual void show_progress_bar (bool show);

private:
  int m_verbosity;
  std::string m_progress_text, m_progress_value;
};

}

#endif
