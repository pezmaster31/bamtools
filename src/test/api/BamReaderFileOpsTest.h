#ifndef BAMREADER_FILEOPSTEST_H
#define BAMREADER_FILEOPSTEST_H

#include "api/BamReader.h"
#include <gtest/gtest.h>

TEST(BamReaderFileOpsTest, Defaults) {

    BamTools::BamReader reader;
    EXPECT_FALSE(reader.IsOpen());
    EXPECT_FALSE(reader.HasIndex());
    EXPECT_EQ("", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0,  reader.GetReferenceCount());
}

TEST(BamReaderFileOpsTest, NonExistentFile) {

    BamTools::BamReader reader;
    EXPECT_FALSE(reader.Open("../data/fake.bam"));

    EXPECT_FALSE(reader.IsOpen());
    EXPECT_FALSE(reader.HasIndex());
    EXPECT_EQ("", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0,  reader.GetReferenceCount());
}

TEST(BamReaderFileOpsTest, EmptyFile) {

    BamTools::BamReader reader;
    EXPECT_FALSE(reader.Open("../data/empty.bam"));

    EXPECT_FALSE(reader.IsOpen());
    EXPECT_FALSE(reader.HasIndex());
    EXPECT_EQ("", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0,  reader.GetReferenceCount());
}

TEST(BamReaderFileOpsTest, NormalFile_NoIndexNoHeader) {

    BamTools::BamReader reader;
    EXPECT_TRUE(reader.Open("../data/ex1.bam"));

    EXPECT_TRUE(reader.IsOpen());
    EXPECT_FALSE(reader.HasIndex());
    EXPECT_EQ("../data/ex1.bam", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(2,  reader.GetReferenceCount());
}

TEST(BamReaderFileOpsTest, NormalFile_ExplicitIndexFilename) {

    BamTools::BamReader reader;
    EXPECT_TRUE(reader.Open("../data/ex1.bam"));

    EXPECT_TRUE(reader.IsOpen());
    EXPECT_FALSE(reader.HasIndex());
    EXPECT_EQ("../data/ex1.bam", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(2,  reader.GetReferenceCount());

    EXPECT_TRUE(reader.OpenIndex("../data/ex1.bam.bai"));

    EXPECT_TRUE(reader.IsOpen());
    EXPECT_TRUE(reader.HasIndex());
    EXPECT_EQ("../data/ex1.bam", reader.GetFilename());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(2,  reader.GetReferenceCount());
}

#endif // BAMREADER_FILEOPSTEST_H
