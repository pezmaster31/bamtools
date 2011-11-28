#ifndef SAMSEQUENCEDICTIONARY_TEST_H
#define SAMSEQUENCEDICTIONARY_TEST_H

#include "api/SamSequenceDictionary.h"
#include <gtest/gtest.h>
#include <vector>

TEST(SamSequenceDictionaryTest, Empty) {

    BamTools::SamSequenceDictionary sequences;
    EXPECT_TRUE(sequences.IsEmpty());
    EXPECT_EQ(0, sequences.Size());
}

TEST(SamSequenceDictionaryTest, AddOne) {

    BamTools::SamSequenceDictionary sequences;
    sequences.Add(BamTools::SamSequence());
    EXPECT_FALSE(sequences.IsEmpty());
    EXPECT_EQ(1, sequences.Size());
}

TEST(SamSequenceDictionaryTest, AddMulti) {

    std::vector<BamTools::SamSequence> groups;
    groups.push_back(BamTools::SamSequence("1", 100));
    groups.push_back(BamTools::SamSequence("2", 100));

    BamTools::SamSequenceDictionary sequences;
    sequences.Add(groups);
    EXPECT_FALSE(sequences.IsEmpty());
    EXPECT_EQ(2, sequences.Size());
}

TEST(SamSequenceDictionaryTest, AddDuplicate) {

    std::vector<BamTools::SamSequence> groups;
    groups.push_back(BamTools::SamSequence("1", 100));
    groups.push_back(BamTools::SamSequence("1", 100));

    // add duplicates through Add()
    BamTools::SamSequenceDictionary sequences;
    sequences.Add(groups);
    EXPECT_FALSE(sequences.IsEmpty());
    EXPECT_EQ(1, sequences.Size());     // duplicate should not get added

    // add duplicates through operator[]
    sequences.Clear();
    sequences["1"] = groups.at(0);
    sequences["1"] = groups.at(0);
    EXPECT_FALSE(sequences.IsEmpty());
    EXPECT_EQ(1, sequences.Size());     // duplicate should not get added
}

TEST(SamSequenceDictionaryTest, Clear) {

    // add a sequence
    BamTools::SamSequenceDictionary sequences;
    sequences.Add(BamTools::SamSequence());
    EXPECT_FALSE(sequences.IsEmpty());
    EXPECT_EQ(1, sequences.Size());;

    // test clear
    sequences.Clear();
    EXPECT_TRUE(sequences.IsEmpty());
    EXPECT_EQ(0, sequences.Size());
}

#endif // SAMSEQUENCEDICTIONARY_TEST_H
