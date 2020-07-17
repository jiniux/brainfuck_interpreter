# Brainfuck interpreter

Optimized lightweight Brainfuck interpreter made in C (needless to say: from scratch). 

## Build

It requires [The Meson Build System](https://github.com/mesonbuild/meson) to be compiled. Once you have it installed, do:

```
$ meson build/
$ meson compile -C build/
```

Then, inside the `build` directory you will find the binaries.

## Optimizations

Currently the interpreter has the optimization features shown below. 

### Combining instructions

`>`, `<`, `+`, `-` instructions are not executed one by one, but the interpreter sums/subtracts the times they are repeated either to `ptr` or `*ptr`.
For example, `>>>` becomes `ptr+=3`.

### Position jump

Unlike some other interpreter, this one automatically jumps at the end and at the start of cycles without crossing them entirely. 

### Loops to zero 

When `[-]` or `[+]` (loops that continues till the current cell becomes zero) occur, the interpreter directly sets `*ptr` to 0.