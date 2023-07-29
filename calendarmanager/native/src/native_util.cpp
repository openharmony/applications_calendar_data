/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "native_util.h"
#include "calendar_log.h"

namespace OHOS::CalendarApi::Native {
void DumpCalendarAccount(const CalendarAccount &account)
{
    LOG_DEBUG("account.name:%{public}s", account.name.c_str());
    LOG_DEBUG("account.type:%{public}s", account.type.c_str());
    LOG_DEBUG("account.displayName:%{public}s", account.displayName.value_or("").c_str());
}

void DumpEvent(const Event &event)
{
    LOG_DEBUG("id       :%{public}d", event.id.value_or(-1));
    LOG_DEBUG("type     :%{public}d", event.type);
    LOG_DEBUG("title    :%{public}s", event.title.value_or("[null]").c_str());
    if (event.location) {
        auto location = event.location.value();
        LOG_DEBUG("location.location  :%{public}s", location.location.value_or("[null]").c_str());
        LOG_DEBUG("location.longitude :%{public}d", location.longitude.value_or(-1));
        LOG_DEBUG("location.latitude  :%{public}d", location.latitude.value_or(-1));
    } else {
        LOG_DEBUG("location [null]");
    }
    LOG_DEBUG("title    :%{public}s", event.title.value_or("").c_str());
    LOG_DEBUG("start    :%{public}lu", event.start);
    LOG_DEBUG("end      :%{public}lu", event.end);
    LOG_DEBUG("isAllDay :%{public}d", event.isAllDay.value_or(0));

    for (const auto &attendee : event.attendees) {
        LOG_DEBUG("attendee.name   :%{public}s", attendee.name.c_str());
        LOG_DEBUG("attendee.email  :%{public}s", attendee.email.c_str());
    }

    LOG_DEBUG("timeZone     :%{public}s", event.timeZone.value_or("[null]").c_str());
    LOG_DEBUG("description  :%{public}s", event.description.value_or("[null]").c_str());
}

DataShare::DataShareValuesBucket BuildValueEvent(const Event &event, int calendarId)
{
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("calendar_id", calendarId);

    LOG_DEBUG("title %{public}s", event.title.value_or("").c_str());
    valuesBucket.Put("title", event.title.value_or(""));
    valuesBucket.Put("dtstart", event.start);
    valuesBucket.Put("dtend", event.end);
    LOG_DEBUG("description %{public}s", event.description.value_or("").c_str());

    valuesBucket.Put("description", event.description.value_or(""));
    valuesBucket.Put("eventTimezone", "Asia/Shanghai");
    valuesBucket.Put("allDay", event.isAllDay ? (event.isAllDay.value() ? 1 : 0) : 0);
    return valuesBucket;
}

DataShare::DataShareValuesBucket BuildAttendeeValue(const Attendee &attendee, int eventId)
{
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("event_id", eventId);
    valuesBucket.Put("attendeeName", attendee.name);
    LOG_DEBUG("attendeeName %{public}s", attendee.name.c_str());
    valuesBucket.Put("attendeeEmail", attendee.email);
    LOG_DEBUG("attendeeEmail %{public}s", attendee.email.c_str());
    return valuesBucket;
}

int GetValue(DataShareResultSetPtr &resultSet, string_view fieldName, int& out)
{
    int index = 0;
    auto ret = resultSet->GetColumnIndex(string(fieldName), index);
    if (ret != DataShare::E_OK) {
        return ret;
    }
    return resultSet->GetInt(index, out);
}

int GetIndexValue(const DataShareResultSetPtr &resultSet, int index, std::string& out)
{
    return resultSet->GetString(index, out);
}

int GetIndexValue(const DataShareResultSetPtr &resultSet, int index, int& out)
{
    return resultSet->GetInt(index, out);
}

int GetValue(DataShareResultSetPtr &resultSet, string_view fieldName, std::string& out)
{
    int index = 0;
    auto fieldNameStr = string(fieldName);
    auto ret = resultSet->GetColumnIndex(fieldNameStr, index);
    if (ret != DataShare::E_OK) {
        LOG_ERROR("GetValue [%{public}s] failed [%{public}d]", fieldNameStr.c_str(), ret);
        return ret;
    }
    return resultSet->GetString(index, out);
}


std::vector<std::shared_ptr<Calendar>> ResultSetToCalendars(DataShareResultSetPtr &resultSet)
{
    std::vector<std::shared_ptr<Calendar>> result;
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    LOG_INFO("GetRowCount is %{public}d", rowCount);
    if (rowCount == 0) {
        return result;
    }
    auto err = resultSet->GoToFirstRow();
    if (err != DataShare::E_OK) {
        LOG_INFO("Failed GoToFirstRow %{public}d", err);
        return result;
    }
    do {
        int idValue = -1;
        if (GetValue(resultSet, "_id", idValue) != DataShare::E_OK) {
            break;
        }
        LOG_DEBUG("id: %{public}d", idValue);
        std::string nameValue;
        if (GetValue(resultSet, "account_name", nameValue) != DataShare::E_OK) {
            break;
        }
        LOG_DEBUG("account_name: %{public}s", nameValue.c_str());
        std::string typeValue;
        if (GetValue(resultSet, "account_type", typeValue) != DataShare::E_OK) {
            break;
        }
        LOG_DEBUG("account_type: %{public}s", typeValue.c_str());

        std::string displayNameValue;
        GetValue(resultSet, "calendar_displayName", displayNameValue);
        LOG_DEBUG("calendar_displayName: %{public}s", displayNameValue.c_str());

        int canReminder = -1;
        GetValue(resultSet, "canReminder", canReminder);
        LOG_DEBUG("canReminder: %{public}d", canReminder);

        int colorValue = 0;
        GetValue(resultSet, "calendar_color", colorValue);
        LOG_DEBUG("calendar_color: %{public}d", colorValue);
        CalendarConfig config {canReminder, "todo"};
        CalendarAccount curAccount {nameValue, typeValue, displayNameValue};
        result.emplace_back(std::make_shared<Calendar>(curAccount, idValue));
    } while (resultSet->GoToNextRow() == DataShare::E_OK);
    return result;
}

int ResultSetToEvents(std::vector<Event> &events, DataShareResultSetPtr &resultSet,
    const std::vector<std::string>& columns)
{
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    LOG_INFO("GetRowCount is %{public}d", rowCount);
    if (rowCount <= 0) {
        return -1;
    }
    auto err = resultSet->GoToFirstRow();
    if (err != DataShare::E_OK) {
        LOG_ERROR("Failed GoToFirstRow %{public}d", err);
        return -1;
    }
    do {
        Event event;
        GetValueOptional(resultSet, "_id", event.id);
        GetValueOptional(resultSet, "title", event.title);
        GetValueOptional(resultSet, "description", event.description);
        GetValueOptional(resultSet, "eventTimezone", event.timeZone);
        events.emplace_back(event);
    } while (resultSet->GoToNextRow() == DataShare::E_OK);
    return 0;
}

int ResultSetToAttendees(std::vector<Attendee> &attendees, DataShareResultSetPtr &resultSet)
{
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    LOG_INFO("GetRowCount is %{public}d", rowCount);
    if (rowCount <= 0) {
        return -1;
    }
    auto err = resultSet->GoToFirstRow();
    if (err != DataShare::E_OK) {
        LOG_ERROR("Failed GoToFirstRow %{public}d", err);
        return -1;
    }
    do {
        Attendee attendee;
        GetValue(resultSet, "attendeeName", attendee.name);
        GetValue(resultSet, "attendeeEmail", attendee.email);
        attendees.emplace_back(attendee);
    } while (resultSet->GoToNextRow() == DataShare::E_OK);
    return 0;
}

bool IsValidHexString(const std::string& colorStr)
{
    if (colorStr.empty()) {
        return false;
    }
    for (char ch : colorStr) {
        if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
            continue;
        }
        return false;
    }
    return true;
}

bool ColorParse(const std::string& colorStr, uint32_t& colorValue)
{
    if (colorStr.empty()) {
        LOG_ERROR("color string is empty");
        return false;
    }

    if (colorStr[0] != '#') { // start with '#'
        LOG_ERROR("color string not start with #");
        return false;
    }

    std::string color = colorStr.substr(1);
    if (!IsValidHexString(color)) {
        return false;
    }
    char* ptr;
    colorValue = std::strtoul(color.c_str(), &ptr, 16); // 16 is convert hex string to number
    if (colorStr.size() == 7) { // 7 #RRGGBB: RRGGBB -> AARRGGBB
        colorValue |= 0xff000000;
        return true;
    }
    if (colorStr.size() == 9) { // 9 #AARRGGBB
        return true;
    }
    return false;
}
}