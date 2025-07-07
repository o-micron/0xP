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

#include <Utilities/XPMaths.h>
#include <Utilities/XPPlatforms.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#define max
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>
#undef max
#pragma clang diagnostic pop

#include <memory>

#define DX12_CHECK(hr) (((HRESULT)(hr)) == S_OK)

#if defined(XP_BUILD_DEBUG)
    #define DX12_ASSERT(hr)                                                                                            \
        if (!DX12_CHECK(hr)) {                                                                                         \
            XPDebugBreak();                                                                                            \
            assert(false);                                                                                             \
        }
#elif defined(XP_BUILD_RELWITHDEBINFO)
    #define DX12_ASSERT(hr)                                                                                            \
        if (!DX12_CHECK(hr)) {                                                                                         \
            XPDebugBreak();                                                                                            \
            exit(-1);                                                                                                  \
        }
#elif defined(XP_BUILD_RELEASE) || defined(XP_BUILD_MINSIZEREL)
    #define DX12_ASSERT(hr)                                                                                            \
        if (!DX12_CHECK(hr)) { exit(-1); }
#endif

#define SET_NAME(/*ID3D12Object**/ pObject, /*LPCWSTR*/ name)                                                          \
    {                                                                                                                  \
        pObject->SetName(name);                                                                                        \
    }

#define SET_NAME_INDEXED(/*ID3D12Object**/ pObject, /*LPCWSTR*/ name, /*UINT*/ index)                                  \
    {                                                                                                                  \
        WCHAR fullName[50];                                                                                            \
        if (swprintf_s(fullName, L"%s[%u]", name, index) > 0) { pObject->SetName(fullName); }                          \
    }

#define NAME_D3D12_OBJECT(x, name)            SET_NAME((x).Get(), name)
#define NAME_D3D12_OBJECT_INDEXED(x, name, n) SET_NAME_INDEXED((x)[n].Get(), name, n)

#define XP_DX12_BUFFER_COUNT 3

struct XPDX12Mesh;
class XPMeshAsset;

struct XPDX12MeshObject
{
    XPDX12MeshObject(XPDX12Mesh& mesh, const XPBoundingBox& boundingBox)
      : mesh(mesh)
      , boundingBox(boundingBox)
    {
    }

    XPDX12Mesh&   mesh;
    std::string   name;
    uint32_t      vertexOffset;
    uint32_t      indexOffset;
    uint32_t      numIndices;
    XPBoundingBox boundingBox;
};

struct XPDX12Mesh
{
    const XPMeshAsset*                     meshAsset = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW               vertexBufferView;
    Microsoft::WRL::ComPtr<ID3D12Resource> normalBuffer;
    D3D12_VERTEX_BUFFER_VIEW               normalBufferView;
    Microsoft::WRL::ComPtr<ID3D12Resource> uvBuffer;
    D3D12_VERTEX_BUFFER_VIEW               uvBufferView;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    D3D12_INDEX_BUFFER_VIEW                indexBufferView;
    std::vector<XPDX12MeshObject>          objects;
};

// --------------------------------------------------------------------------------------------------------------------
// Row-by-row memcpy
inline void
MemcpySubresource(_In_ const D3D12_MEMCPY_DEST*      pDest,
                  _In_ const D3D12_SUBRESOURCE_DATA* pSrc,
                  SIZE_T                             RowSizeInBytes,
                  UINT                               NumRows,
                  UINT                               NumSlices) noexcept
{
    for (UINT z = 0; z < NumSlices; ++z) {
        auto pDestSlice = static_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
        auto pSrcSlice  = static_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * LONG_PTR(z);
        for (UINT y = 0; y < NumRows; ++y) {
            memcpy(pDestSlice + pDest->RowPitch * y, pSrcSlice + pSrc->RowPitch * LONG_PTR(y), RowSizeInBytes);
        }
    }
}

