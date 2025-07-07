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

#include <limits>
#include <stdint.h>

/// @brief A general purpose bitflag structure
template<typename T = uint64_t>
class XPBitFlag
{
  public:
    XPBitFlag()
      : _bits(0)
    {
    }

    XPBitFlag(T bits)
      : _bits(bits)
    {
    }

    /// @brief Returns the current bits value
    T getBits() const { return _bits; }

    /// @brief Checks whether the bitflag has exactly matching bits values
    /// @param bits the bits to check for
    bool is(T bits) const { return _bits == bits; }

    /// @brief Chckecs whether the bitflag has specific bits turned on
    /// @param bits the bits to check for
    bool has(T bits) const { return (_bits & bits) == bits; }

    /// @brief Checks whether the bitflag has any of these bits turned on
    /// @param bits the bits to check for
    bool hasAny(T bits) const { return (_bits & bits) > 0; }

    /// @brief Turns all bits off
    void clearAll() { _bits = 0; }

    /// @brief Turns all bits on
    void setAll() { _bits = std::numeric_limits<T>::max(); }

    /// @brief Turns some bits on
    /// @param bits the bits to be turned on
    void add(T bits) { _bits |= bits; }

    /// @brief Turns some bits off
    /// @param bits the bits to be turned off
    void remove(T bits) { _bits &= ~bits; }

  private:
    T _bits;
};