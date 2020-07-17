project('brainfuck_interpreter', 'c', 
    default_options : ['c_std=gnu11'])
    
subdir('src/')


exe = executable('brainfuck_interpreter', src_files, c_args: ['-O3', '-march=native'])

test('Towers of Hanoi', exe, args : ['../tests/hanoi.b'], is_parallel : false)
test('Mandelbrot', exe, args : ['../tests/mandelbrot.b'], is_parallel : false)