//------------------------------------------------------------------------------------------------
// Row-by-row memcpy
inline void
MemcpySubresource(_In_ const D3D12_MEMCPY_DEST*      pDest,
                  _In_ const void*                   pResourceData,
                  _In_ const D3D12_SUBRESOURCE_INFO* pSrc,
                  SIZE_T                             RowSizeInBytes,
                  UINT                               NumRows,
                  UINT                               NumSlices) noexcept
{
    for (UINT z = 0; z < NumSlices; ++z) {
        auto pDestSlice = static_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
        auto pSrcSlice  = (static_cast<const BYTE*>(pResourceData) + pSrc->Offset) + pSrc->DepthPitch * ULONG_PTR(z);
        for (UINT y = 0; y < NumRows; ++y) {
            memcpy(pDestSlice + pDest->RowPitch * y, pSrcSlice + pSrc->RowPitch * ULONG_PTR(y), RowSizeInBytes);
        }
    }
}
//------------------------------------------------------------------------------------------------
// All arrays must be populated (e.g. by calling GetCopyableFootprints)
inline UINT64
UpdateSubresources(_In_ ID3D12GraphicsCommandList*                                       pCmdList,
                   _In_ ID3D12Resource*                                                  pDestinationResource,
                   _In_ ID3D12Resource*                                                  pIntermediate,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT                            FirstSubresource,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT         NumSubresources,
                   UINT64                                                                RequiredSize,
                   _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
                   _In_reads_(NumSubresources) const UINT*                               pNumRows,
                   _In_reads_(NumSubresources) const UINT64*                             pRowSizesInBytes,
                   _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA*             pSrcData) noexcept
{
    // Minor validation
    auto IntermediateDesc = pIntermediate->GetDesc();
    auto DestinationDesc  = pDestinationResource->GetDesc();
    if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
        IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset || RequiredSize > SIZE_T(-1) ||
        (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
         (FirstSubresource != 0 || NumSubresources != 1))) {
        return 0;
    }

    BYTE*   pData;
    HRESULT hr = pIntermediate->Map(0, nullptr, reinterpret_cast<void**>(&pData));
    if (FAILED(hr)) { return 0; }

    for (UINT i = 0; i < NumSubresources; ++i) {
        if (pRowSizesInBytes[i] > SIZE_T(-1)) return 0;
        D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset,
                                       pLayouts[i].Footprint.RowPitch,
                                       SIZE_T(pLayouts[i].Footprint.RowPitch) * SIZE_T(pNumRows[i]) };
        MemcpySubresource(
          &DestData, &pSrcData[i], static_cast<SIZE_T>(pRowSizesInBytes[i]), pNumRows[i], pLayouts[i].Footprint.Depth);
    }
    pIntermediate->Unmap(0, nullptr);

    if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
        pCmdList->CopyBufferRegion(
          pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
    } else {
        for (UINT i = 0; i < NumSubresources; ++i) {
            D3D12_TEXTURE_COPY_LOCATION Dst = {};
            Dst.pResource                   = pDestinationResource;
            Dst.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            Dst.PlacedFootprint             = {};
            Dst.SubresourceIndex            = i + FirstSubresource;

            D3D12_TEXTURE_COPY_LOCATION Src = {};
            Src.pResource                   = pIntermediate;
            Dst.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            Dst.PlacedFootprint             = pLayouts[i];

            pCmdList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
        }
    }
    return RequiredSize;
}

