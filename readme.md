# CowVM 🐮🐄 - 

CowVM 🐮 is a lightweight compiler backend written in C, designed to provide an efficient and easy-to-use platform for
compiling programming languages to machine code. With a focus on simplicity and performance, CowVM aims to be an ideal
choice for developers looking to generate machine code without the overhead of more complex
backend solutions.

## Features

- Simple API
- JIT mode
- Currently supporting x86_64 instructions set
- Bytecode Interpret mode available

## Example 📢

Compiling a function that retus

```C
#include <cow.h>
typedef int (*JitFunc)();

int main() {

    // create the module
    CowModule module = cow_module_create();
    
    // create the function with no arguments and that returns an 32-bits integer
    CowFunc my_func = cow_create_func(module, "my_func", NULL, 0, cow_type_i32());
    
    // get the builder and start writing the instruction that return the constant 100
    CowBuilder builder = cow_func_get_builder(my_func);
    cow_builder_ret(builder, cow_builder_const_i64(builder, 100));

    // generate the machine instructions from our jit
    cow_module_jit(module);

    // get the pointer of the generate function and call it
    JitFunc ptr_func = my_func->jit_func.generated_func;
    
    // result will now hold 100
    int result = ptr_func(); 
    
    // clear everything from memory
    cow_module_free(module);

    return 0;
}
```

## Todo 📝

- Complete missing instructions (mostly operations)
- Support arguments
- Support the default calling convention for the platform
- Add basic instructions
- Add AOT mode (export module into an ELF file)

# License 📁


    Copyright c 2023 TheCowDev
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
