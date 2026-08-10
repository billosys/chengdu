#include "pandapi/runtime/fixture.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Catch2 stays test-only for fixture seam checks", "[fixture][Catch2]")
{
  pandapi::runtime::FixtureSpec fixture;
  fixture.id = "catch2-test-only-fixture";
  fixture.stdout_expectation.role = pandapi::runtime::StreamRole::TaggedStatus;

  REQUIRE(pandapi::runtime::stream_role_name(fixture.stdout_expectation.role) ==
          "tagged_status");
  REQUIRE(pandapi::runtime::probe_safety_name(pandapi::runtime::ProbeSafety::CiSafe) ==
          "CI-safe");
}
