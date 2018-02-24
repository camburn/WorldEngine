if not defined DevEnvDir (
    CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
)
REM del /Q D:\projects\WorldEngine\build\*
cd /d D:\projects\WorldEngine\build
ECHO "Bulding WorldEngine"

cl /Zl /MD /EHsc /Z7^
    ../src/graphics/shapefile_loader.cpp^
    ../src/*.cpp^
    ../src/graphics/*.cpp^
    ../src/imgui/*.cpp^
    ../src/console/*.cpp^
    ../src/tools/*.cpp^
    ../extern/poly2tri-master/poly2tri/common/*.cc^
    ../extern/poly2tri-master/poly2tri/sweep/*.cc^
    /Fe:WorldEngine.exe^
    /I"../include/"^
    /I"../extern/poly2tri-master/"^
    /I"D:/libraries/"^
    /I"D:/libraries/assimp-4.0.1/include"^
    /I"D:/libraries/freetype/include/freetype2"^
    /I"D:/libraries/glew-2.0.0/include"^
    /I"D:/libraries/glfw-3.2.1.bin.WIN32/include"^
    /I"D:/libraries/glm"^
    /I"D:/libraries/imgui-1.50"^
    /I"D:/python36/include"^
    /I"D:/libraries/shapelib-1.4.0"^
    /I"D:/libraries/soil/include"^
    /link^
    /NODEFAULTLIB:libcmt.lib^
    /LIBPATH:"D:/python36/libs/"^
    python36.lib^
    python36_d.lib^
    /LIBPATH:"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x86/"^
    User32.Lib^
    Gdi32.Lib^
    OpenGL32.lib^
    kernel32.lib^
    shell32.lib^
    /LIBPATH:"D:/libraries/glfw-3.2.1.bin.WIN32/lib-vc2015/"^
    glfw3.lib^
    /LIBPATH:"D:/libraries/glew-2.0.0/lib/Release/Win32/"^
    glew32s.lib^
    /LIBPATH:"D:/libraries/soil/lib/"^
    /LTCG^
    SOIL.lib^
    /LIBPATH:"D:/libraries/freetype/lib/"^
    freetype.lib^
    /LIBPATH:"D:/libraries/assimp-4.0.1/lib/"^
    assimp.lib^
    IrrXML.lib^
    zlibstatic.lib^
    /LIBPATH:"D:/libraries/shapelib-1.4.0/"^
    shapelib.lib

echo ---------------------------------------------------------------------------
cd "D:\projects\PyEngine\"
if errorlevel 1 (
   echo x86 Build Failed, Reason Given is %errorlevel%
   exit /b %errorlevel%
)