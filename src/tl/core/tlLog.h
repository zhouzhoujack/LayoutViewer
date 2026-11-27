#pragma once

//  日志实现已整体改用 toolkit::logkit（liblogkit.so），这里的 tlLog.cc 已删除。
//  本头做三件事：引入 logkit 的 LOGI/LOGW/LOGE/LOGF；把 logkit 的名字引入 namespace tl；
//  为 QString 补 to_string 特化。

#include <QString>
#include <string>

//  toolkit 0.1.3 起公共头统一改名为 lk* 前缀：tlLog.h -> detail/lkLog.h、
//  tlGLog.h -> lkGLog.h；原 detail/tlAssert.h 也随之变为 detail/lkHelper.h
//  （其中定义的是 lk_assert，不再和工程的 tl_assert 冲突）。
#include <toolkit/detail/lkLog.h>

//  LOGI / LOGW / LOGE / LOGF
#include <toolkit/lkGLog.h>

namespace toolkit { namespace logkit {

//  QString（tr () 的返回类型）没有 to_string () 成员，补一个模板特化。
template <> inline std::string to_string<QString> (const QString &s)
{
  return s.toStdString ();
}

} }

#include "tlCommon.h"
#include "tlString.h"
#include "tlThreads.h"
#include "tlObjectCollection.h"

//  恢复工程版 tl_assert（tl::assertion_failed）。
#undef tl_assert
#define tl_assert(COND) if (!(COND)) { \
  tl::assertion_failed (__FILE__, __LINE__, #COND); \
}

namespace tl {

using toolkit::logkit::Channel;
using toolkit::logkit::ChannelProxy;
using toolkit::logkit::LogTee;
using toolkit::logkit::ChannelEndl;
using toolkit::logkit::ChannelNoendl;

using toolkit::logkit::log;
using toolkit::logkit::info;
using toolkit::logkit::warn;
using toolkit::logkit::error;

using toolkit::logkit::verbosity;

using toolkit::logkit::endl;
using toolkit::logkit::noendl;

} // namespace tl
