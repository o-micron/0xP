#pragma once

extern "C" void
downloadFile(const char* url);

extern "C" void
downloadTexture(const char* url);

extern "C" void
downloadMeshAsset(const char* url);

extern "C" void
getMappedTexture(const char* texturePath, unsigned char** data, int* width, int* height, int* channels);
