#pragma once


#include "tlCommon.h"

#include <string>

#if defined(HAVE_QT)
# include <QString>
//  provides QObject for tr
# include <QObject>
#endif

/**
 *  @brief Generic tr function for non-Qt and Qt builds
 *
 *  当使用 layout-core 时（LAY_USES_LAYOUT_CORE），全局 tr() 由
 *  layout-core 的 detail/utilsInternational.h 提供（定义完全相同）。
 *  两处定义同时出现在同一个 TU 会报 redefinition，因此这里直接引用
 *  layout-core 的版本，保证全工程只有一份定义。
 */
#if defined(LAY_USES_LAYOUT_CORE)
# include "detail/utilsInternational.h"
#elif defined(HAVE_QT)
inline QString tr (const char *s)
{
  return QObject::tr (s);
}
#else
std::string TL_PUBLIC tr (const char *s);
#endif

namespace tl
{

#if defined(HAVE_QT)
/**
 *  @brief Convert a UTF8 std::string to a QString
 */
TL_PUBLIC QString to_qstring (const std::string &s);

/**
 *  @brief Convert a QString to a UTF8 std::string
 */
TL_PUBLIC std::string to_string (const QString &s);
#endif

/**
 *  @brief Dummy conversion for convenience and as non-Qt replacement for to_string (const QString &)
 */
inline const std::string &to_string (const std::string &s)
{
  return s;
}

#ifndef _WIN32
/**
 *  @brief Convert a system encoding std::string to a UTF8 std::string
 */
TL_PUBLIC std::string system_to_string (const std::string &s);

/**
 *  @brief Convert a UTF8 string to a system encoding string
 */
TL_PUBLIC std::string string_to_system (const std::string &s);
#endif

/**
 *  @brief Initialize the codecs
 */
TL_PUBLIC void initialize_codecs ();

}


