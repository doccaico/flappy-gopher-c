@echo off

SETLOCAL

SET NAME=flappy

IF        "%1" == ""          ( SET MODE=D && GOTO :MAIN
) ELSE IF "%1" == "--debug"   ( SET MODE=D && GOTO :MAIN
) ELSE IF "%1" == "--release" ( SET MODE=R && GOTO :MAIN
) ELSE IF "%1" == "--help"    ( GOTO :HELP
) ELSE IF "%1" == "-h"        ( GOTO :HELP 
) ELSE                        ( GOTO :HELP  
)

:HELP
    ECHO Usage : $ build [--debug^|--release^|-h^|--help]
    ECHO   Example:  $ build ^| build --debug
    ECHO             $ build --release
    ECHO             $ build -h ^| build --help
EXIT /B 0

:MAIN
    SET CC=cl.exe

    SET C_FLAGS=/std:c17 /nologo /utf-8 /GR- /permissive- /W4 /D_CRT_SECURE_NO_WARNINGS ^
        /I"lib\raylib\include" ^
        /I"lib\stb"
    IF %MODE%==D SET C_FLAGS=%C_FLAGS% /GS /Od /RTCs /D"_DEBUG"
    IF %MODE%==R SET C_FLAGS=%C_FLAGS% /WX /GS- /O2 /Oi /Ot /D"NDEBUG"

    SET LINK_FLAGS=/INCREMENTAL:NO /NOLOGO /LIBPATH:"lib\raylib\lib" /NODEFAULTLIB:libcmt
    IF %MODE%==D SET LINK_FLAGS=%LINK_FLAGS% /DEBUG:FULL
    IF %MODE%==R SET LINK_FLAGS=%LINK_FLAGS%

    %CC% %C_FLAGS% /MP /Fe:"%NAME%.exe" "src\*.c" ^
        /link %LINK_FLAGS% raylib.lib gdi32.lib winmm.lib shell32.lib user32.lib
EXIT /B %ERRORLEVEL%

REM vim:set fenc=cp932 ff=dos
