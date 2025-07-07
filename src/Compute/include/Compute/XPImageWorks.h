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

#if defined(_WIN32) || defined(_WIN64)
    #pragma warning(disable : 4996)
    #define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void
save_as_ppm(const char* filename, uint8_t* texture, uint32_t width, uint32_t height)
{
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", width, height);

    fwrite(texture, sizeof(uint8_t), width * height * 3, file);

    fclose(file);
    printf("texture saved as %s\n", filename);
}