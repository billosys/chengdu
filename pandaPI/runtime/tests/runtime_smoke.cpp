#include "pandapi/runtime/runtime.hpp"

#include <string>

int main()
{
  if (pandapi::runtime::runtime_name() != std::string{"pandapi-runtime"}) {
    return 1;
  }

  if (pandapi::runtime::runtime_abi_version() != 1) {
    return 2;
  }

  return 0;
}
