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

#include <Emulator/XPEmulatorCommon.h>
#include <Emulator/XPEmulatorElfLoader.h>
#include <Emulator/XPEmulatorProcessor.h>

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, const char** argv)
{
    if (argc != 2) {
        printf("Usage: XPLauncher <path to elfprogram>\n\n");
        return -1;
    }
    XPEmulatorProcessor* processor = (XPEmulatorProcessor*)malloc(sizeof(XPEmulatorProcessor));
    RiscvElfLoader*      loader    = xp_emulator_elf_loader_load(argv[1]);
    if (loader == NULL) {
        printf("ELF Program loading failed.\n");
        return -1;
    }
    xp_emulator_processor_initialize(processor);
    if (xp_emulator_processor_load_program(processor, loader) == 0) {
        printf("Program loaded successfully.\n");
        xp_emulator_processor_run(processor);
        printf("Program ended successfully.\n");
    } else {
        printf("Program loading failed.\n");
    }
    xp_emulator_processor_finalize(processor);
    free(processor);
    xp_emulator_elf_loader_unload(loader);
    return 0;
}
