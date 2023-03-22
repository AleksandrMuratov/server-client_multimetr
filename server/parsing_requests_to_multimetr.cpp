#include "parsing_requests_to_multimetr.h"

ParserOfRequestsToMultimetr::ParserOfRequestsToMultimetr():
    res_fail("fail")
    , res_ok("ok")
    , ranges(4, multimetr::Range{})
{
    ranges[0].min = 0.0000001f;
    ranges[0].max = 0.001f;
    ranges[1].min = 0.001f;
    ranges[1].max = 1.0f;
    ranges[2].min = 1.0f;
    ranges[2].max = 1000.0f;
    ranges[3].min = 1000.0f;
    ranges[3].max = 1000000.0f;
}

std::string_view LTrim(std::string_view str){
    size_t pos = 0;
    while(pos < str.size() && std::isspace(str[pos])){
        ++pos;
    }
    str.remove_prefix(pos);
    return str;
}

std::string_view SkipWord(std::string_view str, const std::string& word){
    using namespace std::literals;
    if(word != str.substr(0, word.size())){
        return ""sv;
    }
    str.remove_prefix(word.size());
    return str;
}

std::string_view GetWord(std::string_view& str){
    size_t pos = 0;
    while(pos < str.size() && !std::isspace(str[pos])){
        ++pos;
    }
    std::string_view res = str.substr(0, pos);
    str.remove_prefix(pos);
    return res;
}

std::string_view SkipSymbol(std::string_view str, char c){
    using namespace std::literals;
    if(!str.empty() && str.front() == c){
        str.remove_prefix(1);
        return str;
    }
    else{
        return ""sv;
    }
}

int GetNum(std::string_view& str){
    size_t pos = 0;
    while(pos < str.size() && std::isdigit(static_cast<unsigned char>(str[pos]))){
        ++pos;
    }
    std::string_view num = str.substr(0, pos);
    if(num.empty()){
        return -1;
    }
    str.remove_prefix(pos);
    try {
        return std::stoi(std::string(num));
    }  catch (const std::exception& ex) {
        std::clog << ex.what();
        return -1;
    }

}
