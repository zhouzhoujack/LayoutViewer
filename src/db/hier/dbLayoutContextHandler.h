#ifndef HDR_dbLayoutContextHandler
#define HDR_dbLayoutContextHandler

#include "tlExpression.h"
#include "dbLayout.h"
#include "dbCommon.h"

namespace db
{

/**
 * @brief  A layout providing a context for the expression evaluation
 */
class DB_PUBLIC LayoutContextHandler
  : public tl::ContextHandler
{
public:
  /**
   *  @brief Constructor: provides a context from a layout
   *  This object will provide bindings for <..> (layers) and <<..>> (cells) expressions.
   *  Plus it will provide a database unit value.
   */
  LayoutContextHandler (const db::Layout *layout);

  /**
   *  @brief The constructor with a non-const layout context
   *  In the non-const context, layers or cells will be created if they
   *  are not there yet. To enable non-const context, set the "can_modify"
   *  flag to true.
   */
  LayoutContextHandler (db::Layout *layout, bool can_modify);

  /**
   *  @brief Provide <..> bindings.
   */
  virtual tl::Variant eval_bracket (const std::string &content) const;

  /**
   *  @brief Provide <<..>> bindings.
   */
  virtual tl::Variant eval_double_bracket (const std::string &content) const;

  /**
   *  @brief Provide a database unit value
   */
  virtual double dbu () const
  {
    return mp_layout->dbu ();
  }

private:
  const db::Layout *mp_layout;
  db::Layout *mp_layout_nc;
};

}

#endif

