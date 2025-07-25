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

#include <gtest/gtest.h>
#include <memory>
#include <variant>
#include "native_calendar.h"
#include "calendar_log.h"
#include "native_calendar_manager.h"
#include "native_util.h"

namespace OHOS::CalendarApi::Native {
class CalendarTest : public testing::Test {
public:
    /* SetUpTestCase:The preset action of the test suite is executed before the first TestCase */
    static void SetUpTestSuite(void);

    /* TearDownTestCase:The test suite cleanup action is executed after the last TestCase */
    static void TearDownTestSuite(void);
    /* SetUp:Execute before each test case */
    void SetUp() {};

    /* TearDown:Execute after each test case */
    void TearDown() {};
    static std::shared_ptr<Calendar> calendar;
};

std::shared_ptr<Calendar> CalendarTest::calendar = nullptr;
const std::string TEST_NAME = "unit_test";
static CalendarAccount account {
    TEST_NAME,
    "local",
    "displayName_unit_test"
};

void CalendarTest::SetUpTestSuite(void)
{
    calendar = CalendarManager::GetInstance().GetCalendar(account);
    if (calendar != nullptr && calendar->GetAccount().name == TEST_NAME) {
        CalendarManager::GetInstance().DeleteCalendar(*calendar.get());
    }
    calendar = CalendarManager::GetInstance().CreateCalendar(account);
    ASSERT_TRUE(calendar != nullptr);
    LOG_INFO("SetUpTestCase SUCCESS");
}

void CalendarTest::TearDownTestSuite(void)
{
    // del calendar will del all event associated
    LOG_INFO("DeleteCalendar");
    auto ret = CalendarManager::GetInstance().DeleteCalendar(*calendar.get());
    ASSERT_TRUE(ret);
    LOG_INFO("TearDownTestSuite SUCCESS");
}

HWTEST_F(CalendarTest, AddEvent_test_basic, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "AddEvent_test_basic";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_FALSE(events.empty());
}

HWTEST_F(CalendarTest, AddEvent_test_title_is_Chinese, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "AddEvent_test_title_is_中文";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_FALSE(events.empty());
}

HWTEST_F(CalendarTest, DeleteEvent_dupilcate, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "DeleteEvent_dupilcate";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_FALSE(events.empty());
    auto ret = calendar->DeleteEvent(eventId);
    EXPECT_EQ(ret, true);
    ret = calendar->DeleteEvent(eventId);
    EXPECT_EQ(ret, false);
}

HWTEST_F(CalendarTest, AddEvents_test_normal, testing::ext::TestSize.Level1)
{
    const string title_1 = "AddEvents_test_normal_1";
    const string title_2 = "AddEvents_test_normal_2";
    Event event1;
    event1.title = title_1;
    Event event2;
    event2.title = title_2;
    auto count = calendar->AddEvents({event1, event2});
    EXPECT_EQ(count, 2);
    auto queryResult = calendar->GetEvents(FilterByTitle(title_1), {});
    ASSERT_EQ(1, queryResult.size());
    EXPECT_EQ(queryResult.at(0).title.value(), title_1);
    queryResult = calendar->GetEvents(FilterByTitle(title_2), {});
    ASSERT_EQ(1, queryResult.size());
    EXPECT_EQ(queryResult.at(0).title.value(), title_2);
}

HWTEST_F(CalendarTest, AddEvents_input_empty, testing::ext::TestSize.Level1)
{
    auto count = calendar->AddEvents({});
    ASSERT_EQ(count, 0);
}

HWTEST_F(CalendarTest, GetEvent_test_1, testing::ext::TestSize.Level1)
{
    Event event;
    const string title = "GetEvent_test_1";
    event.title = title;
    event.type = EventType::Important;
    Location testLocation {"test", 123.12, 45.45};
    event.location = std::make_optional<Location>(testLocation);
    event.startTime = 1766620800000;
    event.endTime = 1766620800000;
    event.isAllDay = true;
    event.attendees = {{"xiaoming", "xiaoming@abc.com", NONE, UNKNOWN, REQUIRED},
        {"xiaoqiang", "test_attendee1@abc.com", PARTICIPANT, TENTATIVE, OPTIONAL},
        {"abc", "test_attendee2@abc.com", ORGANIZER, ACCEPTED, RESOURCE}};
    event.timeZone = "UTC";
    event.reminderTime = {0, 1, 2};
    event.description = "UpdateEvent_test_2_disp";
    const auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    const auto events = calendar->GetEvents(FilterByTitle(title), {});
    ASSERT_EQ(1, events.size());
    const auto newEvent = events.at(0);
    EXPECT_EQ(newEvent.title, event.title);
    EXPECT_EQ(newEvent.type, event.type);
    EXPECT_EQ(newEvent.location, event.location);
    EXPECT_EQ(newEvent.startTime, event.startTime);
    EXPECT_EQ(newEvent.endTime, event.endTime);
    EXPECT_EQ(newEvent.isAllDay, event.isAllDay);
    EXPECT_EQ(newEvent.attendees, event.attendees);
    EXPECT_EQ(newEvent.timeZone, event.timeZone);
    EXPECT_EQ(newEvent.reminderTime, event.reminderTime);
    EXPECT_EQ(newEvent.description, event.description);
}

HWTEST_F(CalendarTest, UpdateEvent_test_1, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_test_1";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    string_view newTitle = "UpdateEvent_test_1_new";
    newEvent.title = newTitle;
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterByTitle(newTitle), {});
    ASSERT_EQ(1, events.size());
    EXPECT_EQ(events.at(0).title.value(), newTitle);
}

