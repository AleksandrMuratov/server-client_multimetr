#include "multimetr.h"
#include <string_view>
#include <sstream>

namespace multimetr{

std::string RangeToString(const Range& range){
    std::ostringstream ss;
    ss << '[' << range.min << " ... " << range.max << ')';
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Range& range){
    os << RangeToString(range);
    return os;
}

bool operator==(const Range& lhs, const Range& rhs){
    //так как вычислений min и max не будет, то возможно сравнение без epsilon
    return lhs.min == rhs.min && lhs.max == rhs.max;
}

std::string_view ToStringView(StatusChannel status){
    using namespace std::literals;
    switch (status) {
    case StatusChannel::IDLE_STATE:
        return "idle_state"sv;
    case StatusChannel::ERROR_STATE:
        return "error_state"sv;
    case StatusChannel::MEASURE_STATE:
        return "measure_state"sv;
    case StatusChannel::BUSY_STATE:
        return "busy_state"sv;
    default:
        return ""sv;
    }
}

std::ostream& operator<<(std::ostream& os, const StatusChannel& status){
    os << ToStringView(status);
    return os;
}

}// namespace multimetr
