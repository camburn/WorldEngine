CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
del /Q "D:\projects\embed_python\build\*"
cd /d "D:\projects\embed_python\build"

REM Build x86
cl /Zl /MT /EHsc^
    ..\src\embed.c ..\src\python_api.c^
    /Fe:embed_x86.exe^
    /I"D:/libraries/glew-2.0.0/include"^
    /I"D:/libraries/glfw-3.2.1.bin.WIN32/include"^
    /I"D:\libraries\graphene-shared-x86\include"^
    /I"D:\libraries\graphene-shared-x86\lib\graphene-1.0\include"^
    /I"D:/python36/include/"^
    /I"../src/"^
    /link^
    D:\python36\libs\python36.lib^
    /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.14393.0\um\x86\"^
    User32.Lib^
    Gdi32.Lib^
    OpenGL32.lib^
    kernel32.lib^
    shell32.lib^
    D:\libraries\glfw-3.2.1.bin.WIN32\lib-vc2015\glfw3.lib^
    D:\libraries\glew-2.0.0\lib\Release\Win32\glew32s.lib^
    D:\libraries\graphene-shared-x86\lib\graphene-1.0.lib
echo ---------------------------------------------------------------------------
if errorlevel 1 (
   echo x86 Build Failed, Reason Given is %errorlevel%
   exit /b %errorlevel%
)