HWTEST_F(CalendarTest, UpdateEvent_DeleteLocation_test, testing::ext::TestSize.Level0)
{
    Event event;
    event.title = "UpdateEvent_Location";
    event.location = {"test", 123.12, 45.45};
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    newEvent.location = std::make_optional<Location>();
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newLocation = events.at(0).location.value();
    EXPECT_EQ(newLocation.location.has_value(), true);
    EXPECT_EQ(newLocation.location.value(), "");
    EXPECT_EQ(newLocation.longitude.has_value(), false);
    EXPECT_EQ(newLocation.latitude.has_value(), false);
}

HWTEST_F(CalendarTest, UpdateEvent_AddLocation_test, testing::ext::TestSize.Level0)
{
    Event event;
    event.title = "UpdateEvent_AddLocation_test";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    EXPECT_EQ(newEvent.location.has_value(), true);
    EXPECT_EQ(newEvent.location.value().location.has_value(), true);
    EXPECT_EQ(newEvent.location.value().longitude.has_value(), false);
    newEvent.location = {"test", 123.12, 45.45};
    auto ret = calendar->UpdateEvent(newEvent);
    
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newLocation = events.at(0).location.value();
    EXPECT_EQ(newLocation.location.value(), newEvent.location.value().location.value());
    EXPECT_EQ(newLocation.longitude.value(), newEvent.location.value().longitude.value());
    EXPECT_EQ(newLocation.latitude.value(), newEvent.location.value().latitude.value());
}

HWTEST_F(CalendarTest, UpdateEvent_DeleteRule, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_DeleteRule";
    RecurrenceRule recurrenceRule;
    recurrenceRule.recurrenceFrequency = YEARLY;
    recurrenceRule.daysOfWeek = {1, 3, 5};
    recurrenceRule.weeksOfMonth = {2, 3, 4};
    recurrenceRule.monthsOfYear = {6, 7, 8};
    event.recurrenceRule = std::make_optional<RecurrenceRule>(recurrenceRule);
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {"recurrenceRule"});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    newEvent.recurrenceRule.value().recurrenceFrequency = NORULE;
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {"recurrenceRule"});
    ASSERT_EQ(1, events.size());
    auto newRecurrenceRule = events.at(0).recurrenceRule.value();
    EXPECT_EQ(newRecurrenceRule.recurrenceFrequency, NORULE);
    EXPECT_EQ(newRecurrenceRule.daysOfWeek.has_value(), false);
    EXPECT_EQ(newRecurrenceRule.weeksOfMonth.has_value(), false);
    EXPECT_EQ(newRecurrenceRule.monthsOfYear.has_value(), false);
}

