/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NAPI_CALENDAR_LOG_H
#define NAPI_CALENDAR_LOG_H

#ifndef LOG_TAG
#define LOG_TAG
#endif

#include "hilog/log.h"

namespace OHOS::CalendarApi {
static const OHOS::HiviewDFX::HiLogLabel CALENDAR_LABEL = { LOG_APP, 0x050a, "CalendarApi" };

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Debug(CALENDAR_LABEL, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Info(CALENDAR_LABEL, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Warn(CALENDAR_LABEL, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Error(CALENDAR_LABEL, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Fatal(CALENDAR_LABEL, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
} // namespace OHOS::CalendarApi
#endif

