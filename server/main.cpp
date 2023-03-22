#include <iostream>
#include <sstream>
#include <iomanip>
#include "multimetr.h"
#include "test_runner.h"
#include "server_multimetr.h"
#include "parsing_requests_to_multimetr.h"

using namespace std;

void TestCtrMultimetr(){
    using namespace multimetr;
    Multimetr<uint16_t, 3> m;
    std::array<uint16_t, 3> expected_values = {0, 0, 0};
    std::array<Range, 3> expected_ranges = {
        Range{}, Range{}, Range{}};
    std::array<StatusChannel, 3> expected_statuses = {
        StatusChannel::IDLE_STATE,
        StatusChannel::IDLE_STATE,
        StatusChannel::IDLE_STATE};
    ASSERT_EQUAL(m.GetValuesACD(), expected_values);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);
}

void TestStartAndStopMeasure(){
    using namespace multimetr;
    Multimetr<uint16_t, 3> m;
    m.StartMeasure(0);
    std::array<Range, 3> expected_ranges = {
        Range{}, Range{}, Range{}};
    std::array<StatusChannel, 3> expected_statuses = {
        StatusChannel::MEASURE_STATE,
        StatusChannel::IDLE_STATE,
        StatusChannel::IDLE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    //повторный старт
    m.StartMeasure(0);
    expected_statuses = {
            StatusChannel::MEASURE_STATE,
            StatusChannel::IDLE_STATE,
            StatusChannel::IDLE_STATE};
        ASSERT_EQUAL(m.GetRanges(), expected_ranges);
        ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    m.StartMeasure(1);
    expected_statuses = {
            StatusChannel::MEASURE_STATE,
            StatusChannel::MEASURE_STATE,
            StatusChannel::IDLE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    m.StartMeasure(2);
    expected_statuses = {
            StatusChannel::MEASURE_STATE,
            StatusChannel::MEASURE_STATE,
            StatusChannel::MEASURE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    m.StopMeasure(1);
    expected_statuses = {
            StatusChannel::MEASURE_STATE,
            StatusChannel::IDLE_STATE,
            StatusChannel::MEASURE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    //повторный стоп
    m.StopMeasure(1);
    expected_statuses = {
            StatusChannel::MEASURE_STATE,
            StatusChannel::IDLE_STATE,
            StatusChannel::MEASURE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    m.StopMeasure(0);
    expected_statuses = {
            StatusChannel::IDLE_STATE,
            StatusChannel::IDLE_STATE,
            StatusChannel::MEASURE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);

    m.StopMeasure(2);
    expected_statuses = {
            StatusChannel::IDLE_STATE,
            StatusChannel::IDLE_STATE,
            StatusChannel::IDLE_STATE};
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
    ASSERT_EQUAL(m.GetStatuses(), expected_statuses);
}

void TestSetRange(){
    using namespace multimetr;
    Multimetr<uint16_t, 3> m;
    Range new_range;
    new_range.max = 1000;
    new_range.min = 1;
    std::array<Range, 3> expected_ranges = {
        new_range, Range{}, Range{}};
    m.SetRange(0, new_range);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);

    //повторная установка диапазона
    m.SetRange(0, new_range);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);

    new_range.max = 1;
    new_range.min = 0.001;
    expected_ranges = {
        new_range, Range{}, Range{}};
    m.SetRange(0, new_range);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);

    expected_ranges = {
        new_range, new_range, Range{}};
    m.SetRange(1, new_range);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);

    expected_ranges = {
        new_range, new_range, new_range};
    m.SetRange(2, new_range);
    ASSERT_EQUAL(m.GetRanges(), expected_ranges);
}

void TestComputeVoltageFromADCValue(){
    using namespace multimetr;
    Range range;
    float voltage = ComputeVoltageFromADCValue<uint16_t>(0, range);
    const float epsilon = range.min;
    ASSERT(std::abs(voltage - 0.0f) < epsilon);
    voltage = ComputeVoltageFromADCValue<uint16_t>(65535, range);
    ASSERT(std::abs(voltage - 0.0009999f) < epsilon);
}

void TestGetResult(){
    using namespace multimetr;
    Multimetr<uint16_t, 3> m;
    m.StartMeasure(0);
    for(int i = 0; i < 30; ++i){
        auto res = m.GetResult(0);
        const Range& range = m.GetRange(0);
        ASSERT(res.has_value());
        ASSERT(res->value < range.max);
    }
}

void TestsForMultimetr(){
    TestRunner tr;
    RUN_TEST(tr, TestCtrMultimetr);
    RUN_TEST(tr, TestStartAndStopMeasure);
    RUN_TEST(tr, TestSetRange);
    RUN_TEST(tr, TestComputeVoltageFromADCValue);
    RUN_TEST(tr, TestGetResult);
}

int main()
{
    //TestsForMultimetr();
    multimetr::Multimetr<uint16_t, 10> multimetr;
    m_server::ServerMultimetr server("/tmp/socket_multimetr");
    ParserOfRequestsToMultimetr parser;
    server.RunServer([&multimetr, &parser](std::string_view str){
        std::string ans = parser.Request(multimetr, str);
        return ans;
    });
}
