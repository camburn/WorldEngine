import sys
import glob
import platform
import yaml

env = Environment(TARGET_ARCH='x86')
mode = ARGUMENTS.get('target', 'debug')

if platform.system() == 'Windows':
    with open('config/windows.yaml', 'r') as f:
        data = yaml.load(f.read())
elif platform.system() == 'Linux':
    with open('config/linux.yaml', 'r') as f:
        data = yaml.load(f.read())
else:
    raise Exception("OS Not Supported")

env.Append(
    CXXFLAGS=data['compile_flags'],
    LINKFLAGS=data['link_flags']
)
if mode == 'debug':
    print('Building Debug Version')
    env.Append(
        CXXFLAGS=data['debug_compile_flags'],
        LINKFLAGS=data['debug_link_flags']
    )

env.VariantDir('build', 'src', duplicate=0)
env.Program(
    'bin/WorldEngine',
    Glob('build/*.cpp') + Glob('build/**/*.cpp') + Glob('build/graphics/**/*.cpp'),
    CPPPATH=data['include_dirs'],
    LIBS=data['libs'],
    LIBPATH= data['lib_paths']
)
