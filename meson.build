project('brainfuck_interpreter', 'c', 
    default_options : ['c_std=gnu11'])
    
subdir('src/')

exe = executable('brainfuck_interpreter', src_files, c_args: ['-O3', '-march=native'])

test('Hello world', exe, args : ['../tests/hello_world.btest'], is_parallel : false)
test('Towers of Hanoi', exe, args : ['../tests/hanoi.btest'], is_parallel : false)
test('Mandelbrot', exe, args : ['../tests/mandelbrot.btest'], is_parallel : false)
