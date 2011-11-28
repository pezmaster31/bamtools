#ifndef BAMINDEX_FACTORYTEST_H
#define BAMINDEX_FACTORYTEST_H

#include "api/BamIndex.h"
#include "api/internal/BamIndexFactory_p.h"
#include "api/internal/BamStandardIndex_p.h"
#include "api/internal/BamToolsIndex_p.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>

TEST(BamIndex_FactoryTest, CreateIndexFilename) {

    const std::string bamFilename      = "dummy.bam";
    const std::string baiIndexFilename = BamTools::Internal::BamIndexFactory::CreateIndexFilename(bamFilename, BamTools::BamIndex::STANDARD);
    const std::string btiIndexFilename = BamTools::Internal::BamIndexFactory::CreateIndexFilename(bamFilename, BamTools::BamIndex::BAMTOOLS);

    EXPECT_EQ("dummy.bam.bai", baiIndexFilename);
    EXPECT_EQ("dummy.bam.bti", btiIndexFilename);
}

TEST(BamIndex_FactoryTest, FileExtension) {

    const std::string bamFilename      = "dummy.bam";
    const std::string baiIndexFilename = "dummy.bam.bai";
    const std::string btiIndexFilename = "path/to/dummy.bam.bti";

    const std::string bamExtension = BamTools::Internal::BamIndexFactory::FileExtension(bamFilename);
    const std::string baiExtension = BamTools::Internal::BamIndexFactory::FileExtension(baiIndexFilename);
    const std::string btiExtension = BamTools::Internal::BamIndexFactory::FileExtension(btiIndexFilename);

    EXPECT_EQ(".bam", bamExtension);
    EXPECT_EQ(".bai", baiExtension);
    EXPECT_EQ(".bti", btiExtension);

    // TODO: try some invalid values, just to make sure this is robust/safe enough
}

TEST(BamIndex_FactoryTest, CreateIndexFromFilename) {

    const std::string baiIndexFilename = "../data/ex1.bam.bai";
    const std::string btiIndexFilename = "../data/ex1.bam.bti";

    BamTools::BamIndex* bai = BamTools::Internal::BamIndexFactory::CreateIndexFromFilename(baiIndexFilename, 0);
    BamTools::BamIndex* bti = BamTools::Internal::BamIndexFactory::CreateIndexFromFilename(btiIndexFilename, 0);

    ASSERT_TRUE(bai != NULL);
    ASSERT_TRUE(bti != NULL);

    EXPECT_EQ(BamTools::BamIndex::STANDARD, bai->Type());
    EXPECT_EQ(BamTools::BamIndex::BAMTOOLS, bti->Type());

    delete bai;
    delete bti;

    std::cerr << "TestMessage 4" << std::endl;
}

TEST(BamIndex_FactoryTest, CreateIndexOfType) {

    BamTools::BamIndex* bai = BamTools::Internal::BamIndexFactory::CreateIndexOfType(BamTools::BamIndex::STANDARD, 0);
    BamTools::BamIndex* bti = BamTools::Internal::BamIndexFactory::CreateIndexOfType(BamTools::BamIndex::BAMTOOLS, 0);

    EXPECT_EQ(BamTools::BamIndex::STANDARD, bai->Type());
    EXPECT_EQ(BamTools::BamIndex::BAMTOOLS, bti->Type());

    delete bai;
    delete bti;
}

TEST(BamIndex_FactoryTest, FindIndexFilename) {

    // Both BAI & BTI are available
    const std::string bamFilename = "../data/ex1.bam";
    const std::string baiIndexFilename = BamTools::Internal::BamIndexFactory::FindIndexFilename(bamFilename, BamTools::BamIndex::STANDARD);
    const std::string btiIndexFilename = BamTools::Internal::BamIndexFactory::FindIndexFilename(bamFilename, BamTools::BamIndex::BAMTOOLS);
    EXPECT_EQ("../data/ex1.bam.bai", baiIndexFilename);
    EXPECT_EQ("../data/ex1.bam.bti", btiIndexFilename);

    // No BTI index available, both requests should fall back to BAI
    const std::string bamFilename2 = "../data/BCM.ceph.chr20.bam";
    const std::string baiIndexFilename2 = BamTools::Internal::BamIndexFactory::FindIndexFilename(bamFilename2, BamTools::BamIndex::STANDARD);
    const std::string btiIndexFilename2 = BamTools::Internal::BamIndexFactory::FindIndexFilename(bamFilename2, BamTools::BamIndex::BAMTOOLS);
    EXPECT_EQ("../data/BCM.ceph.chr20.bam.bai", baiIndexFilename2);
    EXPECT_EQ("../data/BCM.ceph.chr20.bam.bai", btiIndexFilename2);
}

#endif // BAMINDEX_FACTORYTEST_H
