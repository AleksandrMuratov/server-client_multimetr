#pragma once

#include <string>
#include <string_view>
#include <exception>
#include <vector>
#include "multimetr.h"

std::string_view LTrim(std::string_view str);
std::string_view SkipWord(std::string_view str, const std::string& word);
std::string_view GetWord(std::string_view& str);
std::string_view SkipSymbol(std::string_view str, char c);
int GetNum(std::string_view& str);

class ParserOfRequestsToMultimetr{
public:
    ParserOfRequestsToMultimetr();

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string RequestStartMeasure(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels);

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string RequestStopMeasure(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels);

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string RequestSetRange(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels);

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string RequestGetStatus(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels);

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string RequestGetResult(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels);

    template<typename ADCBitDepth, size_t NumOfChannels>
    std::string Request(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view request);

private:
    const std::string res_fail = "fail";
    const std::string res_ok = "ok";
    std::vector<multimetr::Range> ranges;
};

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::RequestStartMeasure(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels){
    using namespace std::literals;
    channels = LTrim(channels);
    while(!channels.empty()){
        channels = SkipWord(channels, "channel"s);
        int num = GetNum(channels);
        if(num == -1){
            return res_fail;
        }
        if(!m_metr.StartMeasure(num)){
            return res_fail;
        }
        channels = LTrim(channels);
        if(channels.empty()){
            break;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
    }
    return res_ok;
}

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::RequestStopMeasure(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels){
    using namespace std::literals;
    channels = LTrim(channels);
    while(!channels.empty()){
        channels = SkipWord(channels, "channel"s);
        int num = GetNum(channels);
        if(num == -1){
            return res_fail;
        }
        if(!m_metr.StopMeasure(num)){
            return res_fail;
        }
        channels = LTrim(channels);
        if(channels.empty()){
            break;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
    }
    return res_ok;
}

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::RequestSetRange(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels){
    using namespace std::literals;
    std::string result;
    bool is_set_range = false;
    channels = LTrim(channels);
    bool is_first = true;
    while(!channels.empty()){
        channels = SkipWord(channels, "channel"s);
        int num_ch = GetNum(channels);
        if(num_ch == -1){
            return res_fail;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
        channels = SkipWord(channels, "range"s);
        int num_range = GetNum(channels);
        if(num_range < 0 || num_range >= ranges.size()){
            return res_fail;
        }
        if(!m_metr.SetRange(num_ch, ranges[num_range])){
            return res_fail;
        }
        if(!is_first){
            result += ", "s;
        }
        is_first = false;
        result += "range" + std::to_string(num_range);
        channels = LTrim(channels);
        if(channels.empty()){
            break;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
    }
    return res_ok + " " + result;
}

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::RequestGetStatus(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels){
    using namespace std::literals;
    std::string result;
    bool is_set_range = false;
    channels = LTrim(channels);
    bool is_first = true;
    while(!channels.empty()){
        channels = SkipWord(channels, "channel"s);
        int num_ch = GetNum(channels);
        if(num_ch == -1){
            return res_fail;
        }
        auto status = m_metr.GetStatus(num_ch);
        if(!status){
            return res_fail;
        }
        if(!is_first){
            result += ", "s;
        }
        is_first = false;
        result += multimetr::ToStringView(*status);
        channels = LTrim(channels);
        if(channels.empty()){
            break;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
    }
    return res_ok + " " + result;
}

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::RequestGetResult(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view channels){
    using namespace std::literals;
    std::string result;
    bool is_set_range = false;
    channels = LTrim(channels);
    bool is_first = true;
    while(!channels.empty()){
        channels = SkipWord(channels, "channel"s);
        int num_ch = GetNum(channels);
        if(num_ch == -1){
            return res_fail;
        }
        auto request_res = m_metr.GetResult(num_ch);
        if(!request_res){
            return res_fail;
        }
        if(!is_first){
            result += ", "s;
        }
        is_first = false;
        result += std::to_string(request_res->value);
        channels = LTrim(channels);
        if(channels.empty()){
            break;
        }
        channels = SkipSymbol(channels, ',');
        channels = LTrim(channels);
    }
    return res_ok + " " + result;
}

template<typename ADCBitDepth, size_t NumOfChannels>
std::string ParserOfRequestsToMultimetr::Request(multimetr::Multimetr<ADCBitDepth, NumOfChannels>& m_metr, std::string_view request){
    using namespace std::literals;
    request = LTrim(request);
    std::string command_name(GetWord(request));
    if(command_name == "start_measure"s){
        return RequestStartMeasure<ADCBitDepth, NumOfChannels>(m_metr, request);
    }
    else if(command_name == "set_range"s){
        return RequestSetRange<ADCBitDepth, NumOfChannels>(m_metr, request);
    }
    else if(command_name == "stop_measure"s){
        return RequestStopMeasure<ADCBitDepth, NumOfChannels>(m_metr, request);
    }
    else if(command_name == "get_status"s){
        return RequestGetStatus<ADCBitDepth, NumOfChannels>(m_metr, request);
    }
    else if(command_name == "get_result"s){
        return RequestGetResult<ADCBitDepth, NumOfChannels>(m_metr, request);
    }
    else{
        return res_fail;
    }
}
