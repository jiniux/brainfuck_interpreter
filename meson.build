project('brainfuck_interpreter', 'c', 
    default_options : ['c_std=gnu11'])
    
subdir('src/')

exe = executable('brainfuck_interpreter', src_files, c_args: ['-O3', '-march=native'])

test('Hello world       - Interpreter', exe, args : ['../tests/hello_world.btest'], is_parallel : false)
test('Towers of Hanoi   - Interpreter', exe, args : ['../tests/hanoi.btest'], is_parallel : false)
test('Mandelbrot        - Interpreter', exe, args : ['../tests/mandelbrot.btest'], is_parallel : false)
test('Project Euler 1   - Interpreter', exe, args : ['../tests/project_euler_1.btest'], is_parallel : false)

test('Hello world       - JIT', exe, args : ['../tests/hello_world.btest', '--jit'], is_parallel : false)
test('Towers of Hanoi   - JIT', exe, args : ['../tests/hanoi.btest', '--jit'], is_parallel : false)
test('Mandelbrot        - JIT', exe, args : ['../tests/mandelbrot.btest', '--jit'], is_parallel : false)
test('Project Euler 1   - JIT', exe, args : ['../tests/project_euler_1.btest', '--jit'], is_parallel : false)
