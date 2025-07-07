#ifdef _WIN32
    #define _CRT_SECURE_NO_WARNINGS
#endif
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables for testing
int   global_int      = 42;
float global_float    = 3.14f;
char  global_string[] = "Hello, World!";

// Function prototypes
int
add(int a, int b);
int
factorial(int n);
void
test_arithmetic();
void
test_logical();
void
test_bitwise();
void
test_comparison();
void
test_memory();
void
test_control_flow();
void
test_functions();
void
test_pointers();
void
test_arrays();
void
test_strings();
void
test_floating_point();
void
test_type_conversions();

// Test arithmetic instructions
void
test_arithmetic()
{
    printf("Testing Arithmetic Instructions:\n");

    int a = 10, b = 3;
    printf("  Addition: %d + %d = %d\n", a, b, a + b);
    printf("  Subtraction: %d - %d = %d\n", a, b, a - b);
    printf("  Multiplication: %d * %d = %d\n", a, b, a * b);
    printf("  Division: %d / %d = %d\n", a, b, a / b);
    printf("  Modulo: %d %% %d = %d\n", a, b, a % b);

    // Unary operators
    printf("  Increment: ++%d = %d\n", a, ++a);
    printf("  Decrement: --%d = %d\n", a, --a);
    printf("  Negation: -%d = %d\n", a, -a);

    printf("\n");
}

// Test logical instructions
void
test_logical()
{
    printf("Testing Logical Instructions:\n");

    int x = 1, y = 0;
    printf("  Logical AND: %d && %d = %d\n", x, y, x && y);
    printf("  Logical OR: %d || %d = %d\n", x, y, x || y);
    printf("  Logical NOT: !%d = %d\n", x, !x);

    printf("\n");
}

// Test bitwise instructions
void
test_bitwise()
{
    printf("Testing Bitwise Instructions:\n");

    unsigned int a = 0xF0, b = 0x0F;
    printf("  Bitwise AND: 0x%X & 0x%X = 0x%X\n", a, b, a & b);
    printf("  Bitwise OR: 0x%X | 0x%X = 0x%X\n", a, b, a | b);
    printf("  Bitwise XOR: 0x%X ^ 0x%X = 0x%X\n", a, b, a ^ b);
    printf("  Bitwise NOT: ~0x%X = 0x%X\n", a, ~a);
    printf("  Left shift: 0x%X << 2 = 0x%X\n", a, a << 2);
    printf("  Right shift: 0x%X >> 2 = 0x%X\n", a, a >> 2);

    printf("\n");
}

// Test comparison instructions
void
test_comparison()
{
    printf("Testing Comparison Instructions:\n");

    int a = 5, b = 10;
    printf("  Equal: %d == %d = %d\n", a, b, a == b);
    printf("  Not equal: %d != %d = %d\n", a, b, a != b);
    printf("  Less than: %d < %d = %d\n", a, b, a < b);
    printf("  Less or equal: %d <= %d = %d\n", a, b, a <= b);
    printf("  Greater than: %d > %d = %d\n", a, b, a > b);
    printf("  Greater or equal: %d >= %d = %d\n", a, b, a >= b);

    printf("\n");
}

// Test memory instructions
void
test_memory()
{
    printf("Testing Memory Instructions:\n");

    // Stack allocation
    int local_var = 100;
    printf("  Local variable: %d\n", local_var);

    // Dynamic allocation
    int* ptr = (int*)malloc(sizeof(int));
    *ptr     = 200;
    printf("  Heap allocation: %d\n", *ptr);
    free(ptr);

    // Global access
    printf("  Global variable: %d\n", global_int);

    // Array access
    int arr[5] = { 1, 2, 3, 4, 5 };
    printf("  Array element: arr[2] = %d\n", arr[2]);

    printf("\n");
}

// Test control flow instructions
void
test_control_flow()
{
    printf("Testing Control Flow Instructions:\n");

    // If-else
    int x = 10;
    if (x > 5) {
        printf("  If statement: x is greater than 5\n");
    } else {
        printf("  Else statement: x is not greater than 5\n");
    }

    // Switch
    switch (x % 3) {
        case 0: printf("  Switch: x is divisible by 3\n"); break;
        case 1: printf("  Switch: x mod 3 = 1\n"); break;
        default: printf("  Switch: x mod 3 = 2\n"); break;
    }

    // For loop
    printf("  For loop: ");
    for (int i = 0; i < 5; i++) { printf("%d ", i); }
    printf("\n");

    // While loop
    printf("  While loop: ");
    int i = 0;
    while (i < 3) {
        printf("%d ", i);
        i++;
    }
    printf("\n");

    // Do-while loop
    printf("  Do-while loop: ");
    i = 0;
    do {
        printf("%d ", i);
        i++;
    } while (i < 3);
    printf("\n");

    printf("\n");
}

