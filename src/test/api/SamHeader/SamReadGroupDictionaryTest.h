#ifndef SAMREADGROUPDICTIONARY_TEST_H
#define SAMREADGROUPDICTIONARY_TEST_H

#include "api/SamReadGroupDictionary.h"
#include <gtest/gtest.h>
#include <vector>

TEST(SamReadGroupDictionaryTest, Empty) {
    BamTools::SamReadGroupDictionary readGroups;
    EXPECT_TRUE(readGroups.IsEmpty());
    EXPECT_EQ(0, readGroups.Size());
}

TEST(SamReadGroupDictionaryTest, AddOne) {

    BamTools::SamReadGroupDictionary readGroups;
    readGroups.Add(BamTools::SamReadGroup());
    EXPECT_FALSE(readGroups.IsEmpty());
    EXPECT_EQ(1, readGroups.Size());
}

TEST(SamReadGroupDictionaryTest, AddMulti) {

    std::vector<BamTools::SamReadGroup> groups;
    groups.push_back(BamTools::SamReadGroup("1"));
    groups.push_back(BamTools::SamReadGroup("2"));

    BamTools::SamReadGroupDictionary readGroups;
    readGroups.Add(groups);
    EXPECT_FALSE(readGroups.IsEmpty());
    EXPECT_EQ(2, readGroups.Size());
}

TEST(SamReadGroupDictionaryTest, AddDuplicate) {

    std::vector<BamTools::SamReadGroup> groups;
    groups.push_back(BamTools::SamReadGroup("1"));
    groups.push_back(BamTools::SamReadGroup("1"));

    // add duplicates through Add()
    BamTools::SamReadGroupDictionary readGroups;
    readGroups.Add(groups);
    EXPECT_FALSE(readGroups.IsEmpty());
    EXPECT_EQ(1, readGroups.Size());    // duplicate should not get added

    // add duplicates through operator[]
    readGroups.Clear();
    readGroups["1"] = groups.at(0);
    readGroups["1"] = groups.at(0);
    EXPECT_FALSE(readGroups.IsEmpty());
    EXPECT_EQ(1, readGroups.Size());     // duplicate should not get added
}

TEST(SamReadGroupDictionaryTest, Clear) {

    // add a read group
    BamTools::SamReadGroupDictionary readGroups;
    readGroups.Add(BamTools::SamReadGroup());
    EXPECT_FALSE(readGroups.IsEmpty());
    EXPECT_EQ(1, readGroups.Size());;

    // test clear
    readGroups.Clear();
    EXPECT_TRUE(readGroups.IsEmpty());
    EXPECT_EQ(0, readGroups.Size());
}

#endif // SAMREADGROUPDICTIONARY_TEST_H
