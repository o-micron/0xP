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

#include <Renderer/Metal/MSL/XPMSLParser.h>

#include <Renderer/Metal/metal_cpp.h>
#include <Utilities/XPLogger.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#pragma clang diagnostic pop

#include <sstream>

struct MTLDataTypeValues
{
    unsigned long sizeInBytes;
    unsigned long alignmentInBytes;
    const char*   name;
};

// ---------------------------------------------------------------------------------------------------
void
typePointerToStr(MTL::PointerType* pt, std::stringstream& ss, int level);
void
typeStructToStr(MTL::StructType* st, std::stringstream& ss, int level);
void
typeArrayToStr(MTL::ArrayType* at, std::stringstream& ss, int level);
void
basicTypeValues(MTL::DataType dt, MTLDataTypeValues& values);
void
basicTypeToStr(MTL::DataType dt, std::stringstream& ss, int level);
void
typeToStr(MTL::DataType dt, MTL::PointerType* pointer, std::stringstream& ss, int level);
void
typeToStr(MTL::DataType dt, MTL::ArrayType* array, std::stringstream& ss, int level);
void
typeToStr(MTL::DataType dt, MTL::BufferBinding* bufferBinding, std::stringstream& ss, int level);
void
typeToStr(MTL::DataType dt, MTL::StructMember* member, std::stringstream& ss, int level);
// ---------------------------------------------------------------------------------------------------

void
typePointerToStr(MTL::PointerType* pt, std::stringstream& ss, int level)
{
    typeToStr(pt->elementType(), pt, ss, level);
    ss << "*";
}

void
typeStructToStr(MTL::StructType* st, std::stringstream& ss, int level)
{
    unsigned long offset = 0;
    unsigned long bytes  = 0;
    ss << std::string(level, '\t') << "struct {\n";
    for (size_t i = 0; i < st->members()->count(); ++i) {
        MTL::StructMember* member = static_cast<MTL::StructMember*>(st->members()->object(i));
        ss << std::string(level, '\t');
        // check if there was a padding here and print the number of bytes padded !
        //
        typeToStr(member->dataType(), member, ss, level + 1);
        ss << " #" << member->offset();
        ss << "\n";
    }
    ss << std::string(level, '\t') << "}\n";
}

void
typeArrayToStr(MTL::ArrayType* at, std::stringstream& ss, int level)
{
    typeToStr(at->elementType(), at, ss, level);
    ss << "[" << at->arrayLength() << "]";
}