// Test function calls
void
test_functions()
{
    printf("Testing Function Calls:\n");

    int result = add(5, 3);
    printf("  Function call: add(5, 3) = %d\n", result);

    int fact = factorial(5);
    printf("  Recursive function: factorial(5) = %d\n", fact);

    printf("\n");
}

// Test pointer operations
void
test_pointers()
{
    printf("Testing Pointer Operations:\n");

    int  x   = 42;
    int* ptr = &x;

    printf("  Address of x: %p\n", (void*)&x);
    printf("  Value of ptr: %p\n", (void*)ptr);
    printf("  Dereference ptr: *ptr = %d\n", *ptr);

    // Pointer arithmetic
    int  arr[] = { 10, 20, 30, 40 };
    int* p     = arr;
    printf("  Pointer arithmetic: *(p+2) = %d\n", *(p + 2));

    printf("\n");
}

// Test array operations
void
test_arrays()
{
    printf("Testing Array Operations:\n");

    int numbers[5] = { 1, 2, 3, 4, 5 };

    printf("  Array initialization: ");
    for (int i = 0; i < 5; i++) { printf("%d ", numbers[i]); }
    printf("\n");

    // Multi-dimensional array
    int matrix[2][3] = { { 1, 2, 3 }, { 4, 5, 6 } };
    printf("  2D array element: matrix[1][2] = %d\n", matrix[1][2]);

    printf("\n");
}

// Test string operations
void
test_strings()
{
    printf("Testing String Operations:\n");

    char str1[] = "Hello";
    char str2[] = "World";
    char result[20];

    strcpy(result, str1);
    strcat(result, " ");
    strcat(result, str2);

    printf("  String concatenation: %s\n", result);
    printf("  String length: strlen(\"%s\") = %ui\n", result, strlen(result));
    printf("  String comparison: strcmp(\"%s\", \"%s\") = %d\n", str1, str2, strcmp(str1, str2));

    printf("\n");
}

// Test floating-point operations
void
test_floating_point()
{
    printf("Testing Floating-Point Instructions:\n");

    float  a = 3.14f, b = 2.0f;
    double c = 2.718281828;

    printf("  Float addition: %f + %f = %f\n", a, b, a + b);
    printf("  Float multiplication: %f * %f = %f\n", a, b, a * b);
    printf("  Float division: %f / %f = %f\n", a, b, a / b);

    printf("  Math functions:\n");
    printf("    sqrt(16.0) = %f\n", sqrt(16.0));
    printf("    sin(%f) = %.4f\n", a, sin(a));
    printf("    log(%.6f) = %.4f\n", c, log(c));

    printf("\n");
}

// Test type conversions
void
test_type_conversions()
{
    printf("Testing Type Conversions:\n");

    int   i = 100;
    float f = 3.14f;
    char  c = 'A';

    printf("  int to float: %d -> %f\n", i, (float)i);
    printf("  float to int: %f -> %d\n", f, (int)f);
    printf("  char to int: '%c' -> %d\n", c, (int)c);
    printf("  int to char: %d -> '%c'\n", 65, (char)65);

    // Implicit conversions
    float result = i + f;
    printf("  Mixed arithmetic: %d + %f = %f\n", i, f, result);

    printf("\n");
}

// Helper functions
int
add(int a, int b)
{
    return a + b;
}

int
factorial(int n)
{
    if (n <= 1) { return 1; }
    return n * factorial(n - 1);
}

int
main()
{
    printf("=== Comprehensive C Instruction Test Program ===\n\n");

    // Test various instruction categories
    test_arithmetic();
    test_logical();
    test_bitwise();
    test_comparison();
    test_memory();
    test_control_flow();
    test_functions();
    test_pointers();
    test_arrays();
    test_strings();
    test_floating_point();
    test_type_conversions();

    printf("\n=== All tests completed successfully! ===\n");

    return 0;
}