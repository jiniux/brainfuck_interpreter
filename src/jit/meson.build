supported_archs = { 'linux': ['x86_64'] }

sys = target_machine.system()
arch = target_machine.cpu_family()

if sys in supported_archs and supported_archs[sys].contains(arch)
    file = sys + '/' + arch + '.c'
else
    file = 'null_arch.c'
endif

src_files += files([file])