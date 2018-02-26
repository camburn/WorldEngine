import os
import glob
import subprocess

import sys
import hashlib

import contextlib


BUF_SIZE = 65536
HASH_FILE = '.\\build\\.hash'
PROGRAM_NAME = 'WorldEngine.exe'

BUILD_COMMAND = 'cl /Zl /MD /EHsc /Z7 /c '
#BUILD_COMMAND = 'D:/Programs/mingw/bin/g++.exe -Wall -c '
INCLUDE_LIST = [
    '/I"../include/"',
    '/I"D:/libraries/"',
    '/I"D:/libraries/assimp-4.0.1/include"',
    '/I"D:/libraries/freetype/include/freetype2"',
    '/I"D:/libraries/glew-2.0.0/include"',
    '/I"D:/libraries/glfw-3.2.1.bin.WIN32/include"',
    '/I"D:/libraries/glm"',
    '/I"D:/libraries/imgui-1.50"',
    '/I"D:/python36/include"',
    '/I"D:/libraries/shapelib-1.4.0"',
    '/I"D:/libraries/soil/include"'
]

#INCLUDE_LIST = [x.replace('/I', '-I') for x in INCLUDE_LIST]

LINK_COMMAND = 'LINK /DEBUG /OUT:WorldEngine.exe '
#LINK_COMMAND = 'D:/Programs/mingw/bin/g++.exe -Wall -o WorldEngine.exe '
LINKER_PATHS = [
    '/LIBPATH:"D:/python36/libs/"',
    '/LIBPATH:"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x86/"',
    '/LIBPATH:"D:/libraries/glfw-3.2.1.bin.WIN32/lib-vc2015/"',
    '/LIBPATH:"D:/libraries/glew-2.0.0/lib/Release/Win32/"',
    '/LIBPATH:"D:/libraries/soil/lib/"',
    '/LIBPATH:"D:/libraries/freetype/lib/"',
    '/LIBPATH:"D:/libraries/assimp-4.0.1/lib/"',
    '/LIBPATH:"D:/libraries/shapelib-1.4.0/"',
]
LINKER_LIBS = [
    '/NODEFAULTLIB:libcmt.lib',
    '/LTCG',
    'python36.lib',
    'python36_d.lib',
    'User32.Lib',
    'Gdi32.Lib',
    'OpenGL32.lib',
    'kernel32.lib',
    'shell32.lib',
    'glfw3.lib',
    'glew32s.lib',
    'SOIL.lib',
    'freetype.lib',
    'assimp.lib',
    'IrrXML.lib',
    'zlibstatic.lib',
    'shapelib.lib'
]
#LINKER_PATHS = [x.replace('/LIBPATH:', '-L') for x in INCLUDE_LIST]
#LINKER_PATHS = [x if x.startswith('/') else '-l'+x for x in INCLUDE_LIST]

@contextlib.contextmanager
def pushd(new_dir):
    previous_dir = os.getcwd()
    os.chdir(new_dir)
    yield
    os.chdir(previous_dir)


def calculate_sha(filename):
    sha1 = hashlib.sha1()
    with open(filename, 'rb') as f:
        while True:
            data = f.read(BUF_SIZE)
            if not data:
                break
            sha1.update(data)
    return sha1.hexdigest()


if __name__ == '__main__':
    # Get all source files in the src directory
    hashmap = {}
    old_hashmap = {}
    items = glob.glob('.\\src\\**\\*') + glob.glob('.\\src\\*')
    for filename in (x for x in items if x.endswith('.cpp') or x.endswith('.c')):
        hashmap[filename] = calculate_sha(filename)
    
    # Do we have a previous saved hashmap?
    if os.path.exists(HASH_FILE):
        with open(HASH_FILE, 'r') as f:
            for line in f:
                path, filehash = line.split()
                old_hashmap[path] = filehash
    
    os.chdir('D:/projects/WorldEngine/')
    relink = False
    # Build files that have changed
    with pushd('build'):
        for filename in hashmap.keys():
            if filename not in old_hashmap or hashmap[filename] != old_hashmap[filename]:
                print(os.getcwd())
                subprocess.check_call(BUILD_COMMAND + ' '.join(INCLUDE_LIST) + ' .' + filename)
                relink = True
    
        obj_files = glob.glob('*.obj')
        # relink new program
        if relink or not os.path.exists('build/' + PROGRAM_NAME):
            subprocess.check_call(' '.join([LINK_COMMAND, ' '.join(LINKER_PATHS), ' '.join(LINKER_LIBS), ' '.join(obj_files)]))
        
        
    # save new hashmap to file
    with open(HASH_FILE, 'w') as f:
        for filepath, filehash in hashmap.items():
            f.write('{} {}\n'.format(filepath, filehash))
