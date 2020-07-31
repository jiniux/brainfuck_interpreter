supported_archs = ['x86_64']

if supported_archs.contains(target_machine.cpu_family()) and target_machine.system() == 'linux'
    file = 'compiler_' + target_machine.cpu_family() + '.c'
else
    file = 'compiler_null.c'
endif

src_files += files([file])