
// GoogleTest
#include <gtest/gtest.h>

// BamTools unit tests
#include "test/api/BamToolsApiTest.h"
#include "test/toolkit/BamToolsToolkitTest.h"
#include "test/utils/BamToolsUtilsTest.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
