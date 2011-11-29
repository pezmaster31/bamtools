#ifndef BAMALIGNMENT_FLAGTEST_H
#define BAMALIGNMENT_FLAGTEST_H

#include "api/BamAlignment.h"
#include <gtest/gtest.h>

class BamAlignment_FlagTest : public ::testing::Test {
    protected:
        BamTools::BamAlignment al;
};
        
TEST_F(BamAlignment_FlagTest, IsDuplicate) {
    al.SetIsDuplicate(true);
    EXPECT_TRUE(al.IsDuplicate());
    al.SetIsDuplicate(false);
    EXPECT_FALSE(al.IsDuplicate());
}

TEST_F(BamAlignment_FlagTest, IsFailedQC) {
    al.SetIsFailedQC(true);
    EXPECT_TRUE(al.IsFailedQC());
    al.SetIsFailedQC(false);
    EXPECT_FALSE(al.IsFailedQC());
}

TEST_F(BamAlignment_FlagTest, IsFirstMate) {
    al.SetIsFirstMate(true);
    EXPECT_TRUE(al.IsFirstMate());
    al.SetIsFirstMate(false);
    EXPECT_FALSE(al.IsFirstMate());
}

TEST_F(BamAlignment_FlagTest, IsMapped) {
    al.SetIsMapped(true);
    EXPECT_TRUE(al.IsMapped());
    al.SetIsMapped(false);
    EXPECT_FALSE(al.IsMapped());
}

TEST_F(BamAlignment_FlagTest, IsMateMapped) {
    al.SetIsMateMapped(true);
    EXPECT_TRUE(al.IsMateMapped());
    al.SetIsMateMapped(false);
    EXPECT_FALSE(al.IsMateMapped());
}

TEST_F(BamAlignment_FlagTest, IsMateReverseStrand) {
    al.SetIsMateReverseStrand(true);
    EXPECT_TRUE(al.IsMateReverseStrand());
    al.SetIsMateReverseStrand(false);
    EXPECT_FALSE(al.IsMateReverseStrand());
}

TEST_F(BamAlignment_FlagTest, IsPaired) {
    al.SetIsPaired(true);
    EXPECT_TRUE(al.IsPaired());
    al.SetIsPaired(false);
    EXPECT_FALSE(al.IsPaired());
}

TEST_F(BamAlignment_FlagTest, IsPrimaryAlignment) {
    al.SetIsPrimaryAlignment(true);
    EXPECT_TRUE(al.IsPrimaryAlignment());
    al.SetIsPrimaryAlignment(false);
    EXPECT_FALSE(al.IsPrimaryAlignment());
}

TEST_F(BamAlignment_FlagTest, IsProperPair) {
    al.SetIsProperPair(true);
    EXPECT_TRUE(al.IsProperPair());
    al.SetIsProperPair(false);
    EXPECT_FALSE(al.IsProperPair());
}

TEST_F(BamAlignment_FlagTest, IsReverseStrand) {
    al.SetIsReverseStrand(true);
    EXPECT_TRUE(al.IsReverseStrand());
    al.SetIsReverseStrand(false);
    EXPECT_FALSE(al.IsReverseStrand());
}

TEST_F(BamAlignment_FlagTest, IsSecondMate) {
    al.SetIsSecondMate(true);
    EXPECT_TRUE(al.IsSecondMate());
    al.SetIsSecondMate(false);
    EXPECT_FALSE(al.IsSecondMate());
}  

#endif // BAMALIGNMENT_FLAGTEST_H
