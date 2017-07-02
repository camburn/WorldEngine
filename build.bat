CALL "C:/Program Files (x86)/Microsoft Visual Studio/Preview/Community/VC/Auxiliary/Build/vcvars32.bat"
del /Q "C:/Users/campbell/Documents/Projects/embed_python/build/*"
cd /d "C:/Users/campbell/Documents/Projects/embed_python/build"

REM Build x86
cl /Zl /MT /EHsc^
    ../src/*.cpp^
    ../src/graphics/*.cpp^
    ../src/imgui/*.cpp^
    ../src/console/*.cpp^
    ../src/tools/*.cpp^
    /Fe:embed_x86.exe^
    /I"../include/"^
    /I"C:/Users/campbell/Documents/Projects/libraries/"^
    /I"C:/Users/campbell/Documents/Projects/libraries/Assimp/include"^
    /I"C:/Users/campbell/Documents/Projects/libraries/freetype/include/freetype2"^
    /I"C:/Users/campbell/Documents/Projects/libraries/glew-2.0.0/include"^
    /I"C:/Users/campbell/Documents/Projects/libraries/glfw-3.2.1.bin.WIN32/include"^
    /I"C:/Users/campbell/Documents/Projects/libraries/glfw-3.2.1.bin.WIN64/include"^
    /I"C:/Users/campbell/Documents/Projects/libraries/glm"^
    /I"C:/Users/campbell/Documents/Projects/libraries/imgui-1.49"^
    /I"C:/Program Files (x86)/Python36-32/include"^
    /I"C:/Users/campbell/Documents/Projects/libraries/shapelib-1.4.0"^
    /I"C:/Users/campbell/Documents/Projects/libraries/soil/include"^
    /link^
    /LIBPATH:"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x86/"^
    User32.Lib^
    Gdi32.Lib^
    OpenGL32.lib^
    kernel32.lib^
    shell32.lib^
    /LIBPATH:"C:/Program Files (x86)/Python36-32/libs/python36.lib"^
    /LIBPATH:"C:/Program Files (x86)/Python36-32/libs/python36_d.lib"^
    /LIBPATH:C:/Users/campbell/Documents/Projects/libraries/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib^
    C:/Users/campbell/Documents/Projects/libraries/glew-2.0.0/lib/Release/Win32/glew32s.lib^
    C:/Users/campbell/Documents/Projects/libraries/soil/lib/SOIL.lib^
    C:/Users/campbell/Documents/Projects/libraries/freetype/lib/freetype.lib^
    C:/Users/campbell/Documents/Projects/libraries/Assimp/lib/x86/assimp.lib^
    C:/Users/campbell/Documents/Projects/libraries/shapelib-1.4.0/shapelib.lib
echo ---------------------------------------------------------------------------
cd "C:/Users/campbell/Documents/Projects/embed_python/"
if errorlevel 1 (
   echo x86 Build Failed, Reason Given is %errorlevel%
   exit /b %errorlevel%
)