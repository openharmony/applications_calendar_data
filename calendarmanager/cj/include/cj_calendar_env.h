/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef CJ_CALENDAR_ENV_H
#define CJ_CALENDAR_ENV_H

#include "ability.h"
#include "ability_context.h"
#include "singleton.h"

namespace OHOS::CalendarApi {
class CJCalendarEnv : public OHOS::Singleton<CJCalendarEnv> {
public:
    void Init(std::shared_ptr<OHOS::AbilityRuntime::Context> context);
    std::shared_ptr<OHOS::AbilityRuntime::Context> getContext();
private:
    std::shared_ptr<OHOS::AbilityRuntime::Context> m_context;
    bool hasInited = false;
};
} // namespace Calendar::CalendarApi
#endif