# Interpreter, Compiler, JIT
This repo contains three programs used in my [blog post](https://nickdesaulniers.github.io/blog/2015/05/25/interpreter-compiler-jit/); an interpreter, a compiler, and a Just In Time (JIT) compiler for the brainfuck language.  It's meant to show how similar these techniques are.

### Portability
While all three are written in C, only the interpreter should be portable, even to emscripten.  The compiler and JIT is highly dependant on the x64 Instruction Set Architecture (ISA), and Linux/OSX style calling convention.  The assembler is made for OSX, but you can probably just link using GCC/Clang on Linux.

### Warning
I take no responsibility for you running arbitrary brainfuck programs you found on the Internet through the compiler or the JIT.  I would only run either in a virtual machine, just to be safe!  You've been warned!

## Building
### Native
```bash
mkdir build
cd build
cmake ..
make
```
### Emscripten
Only the interpreter is portable to JavaScript.

```bash
mkdir build
cd build
cp ../samples/* .
emcmake cmake ..
emmake make
```

## Running
### The Interpreter
```bash
./interpreter ../samples/hello_world.bf
```
or Emscripten-ized:

```bash
node interpreter.js hello_world.bf
```

### The Compiler
```bash
./compiler ../samples/hello_world.bf > temp.s
../assemble.sh temp.s
rm temp.s
./a.out
```

### The JIT
```bash
./jit ../samples/hello_world.bf
```

## License
The code in this repo _except_ the code in `samples/` has the following license:

```bash
THE BEER-WARE LICENSE (Revision 42):
<nick@mozilla.com> wrote this file. As long as you retain this notice you
can do whatever you want with this stuff. If we meet some day, and you think
this stuff is worth it, you can buy me a beer in return.

Nick Desaulniers
```