//------------------------------------------------------------------------------------------------
// All arrays must be populated (e.g. by calling GetCopyableFootprints)
inline UINT64
UpdateSubresources(_In_ ID3D12GraphicsCommandList*                                       pCmdList,
                   _In_ ID3D12Resource*                                                  pDestinationResource,
                   _In_ ID3D12Resource*                                                  pIntermediate,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT                            FirstSubresource,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT         NumSubresources,
                   UINT64                                                                RequiredSize,
                   _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
                   _In_reads_(NumSubresources) const UINT*                               pNumRows,
                   _In_reads_(NumSubresources) const UINT64*                             pRowSizesInBytes,
                   _In_ const void*                                                      pResourceData,
                   _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_INFO*             pSrcData) noexcept
{
    // Minor validation
    auto IntermediateDesc = pIntermediate->GetDesc();
    auto DestinationDesc  = pDestinationResource->GetDesc();
    if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
        IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset || RequiredSize > SIZE_T(-1) ||
        (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
         (FirstSubresource != 0 || NumSubresources != 1))) {
        return 0;
    }

    BYTE*   pData;
    HRESULT hr = pIntermediate->Map(0, nullptr, reinterpret_cast<void**>(&pData));
    if (FAILED(hr)) { return 0; }

    for (UINT i = 0; i < NumSubresources; ++i) {
        if (pRowSizesInBytes[i] > SIZE_T(-1)) return 0;
        D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset,
                                       pLayouts[i].Footprint.RowPitch,
                                       SIZE_T(pLayouts[i].Footprint.RowPitch) * SIZE_T(pNumRows[i]) };
        MemcpySubresource(&DestData,
                          pResourceData,
                          &pSrcData[i],
                          static_cast<SIZE_T>(pRowSizesInBytes[i]),
                          pNumRows[i],
                          pLayouts[i].Footprint.Depth);
    }
    pIntermediate->Unmap(0, nullptr);

    if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
        pCmdList->CopyBufferRegion(
          pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
    } else {
        for (UINT i = 0; i < NumSubresources; ++i) {
            D3D12_TEXTURE_COPY_LOCATION Dst = {};
            Dst.pResource                   = pDestinationResource;
            Dst.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            Dst.PlacedFootprint             = {};
            Dst.SubresourceIndex            = i + FirstSubresource;

            D3D12_TEXTURE_COPY_LOCATION Src = {};
            Src.pResource                   = pIntermediate;
            Dst.Type                        = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            Dst.PlacedFootprint             = pLayouts[i];

            pCmdList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
        }
    }
    return RequiredSize;
}

//------------------------------------------------------------------------------------------------
// Heap-allocating UpdateSubresources implementation
inline UINT64
UpdateSubresources(ID3D12Device*                                                 pDevice,
                   _In_ ID3D12GraphicsCommandList*                               pCmdList,
                   _In_ ID3D12Resource*                                          pDestinationResource,
                   _In_ ID3D12Resource*                                          pIntermediate,
                   UINT64                                                        IntermediateOffset,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT                    FirstSubresource,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
                   _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA*     pSrcData) noexcept
{
    UINT64 RequiredSize = 0;
    auto   MemToAlloc =
      static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
    if (MemToAlloc > SIZE_MAX) { return 0; }
    void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
    if (pMem == nullptr) { return 0; }
    auto pLayouts         = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
    auto pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
    auto pNumRows         = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

    auto Desc = pDestinationResource->GetDesc();
    pDevice->GetCopyableFootprints(&Desc,
                                   FirstSubresource,
                                   NumSubresources,
                                   IntermediateOffset,
                                   pLayouts,
                                   pNumRows,
                                   pRowSizesInBytes,
                                   &RequiredSize);
    pDevice->Release();

    UINT64 Result = UpdateSubresources(pCmdList,
                                       pDestinationResource,
                                       pIntermediate,
                                       FirstSubresource,
                                       NumSubresources,
                                       RequiredSize,
                                       pLayouts,
                                       pNumRows,
                                       pRowSizesInBytes,
                                       pSrcData);
    HeapFree(GetProcessHeap(), 0, pMem);
    return Result;
}

