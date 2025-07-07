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

#pragma once

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <boost/dynamic_bitset.hpp>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

struct XPBitArray
{
    XPBitArray(size_t count, bool value)
      : bs(boost::dynamic_bitset<>(0))
    {
        resize(count, value);
    }

    void   resize(size_t count, bool value) { bs.resize(count, value); }
    void   setAll() { bs.set(0, bs.size(), true); }
    void   clearAll() { bs.set(0, bs.size(), false); }
    void   setBit(size_t index) { bs.set(index, true); }
    void   clearBit(size_t index) { bs.set(index, false); }
    void   flipBit(size_t index) { bs.flip(index); }
    bool   isSet(size_t index) const { return bs[index]; }
    size_t getSize() const { return bs.size(); }

  private:
    boost::dynamic_bitset<> bs;
};