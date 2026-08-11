#pragma once

#include "pandapi/runtime/status.hpp"

#include <utility>
#include <variant>

namespace pandapi::runtime {

template <typename T> class StatusResult {
public:
  [[nodiscard]] static StatusResult success(T value)
  {
    return StatusResult{std::move(value)};
  }

  [[nodiscard]] static StatusResult failure(ProcessStatus status)
  {
    return StatusResult{status};
  }

  [[nodiscard]] bool has_value() const noexcept
  {
    return std::holds_alternative<T>(storage_);
  }

  [[nodiscard]] const T& value() const { return std::get<T>(storage_); }

  [[nodiscard]] T& value() { return std::get<T>(storage_); }

  [[nodiscard]] const ProcessStatus& status() const
  {
    return std::get<ProcessStatus>(storage_);
  }

private:
  explicit StatusResult(T value)
      : storage_{std::move(value)}
  {
  }
  explicit StatusResult(ProcessStatus status)
      : storage_{status}
  {
  }

  std::variant<T, ProcessStatus> storage_;
};

} // namespace pandapi::runtime
