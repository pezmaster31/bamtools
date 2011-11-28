#ifndef BAMAUX_BAMREGIONTEST_H
#define BAMAUX_BAMREGIONTEST_H

#include "api/BamAux.h"
#include <gtest/gtest.h>

TEST(BamAux_BamRegionTest, DefaultValues) {
    BamTools::BamRegion region;
    EXPECT_EQ(-1, region.LeftRefID);
    EXPECT_EQ(-1, region.LeftPosition);
    EXPECT_EQ(-1, region.RightRefID);
    EXPECT_EQ(-1, region.RightPosition);
}

TEST(BamAux_BamRegionTest, IsNull) {
    
    BamTools::BamRegion empty;
    EXPECT_TRUE(empty.isNull());

    BamTools::BamRegion normal(1,100,2,200);
    EXPECT_FALSE(normal.isNull());
}

TEST(BamAux_BamRegionTest, Clear) {

    BamTools::BamRegion region(1,100);
    EXPECT_FALSE(region.isNull());
    region.clear();
    EXPECT_TRUE(region.isNull());
}

TEST(BamAux_BamRegionTest, IsLeftBoundSpecified) {

    // no left bound
    BamTools::BamRegion empty;
    EXPECT_FALSE(empty.isLeftBoundSpecified());

    // regular left bound
    BamTools::BamRegion region(1, 100);
    EXPECT_TRUE(region.isLeftBoundSpecified());

    // invalid left bound
    BamTools::BamRegion invalidID(-2);
    EXPECT_FALSE(invalidID.isLeftBoundSpecified());
    BamTools::BamRegion invalidPosition(1, -200);
    EXPECT_FALSE(invalidPosition.isLeftBoundSpecified());
}

#endif // BAMAUX_BAMREGIONTEST_H
