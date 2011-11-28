
// GoogleTest
#include <gtest/gtest.h>

// BamTools unit tests
#include "test/api/BamAlignment/BamAlignmentDefaultTest.h"
#include "test/api/BamAlignment/BamAlignmentFlagTest.h"
#include "test/api/BamAux/BamAuxBamRegionTest.h"
#include "test/api/BamIndex/BamIndexFactoryTest.h"
#include "test/api/BamIndex/BamStandardIndexTest.h"
#include "test/api/BamMultiReader/BamMultiReaderFileOpsTest.h"
#include "test/api/BamReader/BamReaderFileOpsTest.h"
#include "test/api/SamHeader/SamHeaderValidatorTest.h"
#include "test/api/SamHeader/SamReadGroupDictionaryTest.h"
#include "test/api/SamHeader/SamSequenceDictionaryTest.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
