#ifndef BAMMULTIREADER_FILEOPSTEST_H
#define BAMMULTIREADER_FILEOPSTEST_H

#include "api/BamMultiReader.h"
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

TEST(BamMultiReaderFileOpsTest, Defaults) {

    BamTools::BamMultiReader reader;

    EXPECT_FALSE(reader.HasOpenReaders());
    EXPECT_FALSE(reader.HasIndexes());
    std::vector<std::string> filenames = reader.Filenames();
    EXPECT_EQ(0, (int)filenames.size());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0, reader.GetReferenceCount());
}

TEST(BamMultiReaderFileOpsTest, NonExistentFiles) {

    std::vector<std::string> inputFiles;
    inputFiles.push_back("../data/fake.bam");
    inputFiles.push_back("../data/fake2.bam");

    BamTools::BamMultiReader reader;
    EXPECT_FALSE(reader.Open(inputFiles));

    EXPECT_FALSE(reader.HasOpenReaders());
    EXPECT_FALSE(reader.HasIndexes());
    std::vector<std::string> filenames = reader.Filenames();
    EXPECT_EQ(0, (int)filenames.size());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0, reader.GetReferenceCount());
}

TEST(BamMultiReaderFileOpsTest, EmptyFiles) {

    std::vector<std::string> inputFiles;
    inputFiles.push_back("../data/empty.bam");
    inputFiles.push_back("../data/empty1.bam");
    inputFiles.push_back("../data/empty2.bam");

    BamTools::BamMultiReader reader;
    EXPECT_FALSE(reader.Open(inputFiles));

    EXPECT_FALSE(reader.HasOpenReaders());
    EXPECT_FALSE(reader.HasIndexes());
    std::vector<std::string> filenames = reader.Filenames();
    EXPECT_EQ(0, (int)filenames.size());
    EXPECT_EQ("", reader.GetHeaderText());
    EXPECT_EQ(0, reader.GetReferenceCount());
}

#endif // BAMMULTIREADER_FILEOPSTEST_H
