#pragma once

#include <string>

namespace Tracer {

class StatusMessage {
public:
    StatusMessage() = delete;
    static void Set(const std::string& msg);
    static const std::string& Get();

private:
    inline static std::string s_message;
};

} 