HWTEST_F(CalendarTest, UpdateEvent_AddRule, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_DeleteRule";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {"recurrenceRule"});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    EXPECT_EQ(newEvent.recurrenceRule.value().recurrenceFrequency, NORULE);
    RecurrenceRule recurrenceRule;
    recurrenceRule.recurrenceFrequency = YEARLY;
    recurrenceRule.daysOfWeek = {1, 3, 5};
    recurrenceRule.weeksOfMonth = {2, 3, 4};
    recurrenceRule.monthsOfYear = {6, 7, 8};
    newEvent.recurrenceRule = std::make_optional<RecurrenceRule>(recurrenceRule);

    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {"recurrenceRule"});
    ASSERT_EQ(1, events.size());
    auto newRecurrenceRule = events.at(0).recurrenceRule.value();
    EXPECT_EQ(newRecurrenceRule.recurrenceFrequency, YEARLY);
   
    for (int i = 0; i < newRecurrenceRule.weeksOfMonth.value().size(); i++) {
        EXPECT_EQ(newRecurrenceRule.daysOfWeek.value()[i], recurrenceRule.daysOfWeek.value()[i]);
        EXPECT_EQ(newRecurrenceRule.weeksOfMonth.value()[i], recurrenceRule.weeksOfMonth.value()[i]);
        EXPECT_EQ(newRecurrenceRule.monthsOfYear.value()[i], recurrenceRule.monthsOfYear.value()[i]);
    }
}

HWTEST_F(CalendarTest, UpdateEvent_Title, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_Title";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newEvent = events.at(0);
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterByTitle(newEvent.title.value()), {});
    ASSERT_EQ(1, events.size());
    EXPECT_EQ(events.at(0).title.value(), event.title.value());
}

HWTEST_F(CalendarTest, UpdateEvent_AddService, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_AddService";
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    EXPECT_EQ(events.at(0).service.has_value(), false);
    auto newEvent = events.at(0);
    EventService testService {
        "Meeting",
        "//testuri",
        "test_discription"
    };
    newEvent.service = std::make_optional<EventService>(testService);
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    auto newService = events.at(0).service.value();
    EXPECT_EQ(newService.type, testService.type);
    EXPECT_EQ(newService.uri, testService.uri);
    EXPECT_EQ(newService.description.value(), testService.description.value());
}

HWTEST_F(CalendarTest, UpdateEvent_DeleteService, testing::ext::TestSize.Level1)
{
    Event event;
    event.title = "UpdateEvent_DeleteService";
    EventService testService {
        "Meeting",
        "//testuri",
        "test_discription"
    };
    event.service = std::make_optional<EventService>(testService);
    auto eventId = calendar->AddEvent(event);
    ASSERT_NE(eventId, 0);
    auto events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    EXPECT_EQ(events.at(0).service.has_value(), true);
    auto addService = events.at(0).service.value();
    EXPECT_EQ(addService.type, testService.type);
    EXPECT_EQ(addService.uri, testService.uri);
    EXPECT_EQ(addService.description.value(), testService.description.value());
    auto newEvent = events.at(0);
    EventService testNewService {
        "Meeting",
        "",
        std::nullopt
    };
    newEvent.service = std::make_optional<EventService>(testNewService);
    auto ret = calendar->UpdateEvent(newEvent);
    EXPECT_EQ(ret, true);
    events = calendar->GetEvents(FilterById({eventId}), {});
    ASSERT_EQ(1, events.size());
    ASSERT_EQ(events.at(0).service.has_value(), true);
    auto newService = events.at(0).service.value();
    EXPECT_EQ(newService.type, testNewService.type);
    EXPECT_EQ(newService.uri, testNewService.uri);
    EXPECT_EQ(newService.description.has_value(), true);
    EXPECT_EQ(newService.description.value(), "");
}

HWTEST_F(CalendarTest, UpdateEvents_test_1, testing::ext::TestSize.Level1)
{
    Event event1;
    event1.title = "UpdateEvents_test_1";
    Event event2;
    event2.title = "UpdateEvents_test_2";
    auto count = calendar->AddEvents({event1, event2});
    EXPECT_EQ(count, 2);

    auto queryResult = calendar->GetEvents(FilterByTitle("UpdateEvents_test_1"), {});
    ASSERT_EQ(1, queryResult.size());

    auto newEvent1 = queryResult.at(0);
    newEvent1.title = "title_test_update_1_new";
    queryResult = calendar->GetEvents(FilterByTitle("UpdateEvents_test_2"), {});
    ASSERT_EQ(1, queryResult.size());
    auto newEvent2 = queryResult.at(0);
    newEvent2.title = "title_test_update_2_new";
    std::vector<Event> newEvents;
    newEvents.emplace_back(newEvent1);
    newEvents.emplace_back(newEvent2);
    count = calendar->UpdateEvents(newEvents);
    EXPECT_EQ(count, newEvents.size());
    auto delRet = calendar->DeleteEvents({newEvent1.id.value(), newEvent2.id.value()});
    ASSERT_EQ(delRet, newEvents.size());
}

