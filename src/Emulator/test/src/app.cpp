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

#include "../common/syscalls.h"
#include "../common/uart.h"

#include <stdio.h>

class Animal
{
  public:
    Animal() { printf("Animal::Animal()\n"); }
    virtual ~Animal() { printf("Animal::~Animal()\n"); }
};

class Cat : public Animal
{
  public:
    Cat()
      : Animal()
    {
        printf("Cat::Cat()\n");
    }
    virtual ~Cat() { printf("Cat::~Cat()\n"); }
};

class Car
{
  public:
    Car() { printf("Car::Car()\n"); }
    virtual ~Car() { printf("Car::~Car()\n"); }
};

static double StaticVariable = -57.0;

static Car car = Car();

int
main(int argc, char** argv)
{
#if defined(__riscv)
    #if __riscv_xlen == 32
    printf("RUNNING ON RV32\n");
    #elif __riscv_xlen == 64
    printf("RUNNING ON RV64\n");
    #elif __riscv_xlen == 128
    printf("RUNNING ON RV128\n");
    #endif
#endif

#ifdef __cplusplus
    printf("C++ PROGRAM\n");
#else
    printf("C PROGRAM\n");
#endif

    Animal* animal = new Cat();
    delete animal;

    printf("StaticVariable: %lf\n", StaticVariable);
    return 0;
}