void
basicTypeValues(MTL::DataType dt, MTLDataTypeValues& values)
{
    switch (dt) {
        case MTL::DataType::DataTypeBool: values = { 1, 1, "bool" }; return;

        case MTL::DataType::DataTypeChar: values = { 1, 1, "char" }; return;
        case MTL::DataType::DataTypeUChar: values = { 1, 1, "uchar" }; return;

        case MTL::DataType::DataTypeShort: values = { 2, 2, "short" }; return;
        case MTL::DataType::DataTypeUShort: values = { 2, 2, "ushort" }; return;

        case MTL::DataType::DataTypeInt: values = { 4, 4, "int" }; return;
        case MTL::DataType::DataTypeUInt: values = { 4, 4, "uint" }; return;

        case MTL::DataType::DataTypeLong: values = { 8, 8, "long" }; return;
        case MTL::DataType::DataTypeULong: values = { 8, 8, "ulong" }; return;

        case MTL::DataType::DataTypeHalf: values = { 2, 2, "half" }; return;
        case MTL::DataType::DataTypeFloat: values = { 4, 4, "float" }; return;

        case MTL::DataType::DataTypeBool2: values = { 2, 2, "bool2" }; return;
        case MTL::DataType::DataTypeBool3: values = { 4, 4, "bool3" }; return;
        case MTL::DataType::DataTypeBool4: values = { 4, 4, "bool4" }; return;

        case MTL::DataType::DataTypeChar2: values = { 2, 2, "char2" }; return;
        case MTL::DataType::DataTypeUChar2: values = { 2, 2, "uchar2" }; return;
        case MTL::DataType::DataTypeChar3: values = { 4, 4, "char3" }; return;
        case MTL::DataType::DataTypeUChar3: values = { 4, 4, "uchar3" }; return;
        case MTL::DataType::DataTypeChar4: values = { 4, 4, "char4" }; return;
        case MTL::DataType::DataTypeUChar4: values = { 4, 4, "uchar4" }; return;

        case MTL::DataType::DataTypeShort2: values = { 4, 4, "short2" }; return;
        case MTL::DataType::DataTypeUShort2: values = { 4, 4, "ushort2" }; return;
        case MTL::DataType::DataTypeShort3: values = { 8, 8, "short3" }; return;
        case MTL::DataType::DataTypeUShort3: values = { 8, 8, "ushort3" }; return;
        case MTL::DataType::DataTypeShort4: values = { 8, 8, "short4" }; return;
        case MTL::DataType::DataTypeUShort4: values = { 8, 8, "ushort4" }; return;

        case MTL::DataType::DataTypeInt2: values = { 8, 8, "int2" }; return;
        case MTL::DataType::DataTypeUInt2: values = { 8, 8, "uint2" }; return;
        case MTL::DataType::DataTypeInt3: values = { 16, 16, "int3" }; return;
        case MTL::DataType::DataTypeUInt3: values = { 16, 16, "uint3" }; return;
        case MTL::DataType::DataTypeInt4: values = { 16, 16, "int4" }; return;
        case MTL::DataType::DataTypeUInt4: values = { 16, 16, "uint4" }; return;

        case MTL::DataType::DataTypeLong2: values = { 16, 16, "long2" }; return;
        case MTL::DataType::DataTypeULong2: values = { 16, 16, "ulong2" }; return;
        case MTL::DataType::DataTypeLong3: values = { 32, 32, "long3" }; return;
        case MTL::DataType::DataTypeULong3: values = { 32, 32, "ulong3" }; return;
        case MTL::DataType::DataTypeLong4: values = { 32, 32, "long4" }; return;
        case MTL::DataType::DataTypeULong4: values = { 32, 32, "ulong4" }; return;

        case MTL::DataType::DataTypeHalf2: values = { 4, 4, "half2" }; return;
        case MTL::DataType::DataTypeHalf3: values = { 8, 8, "half3" }; return;
        case MTL::DataType::DataTypeHalf4: values = { 8, 8, "half4" }; return;

        case MTL::DataType::DataTypeFloat2: values = { 8, 8, "float2" }; return;
        case MTL::DataType::DataTypeFloat3: values = { 16, 16, "float3" }; return;
        case MTL::DataType::DataTypeFloat4: values = { 16, 16, "float4" }; return;

        case MTL::DataType::DataTypeHalf2x2: values = { 8, 4, "half2x2" }; return;
        case MTL::DataType::DataTypeHalf2x3: values = { 16, 8, "half2x3" }; return;
        case MTL::DataType::DataTypeHalf2x4: values = { 16, 8, "half2x4" }; return;
        case MTL::DataType::DataTypeHalf3x2: values = { 12, 4, "half3x2" }; return;
        case MTL::DataType::DataTypeHalf3x3: values = { 24, 8, "half3x3" }; return;
        case MTL::DataType::DataTypeHalf3x4: values = { 24, 8, "half3x4" }; return;
        case MTL::DataType::DataTypeHalf4x2: values = { 16, 4, "half4x2" }; return;
        case MTL::DataType::DataTypeHalf4x3: values = { 32, 8, "half4x3" }; return;
        case MTL::DataType::DataTypeHalf4x4: values = { 32, 8, "half4x4" }; return;

        case MTL::DataType::DataTypeFloat2x2: values = { 16, 8, "float2x2" }; return;
        case MTL::DataType::DataTypeFloat2x3: values = { 32, 16, "float2x3" }; return;
        case MTL::DataType::DataTypeFloat2x4: values = { 32, 16, "float2x4" }; return;
        case MTL::DataType::DataTypeFloat3x2: values = { 24, 8, "float3x2" }; return;
        case MTL::DataType::DataTypeFloat3x3: values = { 48, 16, "float3x3" }; return;
        case MTL::DataType::DataTypeFloat3x4: values = { 48, 16, "float3x4" }; return;
        case MTL::DataType::DataTypeFloat4x2: values = { 32, 8, "float4x2" }; return;
        case MTL::DataType::DataTypeFloat4x3: values = { 64, 16, "float4x3" }; return;
        case MTL::DataType::DataTypeFloat4x4: values = { 64, 16, "float4x4" }; return;

        case MTL::DataType::DataTypeTexture: values = { 0, 0, "texture" }; return;
        case MTL::DataType::DataTypeSampler: values = { 0, 0, "sampler" }; return;
        case MTL::DataType::DataTypeR8Unorm: values = { 0, 0, "r8unorm" }; return;
        case MTL::DataType::DataTypeR8Snorm: values = { 0, 0, "r8snorm" }; return;
        case MTL::DataType::DataTypeR16Unorm: values = { 0, 0, "r16unorm" }; return;
        case MTL::DataType::DataTypeR16Snorm: values = { 0, 0, "r16snorm" }; return;
        case MTL::DataType::DataTypeRG8Unorm: values = { 0, 0, "rg8unorm" }; return;
        case MTL::DataType::DataTypeRG8Snorm: values = { 0, 0, "rg8snorm" }; return;
        case MTL::DataType::DataTypeRG16Unorm: values = { 0, 0, "rg16unorm" }; return;
        case MTL::DataType::DataTypeRG16Snorm: values = { 0, 0, "rg16snorm" }; return;
        case MTL::DataType::DataTypeRGBA8Unorm: values = { 0, 0, "rgba8unorm" }; return;
        case MTL::DataType::DataTypeRGBA8Unorm_sRGB: values = { 0, 0, "rgba8unorm_srgb" }; return;
        case MTL::DataType::DataTypeRGBA8Snorm: values = { 0, 0, "rgba8snorm" }; return;
        case MTL::DataType::DataTypeRGBA16Unorm: values = { 0, 0, "rgba16unorm" }; return;
        case MTL::DataType::DataTypeRGBA16Snorm: values = { 0, 0, "rgba16snorm" }; return;
        case MTL::DataType::DataTypeRGB10A2Unorm: values = { 0, 0, "rgb10a2unorm" }; return;
        case MTL::DataType::DataTypeRG11B10Float: values = { 0, 0, "rg11b10float" }; return;
        case MTL::DataType::DataTypeRGB9E5Float: values = { 0, 0, "rgb9e5float" }; return;
        case MTL::DataType::DataTypeRenderPipeline: values = { 0, 0, "renderpipeline" }; return;
        case MTL::DataType::DataTypeComputePipeline: values = { 0, 0, "computepipeline" }; return;
        case MTL::DataType::DataTypeIndirectCommandBuffer: values = { 0, 0, "indirectcommandbuffer" }; return;
        case MTL::DataType::DataTypeVisibleFunctionTable: values = { 0, 0, "visiblefunctiontable" }; return;
        case MTL::DataType::DataTypeIntersectionFunctionTable: values = { 0, 0, "intersectionfunctiontable" }; return;
        case MTL::DataType::DataTypePrimitiveAccelerationStructure:
            values = { 0, 0, "primitiveaccelerationstructure" };
            return;
        case MTL::DataType::DataTypeInstanceAccelerationStructure:
            values = { 0, 0, "instanceaccelerationstructure" };
            return;
        case MTL::DataType::DataTypeNone: values = { 0, 0, "void" }; return;
        default: values = { 0, 0, "unknown" }; return;
    };
}

