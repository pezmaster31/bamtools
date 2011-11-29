#ifndef BAMALIGNMENT_DEFAULTTEST_H
#define BAMALIGNMENT_DEFAULTTEST_H

#include "api/BamAlignment.h"
#include <gtest/gtest.h>

class BamAlignment_DefaultTest : public ::testing::Test {
    protected:
        BamTools::BamAlignment al;
};

TEST_F(BamAlignment_DefaultTest, DefaultValues) {
    EXPECT_EQ(-1, al.RefID);
    EXPECT_EQ(-1, al.Position);
    EXPECT_EQ(-1, al.MateRefID);
    EXPECT_EQ(-1, al.MatePosition);
    EXPECT_EQ(0,  al.InsertSize);
}

#endif // BAMALIGNMENT_DEFAULTTEST_H