//------------------------------------------------------------------------------------------------
// Heap-allocating UpdateSubresources implementation
inline UINT64
UpdateSubresources(ID3D12Device*                                                 pDevice,
                   _In_ ID3D12GraphicsCommandList*                               pCmdList,
                   _In_ ID3D12Resource*                                          pDestinationResource,
                   _In_ ID3D12Resource*                                          pIntermediate,
                   UINT64                                                        IntermediateOffset,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT                    FirstSubresource,
                   _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
                   _In_ const void*                                              pResourceData,
                   _In_reads_(NumSubresources) D3D12_SUBRESOURCE_INFO*           pSrcData) noexcept
{
    UINT64 RequiredSize = 0;
    auto   MemToAlloc =
      static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
    if (MemToAlloc > SIZE_MAX) { return 0; }
    void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
    if (pMem == nullptr) { return 0; }
    auto pLayouts         = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
    auto pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
    auto pNumRows         = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

    auto Desc = pDestinationResource->GetDesc();
    pDevice->GetCopyableFootprints(&Desc,
                                   FirstSubresource,
                                   NumSubresources,
                                   IntermediateOffset,
                                   pLayouts,
                                   pNumRows,
                                   pRowSizesInBytes,
                                   &RequiredSize);
    pDevice->Release();

    UINT64 Result = UpdateSubresources(pCmdList,
                                       pDestinationResource,
                                       pIntermediate,
                                       FirstSubresource,
                                       NumSubresources,
                                       RequiredSize,
                                       pLayouts,
                                       pNumRows,
                                       pRowSizesInBytes,
                                       pResourceData,
                                       pSrcData);
    HeapFree(GetProcessHeap(), 0, pMem);
    return Result;
}

//------------------------------------------------------------------------------------------------
// Stack-allocating UpdateSubresources implementation
template<UINT MaxSubresources>
inline UINT64
UpdateSubresources(ID3D12Device*                                             pDevice,
                   _In_ ID3D12GraphicsCommandList*                           pCmdList,
                   _In_ ID3D12Resource*                                      pDestinationResource,
                   _In_ ID3D12Resource*                                      pIntermediate,
                   UINT64                                                    IntermediateOffset,
                   _In_range_(0, MaxSubresources) UINT                       FirstSubresource,
                   _In_range_(1, MaxSubresources - FirstSubresource) UINT    NumSubresources,
                   _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept
{
    UINT64                             RequiredSize = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[MaxSubresources];
    UINT                               NumRows[MaxSubresources];
    UINT64                             RowSizesInBytes[MaxSubresources];

    auto Desc = pDestinationResource->GetDesc();
    pDevice->GetCopyableFootprints(
      &Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
    pDevice->Release();

    return UpdateSubresources(pCmdList,
                              pDestinationResource,
                              pIntermediate,
                              FirstSubresource,
                              NumSubresources,
                              RequiredSize,
                              Layouts,
                              NumRows,
                              RowSizesInBytes,
                              pSrcData);
}

//------------------------------------------------------------------------------------------------
// Stack-allocating UpdateSubresources implementation
template<UINT MaxSubresources>
inline UINT64
UpdateSubresources(ID3D12Device*                                          pDevice,
                   _In_ ID3D12GraphicsCommandList*                        pCmdList,
                   _In_ ID3D12Resource*                                   pDestinationResource,
                   _In_ ID3D12Resource*                                   pIntermediate,
                   UINT64                                                 IntermediateOffset,
                   _In_range_(0, MaxSubresources) UINT                    FirstSubresource,
                   _In_range_(1, MaxSubresources - FirstSubresource) UINT NumSubresources,
                   _In_ const void*                                       pResourceData,
                   _In_reads_(NumSubresources) D3D12_SUBRESOURCE_INFO*    pSrcData) noexcept
{
    UINT64                             RequiredSize = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[MaxSubresources];
    UINT                               NumRows[MaxSubresources];
    UINT64                             RowSizesInBytes[MaxSubresources];

    auto Desc = pDestinationResource->GetDesc();
    pDevice->GetCopyableFootprints(
      &Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
    pDevice->Release();

    return UpdateSubresources(pCmdList,
                              pDestinationResource,
                              pIntermediate,
                              FirstSubresource,
                              NumSubresources,
                              RequiredSize,
                              Layouts,
                              NumRows,
                              RowSizesInBytes,
                              pResourceData,
                              pSrcData);
}
// --------------------------------------------------------------------------------------------------------------------