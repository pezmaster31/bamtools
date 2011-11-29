#ifndef SAMHEADER_VALIDATIONTEST_H
#define SAMHEADER_VALIDATIONTEST_H

#include "api/SamHeader.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

TEST(SamHeader_ValidationTest, EmptyHeader) {

    BamTools::SamHeader header("");
    std::string headerText = header.ToString();
    ASSERT_TRUE(headerText.empty());
    EXPECT_TRUE(header.IsValid());
}

TEST(SamHeader_ValidationTest, Version) {

    BamTools::SamHeader header("");

    // empty - VALID HEADER
    header.Version = "";
    EXPECT_TRUE(header.IsValid());

    // normal - VALID HEADER
    header.Version = "1.1";
    EXPECT_TRUE(header.IsValid());
    header.Version = "11.1";
    EXPECT_TRUE(header.IsValid());
    header.Version = "1.11";

    // no period - INVALID HEADER
    header.Version = "11";
    EXPECT_FALSE(header.IsValid());

    // no major - INVALID HEADER
    header.Version = ".1";
    EXPECT_FALSE(header.IsValid());

    // invalid major - INVALID HEADER
    header.Version = "a.1";
    EXPECT_FALSE(header.IsValid());

    // no minor - INVALID HEADER
    header.Version = "1.";
    EXPECT_FALSE(header.IsValid());

    // invalid minor - INVALID HEADER
    header.Version = "1.a";
    EXPECT_FALSE(header.IsValid());
}

TEST(SamHeader_ValidationTest, GroupOrder) {

    BamTools::SamHeader header("");

    // empty - VALID HEADER
    header.GroupOrder = "";
    EXPECT_TRUE(header.IsValid());

    // normal group order- VALID HEADER
    header.GroupOrder = "none";
    EXPECT_TRUE(header.IsValid());
    header.GroupOrder = "query";
    EXPECT_TRUE(header.IsValid());
    header.GroupOrder = "reference";
    EXPECT_TRUE(header.IsValid());

    // invalid group order - INVALID HEADER
    header.GroupOrder = "fake";
    EXPECT_FALSE(header.IsValid());
}

TEST(SamHeader_ValidationTest, SortOrder) {

    BamTools::SamHeader header("");

    // empty - VALID HEADER
    header.SortOrder = "";
    EXPECT_TRUE(header.IsValid());

    // normal sort order- VALID HEADER
    header.SortOrder = "coordinate";
    EXPECT_TRUE(header.IsValid());
    header.SortOrder = "queryname";
    EXPECT_TRUE(header.IsValid());
    header.SortOrder = "unsorted";
    EXPECT_TRUE(header.IsValid());

    // invalid sort order - INVALID HEADER
    header.SortOrder = "fake";
    EXPECT_FALSE(header.IsValid());
}

TEST(SamHeader_ValidationTest, SequenceDictionary) {

    BamTools::SamHeader header("");
    BamTools::SamSequence sequence;

    // --------------------------------
    // empty - VALID HEADER

    header.Sequences.Clear();
    EXPECT_TRUE(header.IsValid());

    // --------------------------------
    // normal - VALID HEADER

    sequence.Name   = "MT";
    sequence.Length = "1000";
    header.Sequences.Add(sequence);
    EXPECT_TRUE(header.IsValid());

    // normal ( length == MIN )
    sequence.Name   = "MT";
    sequence.Length = "1";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_TRUE(header.IsValid());

    // normal ( length == MAX )
    sequence.Name = "MT";
    sequence.Length = "536870911";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_TRUE(header.IsValid());

    // ---------------------------------
    // missing required info - INVALID HEADER

    // missing ID
    sequence.Name = "";
    sequence.Length = "1000";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // missing length
    sequence.Name = "MT";
    sequence.Length = "";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // -------------------------------------
    // improper SN format - INVALID HEADER

    // starts with *
    sequence.Name = "*MT";
    sequence.Length = "1000";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // starts with =
    sequence.Name = "=MT";
    sequence.Length = "1000";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // -----------------------------------------
    // nonunique ID - INVALID HEADER

    // this behavior is blocked by SamSequenceDictionary
    // see SamSequenceDictionaryTest::AddDuplicateOk

    // --------------------------------------------
    // invalid length  - INVALID HEADER

    // length = 0 (MIN == 1)
    sequence.Name = "MT";
    sequence.Length = "0";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // length = -1
    sequence.Name = "MT";
    sequence.Length = "-1";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());

    // length = MAX + 1
    sequence.Name ="MT";
    sequence.Length = "536870912";
    header.Sequences.Clear();
    header.Sequences.Add(sequence);
    EXPECT_FALSE(header.IsValid());
}

TEST(SamHeader_ValidationTest, ReadGroupDictionary) {

    BamTools::SamHeader header("");
    BamTools::SamReadGroup readGroup;

    // ---------------------------------
    // empty - VALID HEADER

    header.ReadGroups.Clear();
    EXPECT_TRUE(header.IsValid());

    // ---------------------------------
    // normal - VALID HEADER

    readGroup.ID = "Group123";
    readGroup.PlatformUnit = "PU001";

    // technology: none
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: capillary
    readGroup.SequencingTechnology = "CAPILLARY";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: Helicos
    readGroup.SequencingTechnology = "HELICOS";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: Illumina
    readGroup.SequencingTechnology = "ILLUMINA";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: IonTorrent
    readGroup.SequencingTechnology = "IONTORRENT";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: 454
    readGroup.SequencingTechnology = "LS454";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: Pacific Bio
    readGroup.SequencingTechnology = "PACBIO";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // technology: Solid
    readGroup.SequencingTechnology = "SOLID";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_TRUE(header.IsValid());

    // ---------------------------------
    // missing ID - INVALID HEADER

    readGroup.ID = "";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_FALSE(header.IsValid());

    // -----------------------------------------
    // nonunique ID - INVALID HEADER

    // this behavior is blocked by SamReadGroupDictionary
    // see SamReadGroupDictionaryTest::AddDuplicateOk

    // ------------------------------------------
    // nonunique platform unit - INVALID HEADER

    BamTools::SamReadGroup dupPU("Group1");
    BamTools::SamReadGroup dupPU2("Group2");
    dupPU.PlatformUnit = "PU001";
    dupPU2.PlatformUnit = "PU001";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(dupPU);
    header.ReadGroups.Add(dupPU2);
    EXPECT_FALSE(header.IsValid());

    // ------------------------------------------------
    // invalid sequencing technology - INVALID HEADER

    readGroup.SequencingTechnology = "fake";
    header.ReadGroups.Clear();
    header.ReadGroups.Add(readGroup);
    EXPECT_FALSE(header.IsValid());
}

#endif // SAMHEADER_VALIDATIONTEST_H
