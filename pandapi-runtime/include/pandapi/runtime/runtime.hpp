#pragma once

#include <string>

namespace pandapi::runtime {

[[nodiscard]] std::string runtime_name();
[[nodiscard]] int runtime_abi_version() noexcept;

}  // namespace pandapi::runtime
