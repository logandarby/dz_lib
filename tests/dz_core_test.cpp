#include <gtest/gtest.h>

extern "C" {
#include "dz_debug.c"
#include "dz_debug.h"
}

TEST(Errors, EachTrace) {
  DZ_TRACE("Trace");
  DZ_WARN("Warn");
  DZ_WARNNO("Warnno");
  DZ_INFO("Info");
  DZ_ERROR("Error");
  DZ_ERRNO("Error no");
  DZ_INFO("Info with arguments: %d %s", 4, "test");
  volatile bool falsevar = false;
  if (falsevar) {  // Ensure macro expansion is alright
    DZ_ASSERT(false, "FALSE ASSERTION");
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
