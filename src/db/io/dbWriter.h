#ifndef HDR_dbWriter
#define HDR_dbWriter

#include "dbCommon.h"

#include "tlException.h"
#include "dbSaveLayoutOptions.h"

namespace tl 
{
  class OutputStream;
  class OutputStreamBase;
}

namespace db
{

class Layout;

/**
 *  @brief The generic writer base class
 */
class DB_PUBLIC WriterBase
{
public:
  /**
   *  @brief Constructor
   */
  WriterBase () { }

  /**
   *  @brief Destructor
   */
  virtual ~WriterBase () { }

  /**
   *  @brief Actually write the layout
   *  The layout is non-const since the writer may modify the meta information of the layout.
   */
  virtual void write (db::Layout &layout, tl::OutputStream &stream, const db::SaveLayoutOptions &options) = 0;
};

/**
 *  @brief A generic stream format writer
 */
class DB_PUBLIC Writer
{
public:
  typedef std::vector<MetaInfo> meta_info;
  typedef meta_info::const_iterator meta_info_iterator;

  /**
   *  @brief The constructor
   */
  Writer (const SaveLayoutOptions &options);

  /**
   *  @brief The destructor
   */
  ~Writer ();

  /**
   *  @brief The generic write method
   *  The layout is non-const since the writer may modify the meta information of the layout.
   */
  void write (db::Layout &layout, tl::OutputStream &stream);

  /**
   *  @brief True, if for this format a valid writer is provided
   */
  bool is_valid () const
  {
    return mp_writer != 0;
  }

private:
  WriterBase *mp_writer;
  db::SaveLayoutOptions m_options;
};

}

#endif

