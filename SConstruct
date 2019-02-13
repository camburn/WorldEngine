import os
import sys
import glob
import platform
import re
import yaml

env = Environment()

mode = ARGUMENTS.get('target', 'debug')

def env_substitute_constructor(loader, node):
    value = loader.construct_scalar(node)
    env_var, remaining_path = pattern.match(value).groups()
    return os.environ[env_var] + remaining_path

pattern = re.compile(r'^\$\((.*)\)(.*)$')
yaml.add_implicit_resolver("!env_substitute", pattern)
yaml.add_constructor('!env_substitute', env_substitute_constructor)

if platform.system() == 'Windows':
    env.Append(
        TARGET_ARCH='x86_64'
    )
    with open('config/windows_x64.yaml', 'r') as f:
        try:
            data = yaml.load(f.read())
        except KeyError as err:
            print("Required build environment variable missing", err)
            sys.exit(1)
elif platform.system() == 'Linux':
    env['CXX'] = "clang++"
    env.Append(
        TARGET_ARCH='x86_64'
    )
    with open('config/linux.yaml', 'r') as f:
        try:
            data = yaml.load(f.read())
        except KeyError as err:
            print("Required build environment variable missing", err)
            sys.exit(1)
else:
    raise Exception("OS Not Supported")

env.Append(
    CXXFLAGS=data['compile_flags'],
    LINKFLAGS=data['link_flags'],
    ENV={'TERM': os.environ['TERM']}
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
    Glob('extern/glfw-3.2.1/src/*.c') + Glob('extern/imgui/*.cpp') + Glob('build/*.cpp') + Glob('build/**/*.cpp') + Glob('build/graphics/**/*.cpp'),
    CPPPATH=data['include_dirs'],
    LIBS=data['libs'],
    LIBPATH=data['lib_paths']
)
