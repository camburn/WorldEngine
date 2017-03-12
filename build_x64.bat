echo ---------------------------------------------------------------------------
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d "D:\projects\embed_python\build"
REM Build AMD64
cl /Zl /MT /EHsc^
    ..\src\embed.c ..\src\python_api.c^
    /Fe:embed_x64.exe^
    /I"D:/libraries/glew-2.0.0/include"^
    /I"D:/libraries/glfw-3.2.1.bin.WIN64/include"^
    /I"D:\libraries\graphene-shared-x64\include"^
    /I"D:\libraries\graphene-shared-x64\lib\graphene-1.0\include"^
    /I"D:/python36-x64/include/"^
    /I"../src/"^
    /link^
    D:\python36-x64\libs\python36.lib^
    /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.14393.0\um\x64\"^
    User32.Lib^
    Gdi32.Lib^
    OpenGL32.lib^
    kernel32.lib^
    shell32.lib^
    D:\libraries\glfw-3.2.1.bin.WIN64\lib-vc2015\glfw3.lib^
    D:\libraries\glew-2.0.0\lib\Release\x64\glew32.lib^
    D:\libraries\graphene-shared-x64\lib\graphene-1.0.lib
if errorlevel 1 (
   echo AMD64 Build Failed, Reason Given is %errorlevel%
   exit /b %errorlevel%
)