HWTEST_F(CalendarTest, GetConfig_default_test, testing::ext::TestSize.Level1)
{
    auto config = calendar->GetConfig();
    EXPECT_TRUE(config.enableReminder.has_value());
    EXPECT_TRUE(std::get<1>(config.color) == 0);
}

HWTEST_F(CalendarTest, SetConfig_empty_param_test, testing::ext::TestSize.Level1)
{
    CalendarConfig config;
    auto ret = calendar->SetConfig(config);
    ASSERT_TRUE(ret);
    auto configExpect = calendar->GetConfig();
    EXPECT_TRUE(configExpect.enableReminder.has_value());
    EXPECT_TRUE(std::get<0>(config.color).empty());
}

HWTEST_F(CalendarTest, SetConfig_with_color_test, testing::ext::TestSize.Level1)
{
    CalendarConfig config;
    config.color = 0xaabbcc;
    config.enableReminder = false;
    ASSERT_TRUE(calendar->SetConfig(config));
    auto configExpect = calendar->GetConfig();
    EXPECT_EQ(config, configExpect);
    config.color = 0xaabbccdd;
    config.enableReminder = true;
    ASSERT_TRUE(calendar->SetConfig(config));
    configExpect = calendar->GetConfig();
    EXPECT_EQ(config, configExpect);
}

HWTEST_F(CalendarTest, UpdateEvent_test_3, testing::ext::TestSize.Level1)
{
    Event event;
    RecurrenceRule recurrenceRule;
    recurrenceRule.recurrenceFrequency = YEARLY;
    recurrenceRule.daysOfWeek = {1, 3, 5};
    recurrenceRule.weeksOfMonth = {2, 3, 4};
    recurrenceRule.monthsOfYear = {6, 7, 8};
    recurrenceRule.excludedDates = {1713672150000};
    event.recurrenceRule = std::make_optional<RecurrenceRule>(recurrenceRule);
    event.title = std::make_optional<std::string>("UpdateEvent_test_3");
    event.isLunar = std::make_optional<bool>(true);
    auto eventId = calendar->AddEvent(event);
    auto events = calendar->GetEvents(FilterById({eventId}), {"recurrenceRule", "identifier", "isLunar", "id"});
    ASSERT_NE(eventId, 0);
}

HWTEST_F(CalendarTest, AddEventInfoNoID, testing::ext::TestSize.Level1)
{
    Event event;
    RecurrenceRule recurrenceRule;
    recurrenceRule.recurrenceFrequency = YEARLY;
    recurrenceRule.daysOfWeek = {1, 3, 5};
    recurrenceRule.weeksOfMonth = {2, 3, 4};
    recurrenceRule.monthsOfYear = {6, 7, 8};
    recurrenceRule.excludedDates = {1713672150000};
    event.recurrenceRule = std::make_optional<RecurrenceRule>(recurrenceRule);
    int channelId = 0;
    int eventInfo = calendar->AddEventInfo(event, channelId);
    ASSERT_NE(eventInfo, 0);
}

HWTEST_F(CalendarTest, UpdateEventNoID, testing::ext::TestSize.Level1)
{
    Event event;
    bool isUpdataEvent = calendar->UpdateEvent(event);
    ASSERT_EQ(isUpdataEvent, 0);
}

HWTEST_F(CalendarTest, BuildValueEventIsLunar, testing::ext::TestSize.Level1)
{
    Event event;
    event.identifier = std::make_optional<std::string>("1111");
    event.isLunar = std::make_optional<bool>(true);
    DataShare::DataShareValuesBucket newShareValuesBucket;
    newShareValuesBucket.Put("identifier", event.identifier.value());
    newShareValuesBucket.Put("event_calendar_type", event.isLunar.value());
    auto shareValuesBucket = BuildValueEvent(event, 0, 0, false);
    auto itIdentifier = shareValuesBucket.valuesMap.find("identifier");
    auto *itIdentifierVal = std::get_if<std::string>(&itIdentifier->second);
    std::string identifierVal = *itIdentifierVal;
    auto itIsLunar = shareValuesBucket.valuesMap.find("event_calendar_type");
    auto *itIsLunarVal = std::get_if<bool>(&itIsLunar->second);
    bool isLunarVal = *itIsLunarVal;
    EXPECT_EQ(event.identifier.value(), identifierVal);
    EXPECT_EQ(event.isLunar.value(), isLunarVal);
}

}  // namespace OHOS::CalendarApi::Native