void
basicTypeToStr(MTL::DataType dt, std::stringstream& ss, int level)
{
    MTLDataTypeValues v;
    basicTypeValues(dt, v);
    ss << std::string(level, '\t') << v.name << "<" << v.sizeInBytes << "><" << v.alignmentInBytes << ">";
}

void
typeToStr(MTL::DataType dt, MTL::PointerType* pt, std::stringstream& ss, int level)
{
    switch (dt) {
        case MTL::DataType::DataTypeStruct: typeStructToStr(pt->elementStructType(), ss, level); return;
        case MTL::DataType::DataTypeArray: typeArrayToStr(pt->elementArrayType(), ss, level); return;
        case MTL::DataType::DataTypePointer: XP_LOG(XPLoggerSeverityFatal, "Unreachable"); return;
        default: basicTypeToStr(dt, ss, level); return;
    };
}

void
typeToStr(MTL::DataType dt, MTL::ArrayType* at, std::stringstream& ss, int level)
{
    switch (dt) {
        case MTL::DataType::DataTypeStruct: typeStructToStr(at->elementStructType(), ss, level); return;
        case MTL::DataType::DataTypeArray: typeArrayToStr(at->elementArrayType(), ss, level); return;
        case MTL::DataType::DataTypePointer: typePointerToStr(at->elementPointerType(), ss, level); return;
        default: basicTypeToStr(dt, ss, level); return;
    };
}

void
typeToStr(MTL::DataType dt, MTL::BufferBinding* bufferBinding, std::stringstream& ss, int level)
{
    switch (dt) {
        case MTL::DataType::DataTypeStruct: typeStructToStr(bufferBinding->bufferStructType(), ss, level); return;
        case MTL::DataType::DataTypeArray: typePointerToStr(bufferBinding->bufferPointerType(), ss, level); return;
        case MTL::DataType::DataTypePointer: typePointerToStr(bufferBinding->bufferPointerType(), ss, level); return;
        default: basicTypeToStr(dt, ss, level); return;
    };
}

void
typeToStr(MTL::DataType dt, MTL::StructMember* member, std::stringstream& ss, int level)
{
    switch (dt) {
        case MTL::DataType::DataTypeStruct: typeStructToStr(member->structType(), ss, level); return;
        case MTL::DataType::DataTypeArray: typeArrayToStr(member->arrayType(), ss, level); return;
        case MTL::DataType::DataTypePointer: typePointerToStr(member->pointerType(), ss, level); return;
        default: basicTypeToStr(dt, ss, level); return;
    };
}

