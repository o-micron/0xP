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

#include <stddef.h>

// clang-format off

#define XP_UNIMPLEMENTED(A) printf(A); assert(false);

#define XP_UNUSED(A) (void)(A);

#define XP_STRINGIFY(A) #A

#define XP_STRINGIFY_ENUM(A)          constexpr std::string_view A##Str = #A;
#define XP_STRINGIFY_ENUM_EXTENDED(A) constexpr std::string_view A##Str = #A;

// this is here because VA_ARGS is not in the standard yet ? wtf ?! yep, this is the truth !
// MSVC will expand the VA_ARGS incorrectly leaving the whole thing down below a mess !
// A quick fix is here
#define XP_EXPAND(X) X

#define XP_CONCATENATE(arg1, arg2)  XP_CONCATENATE1(arg1, arg2)
#define XP_CONCATENATE1(arg1, arg2) XP_CONCATENATE2(arg1, arg2)
#define XP_CONCATENATE2(arg1, arg2) arg1##arg2

#define XP_FOR_EACH_X_1(A, B, x, ...)  B(x)
#define XP_FOR_EACH_X_2(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_1(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_3(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_2(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_4(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_3(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_5(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_4(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_6(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_5(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_7(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_6(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_8(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_7(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_9(A, B, x, ...)  A(x) XP_EXPAND(XP_FOR_EACH_X_8(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_10(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_9(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_11(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_10(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_12(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_11(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_13(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_12(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_14(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_13(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_15(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_14(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_16(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_15(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_17(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_16(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_18(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_17(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_19(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_18(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_20(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_19(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_21(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_20(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_22(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_21(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_23(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_22(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_24(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_23(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_25(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_24(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_26(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_25(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_27(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_26(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_28(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_27(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_29(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_28(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_30(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_29(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_31(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_30(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_32(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_31(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_33(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_32(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_34(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_33(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_35(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_34(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_36(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_35(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_37(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_36(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_38(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_37(A, B, __VA_ARGS__))
#define XP_FOR_EACH_X_39(A, B, x, ...) A(x) XP_EXPAND(XP_FOR_EACH_X_38(A, B, __VA_ARGS__))

#define XP_FOR_EACH_X_RSEQ_N() 39, 38, 37, 36, 35, 34, 33, 32, 31, 30,                         \
                                29, 28, 27, 26, 25, 24, 23, 22, 21, 20,                         \
                                19, 18, 17, 16, 15, 14, 13, 12, 11, 10,                         \
                                9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define XP_FOR_EACH_X_ARG_N(    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,                    \
                                _10, _11, _12, _13, _14, _15, _16, _17, _18, _19,               \
                                _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,               \
                                _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, N, ...) N

#define XP_FOR_EACH_X_NARG_(...) XP_EXPAND(XP_FOR_EACH_X_ARG_N(__VA_ARGS__))
#define XP_FOR_EACH_X_NARG(...)  XP_EXPAND(XP_FOR_EACH_X_NARG_(__VA_ARGS__, XP_FOR_EACH_X_RSEQ_N()))
#define XP_FOR_EACH_X_(N, A, B, x, ...) XP_EXPAND(XP_CONCATENATE(XP_FOR_EACH_X_, N)(A, B, x, __VA_ARGS__))
#define XP_FOR_EACH_X(A, B, x, ...) XP_EXPAND(XP_FOR_EACH_X_(XP_FOR_EACH_X_NARG(x, __VA_ARGS__), A, B, x, __VA_ARGS__))

// clang-format on

template<typename T1, typename T2>
inline size_t constexpr XPOffsetOf(T1 T2::* member)
{
    T2 object{};
    return size_t(&(object.*member)) - size_t(&object);
}

#define XP_INITIAL_WINDOW_WIDTH  512
#define XP_INITIAL_WINDOW_HEIGHT 512

#define XP_FORCE_VSYNC true
