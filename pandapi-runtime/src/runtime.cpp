#include "pandapi/runtime/runtime.hpp"

namespace pandapi::runtime {

std::string runtime_name()
{
  return "pandapi-runtime";
}

int runtime_abi_version() noexcept
{
  return 1;
}

}  // namespace pandapi::runtime
