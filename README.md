# Brainfuck interpreter

Optimized lightweight Brainfuck interpreter made in C (needless to say: from scratch). 

## Build

It requires [The Meson Build System](https://github.com/mesonbuild/meson) to be compiled. Once you have it installed, do:

```
$ meson build/
$ meson compile -C build/
```

Then, inside the `build` directory you will find the binaries.

## Features

- `65535` 8-bit cells.
- Bytecode parsing and optimization.
- Optimized integrated JIT compiler.

## Usage 

```
$ brainfuck_interpreter <filename.bf> [--jit]
```

## Benchmarks

The following tests were conducted on an **Intel Core i7-7700k 4.20GHz** processor; the program was compiled using **GCC 9.3.0** with the following flags: `-O3 -march=native`.

| Program | Interpreter  | JIT compiled code |
|---|---|---|
| `mandelbrot.bf`  | `3.782s` | `1.714s` |
| `hanoi.bf` | `0.356s` | `0.260s`|
| `project_euler_1.bf` | `13.902s` | `7.552s` |

## Optimizations

Currently the interpreter has the optimization features shown below. 

### Combining instructions

`>`, `<`, `+`, `-` instructions are not executed one by one, but the interpreter sums/subtracts the times they are repeated either to `ptr` or `*ptr`.
For example, `>>>` becomes `ptr+=3`.

### Position jump

Unlike some other interpreter, this one automatically jumps at the end and at the start of cycles without crossing them entirely. 

### Loops to zero 

When `[-]` or `[+]` (loops that continues till the current cell becomes zero) occur, the interpreter directly sets `*ptr` to 0.

## Special thanks

Thanks to [GaggiX](https://github.com/GaggiX) for helping. 
