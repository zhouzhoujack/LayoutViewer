#ifndef HDR_layCursor
#define HDR_layCursor


class QCursor;

namespace lay
{

/**
 *  @brief A wrapper for the cursor definitions
 */
struct Cursor
{
  /**
   *  @brief An enumeration of all cursors defined
   */
  enum cursor_shape
  {
    keep = -2,
    none = -1,
    arrow = 0,
    up_arrow = 1,
    cross = 2,
    wait = 3,
    i_beam = 4,
    size_ver = 5,
    size_hor = 6,
    size_bdiag = 7,
    size_fdiag = 8,
    size_all = 9,
    blank = 10,
    split_v = 11,
    split_h = 12,
    pointing_hand = 13,
    forbidden = 14,
    whats_this = 15,
    busy = 16,
    open_hand = 17,
    closed_hand = 18
  };

  /**
   *  @brief Get the QCursor from the lay::cursor_shape enum
   */
  static QCursor qcursor (cursor_shape cursor);
};

}

#endif

