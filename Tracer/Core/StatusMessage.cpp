#include "Core/StatusMessage.hpp"

namespace Tracer {

const std::string& StatusMessage::Get() {
    return s_message;
}

void StatusMessage::Set(const std::string& msg) {
    s_message = msg;
}

}