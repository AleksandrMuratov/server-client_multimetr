#pragma once

#include <iostream>
#include <array>
#include <cstdint>
#include <limits>
#include <cstdlib>
#include <random>
#include <cassert>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cmath>
#include <optional>

namespace multimetr{

struct Range{
    float max = 0.001;
    float min = 0.0000001;// минимальное разрешение измерения? насколько я понял
};

std::string RangeToString(const Range& range);

std::ostream& operator<<(std::ostream& os, const Range& range);

//оператор сравнения для тестов
bool operator==(const Range& lhs, const Range& rhs);

enum class StatusChannel{
    IDLE_STATE,
    ERROR_STATE,
    MEASURE_STATE,
    BUSY_STATE
};

std::string_view ToStringView(StatusChannel status);

std::ostream& operator<<(std::ostream& os, const StatusChannel& status);

template<typename BitDepth>
float ComputeVoltageFromADCValue(BitDepth value, Range range){
    return std::round(value / (std::numeric_limits<BitDepth>::max() / ((range.max - range.min) / range.min)))*range.min;
}

template<typename ADCBitDepth, size_t NumOfChannels = 2>
class Multimetr{
public:
    struct ResultRequest{
        float value = 0.0f;
        StatusChannel status;
    };

    Multimetr(){
        ADCBitDepth null_value = 0;
        std::fill(channels_.begin(), channels_.end(), null_value);
        std::fill(ranges_.begin(), ranges_.end(), Range{});
        std::fill(statuses_.begin(), statuses_.end(), StatusChannel::IDLE_STATE);
    }

    bool StartMeasure(size_t channel, std::ostream& log_out = std::clog){
        if(channel >= NumOfChannels || statuses_[channel] != StatusChannel::IDLE_STATE){
            return false;
        }
        statuses_[channel] = StatusChannel::MEASURE_STATE;
        PrintCurrentTime(log_out);
        log_out << "start_measure channel" << channel << '\n';
        return true;
    }

    bool StopMeasure(size_t channel, std::ostream& log_out = std::clog){
        if(channel >= NumOfChannels || statuses_[channel] == StatusChannel::IDLE_STATE){
            return false;
        }
        statuses_[channel] = StatusChannel::IDLE_STATE;
        PrintCurrentTime(log_out);
        log_out << "stop_measure channel" << channel << '\n';
        return true;
    }

    bool SetRange(size_t channel, Range range, std::ostream& log_out = std::clog){
        if(channel >= NumOfChannels){
            return false;
        }
        ranges_[channel] = range;
        PrintCurrentTime(log_out);
        log_out << "set_range " << range << " channel " << channel << '\n';
        return true;
    }

    std::optional<ResultRequest> GetResult(size_t channel, std::ostream& log_out = std::clog) {
        if(channel >= NumOfChannels){
            return std::nullopt;
        }
        if(statuses_[channel] == StatusChannel::IDLE_STATE){
            return ResultRequest{0.0f, statuses_[channel]};
        }
        const Range& range = ranges_[channel];
        ResultRequest result;
        statuses_[channel] = GetStatusIn(channel);
        channels_[channel] = GetDigitIn(channel);
        result.status = statuses_[channel];
        result.value = ComputeVoltageFromADCValue<ADCBitDepth>(channels_[channel], range);
        PrintCurrentTime(log_out);
        log_out << "get_result channel" << channel << " " << result.value << '\n';
        return result;
    }

    const std::array<ADCBitDepth, NumOfChannels>& GetValuesACD() const{
        return channels_;
    }

    const std::array<Range, NumOfChannels>& GetRanges() const{
        return ranges_;
    }

    const std::array<StatusChannel, NumOfChannels> GetStatuses() const{
        return statuses_;
    }

    const Range& GetRange(size_t channel) const{
        assert(channel < NumOfChannels);
        return ranges_[channel];
    }

    std::optional<StatusChannel> GetStatus(size_t channel) const{
        if(channel >= NumOfChannels){
            return std::nullopt;
        }
        else{
            return statuses_[channel];
        }
    }

private:
    static ADCBitDepth RandomNum(){
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<ADCBitDepth> dist;
        return dist(gen);
    }

    ADCBitDepth GetDigitIn([[maybe_unused]] size_t channel) const{
        return RandomNum();
    }

    StatusChannel GetStatusIn(size_t channel) const {
        ADCBitDepth random_num = RandomNum() % 10;
        if(random_num == 0){
            return StatusChannel::ERROR_STATE;
        }else if(random_num == 1){
            return StatusChannel::BUSY_STATE;
        }
        else{
            return StatusChannel::MEASURE_STATE;
        }
    }

    static void PrintCurrentTime(std::ostream& log_out = std::clog){
        auto start = std::chrono::system_clock::now();
        std::time_t cur_time = std::chrono::system_clock::to_time_t(start);
        log_out << std::ctime(&cur_time);
    }

    void PrintLog(std::ostream& log_out = std::clog) const{
        for(size_t i = 0; i < NumOfChannels; ++i){
            log_out << "channel " << i
                    << " range: " << ranges_[i]
                    << " value ADC: " << channels_[i]
                    << " status: " << statuses_[i] << '\n';
        }
    }

    std::array<ADCBitDepth, NumOfChannels> channels_;
    std::array<Range, NumOfChannels> ranges_;
    std::array<StatusChannel, NumOfChannels> statuses_;
};

}// namespace multimetr
