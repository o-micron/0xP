/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <utilities/XPBitArray.h>

class BitArrayTests : public ::testing::Test
{
  public:
    BitArrayTests()
      : bitArray(XPBitArray(1000000, false))
    {
    }

  protected:
    void SetUp() override { bitArray = XPBitArray(1000000, false); }
    void TearDown() override {}

    XPBitArray bitArray;
};

TEST_F(BitArrayTests, Constructor)
{
    for (size_t i = 0; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), false); }
}

TEST_F(BitArrayTests, SetAll)
{
    bitArray.setAll();
    for (size_t i = 0; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), true); }
}

TEST_F(BitArrayTests, ClearAll)
{
    bitArray.clearAll();
    for (size_t i = 0; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), false); }
}

TEST_F(BitArrayTests, SetBit)
{
    bitArray.clearAll();
    bitArray.setBit(0);
    EXPECT_EQ(bitArray.isSet(0), true);
    for (size_t i = 1; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), false); }
    for (size_t i = 0; i < bitArray.getSize(); ++i) { bitArray.setBit(i); }
    for (size_t i = 0; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), true); }
}

TEST_F(BitArrayTests, ClearBit)
{
    bitArray.setAll();
    bitArray.clearBit(0);
    EXPECT_EQ(bitArray.isSet(0), false);
    for (size_t i = 1; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), true); }
    for (size_t i = 0; i < bitArray.getSize(); ++i) { bitArray.clearBit(i); }
    for (size_t i = 0; i < bitArray.getSize(); ++i) { EXPECT_EQ(bitArray.isSet(i), false); }
}