void
parseBufferBinding(MTL::Binding* binding, std::stringstream& ss)
{
    MTL::BufferBinding* bufferBinding = static_cast<MTL::BufferBinding*>(binding);
    ss << "\tname: " << bufferBinding->name()->cString(NS::StringEncoding::UTF8StringEncoding) << "\n";
    ss << "\talignment: " << bufferBinding->bufferAlignment() << "\n";
    ss << "\tsize: " << bufferBinding->bufferDataSize() << "\n";
    typeToStr(bufferBinding->bufferDataType(), bufferBinding, ss, 1);
}

void
parseMeshRenderingVertexShaderBindings(NS::Array* bindings)
{
    for (size_t i = 0; i < bindings->count(); ++i) {
        MTL::Binding* binding = reinterpret_cast<MTL::Binding*>(bindings->object(i));
        const char*   access  = [binding]() -> const char* {
            switch (binding->access()) {
                case MTL::ArgumentAccessReadOnly: return "read only";
                case MTL::ArgumentAccessWriteOnly: return "write only";
                case MTL::ArgumentAccessReadWrite: return "read write";
            }
        }();
        std::string type = [binding]() -> std::string {
            switch (binding->type()) {
                case MTL::BindingTypeBuffer: {
                    std::stringstream ss;
                    ss << "buffer {\n";
                    parseBufferBinding(binding, ss);
                    ss << "}\n";
                    return ss.str();
                }
                case MTL::BindingTypeImageblock: return "image block";
                case MTL::BindingTypeImageblockData: return "image block data";
                case MTL::BindingTypeInstanceAccelerationStructure: return "instance acceleration structure";
                case MTL::BindingTypeIntersectionFunctionTable: return "intersection function table";
                case MTL::BindingTypeObjectPayload: return "object payload";
                case MTL::BindingTypePrimitiveAccelerationStructure: return "primitive acceleration structure";
                case MTL::BindingTypeSampler: return "sampler";
                case MTL::BindingTypeTexture: return "texture";
                case MTL::BindingTypeThreadgroupMemory: return "thread group memory";
                case MTL::BindingTypeVisibleFunctionTable: return "visible function table";
            }
        }();
        XP_LOGV(XPLoggerSeverityInfo,
                "\tindex: %lu, access: %s, type: %s, name: %s, isArgumentBuffer: %s",
                binding->index(),
                access,
                type.c_str(),
                binding->name()->cString(NS::UTF8StringEncoding),
                binding->argument() ? "YES" : "NO");
    }
}

void
parseMeshRenderingFragmentShaderBindings(NS::Array* bindings)
{
    for (size_t i = 0; i < bindings->count(); ++i) {
        MTL::Binding* binding = reinterpret_cast<MTL::Binding*>(bindings->object(i));
        const char*   access  = [binding]() -> const char* {
            switch (binding->access()) {
                case MTL::ArgumentAccessReadOnly: return "read only";
                case MTL::ArgumentAccessWriteOnly: return "write only";
                case MTL::ArgumentAccessReadWrite: return "read write";
            }
        }();
        std::string type = [binding]() -> std::string {
            switch (binding->type()) {
                case MTL::BindingTypeBuffer: {
                    std::stringstream ss;
                    ss << "buffer {\n";
                    parseBufferBinding(binding, ss);
                    ss << "}\n";
                    return ss.str();
                }
                case MTL::BindingTypeImageblock: return "image block";
                case MTL::BindingTypeImageblockData: return "image block data";
                case MTL::BindingTypeInstanceAccelerationStructure: return "instance acceleration structure";
                case MTL::BindingTypeIntersectionFunctionTable: return "intersection function table";
                case MTL::BindingTypeObjectPayload: return "object payload";
                case MTL::BindingTypePrimitiveAccelerationStructure: return "primitive acceleration structure";
                case MTL::BindingTypeSampler: return "sampler";
                case MTL::BindingTypeTexture: return "texture";
                case MTL::BindingTypeThreadgroupMemory: return "thread group memory";
                case MTL::BindingTypeVisibleFunctionTable: return "visible function table";
            }
        }();
        XP_LOGV(XPLoggerSeverityInfo,
                "\tindex: %lu, access: %s, type: %s, name: %s",
                binding->index(),
                access,
                type.c_str(),
                binding->name()->cString(NS::UTF8StringEncoding));
    }
}

extern void
parseMeshRenderingShader(const char* shaderName, NS::Array* vertexBindings, NS::Array* fragmentBindings)
{
    XP_LOGV(XPLoggerSeverityInfo, "Vertex Shader Bindings <%s>:", shaderName);
    XP_LOGV(XPLoggerSeverityInfo, "Fragment Shader Bindings <%s>:", shaderName);
    parseMeshRenderingVertexShaderBindings(vertexBindings);
    parseMeshRenderingFragmentShaderBindings(fragmentBindings);
}
