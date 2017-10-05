@echo off
if "%1"=="" goto USAGE
echo ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
echo Model: %1
echo ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
call bcc31 -ml -w -I.. %1 %2 %3 %4 %5 ..\simlib.lib
if errorlevel 1 goto ERR
del *.obj
echo.
goto END
:ERR
echo.
echo CHYBA!
echo Chyba v modelu: %1 >>simlib.log
goto END
:USAGE
echo.
echo SIMLIB.BAT - pýeklad modelu v C++/SIMLIB (TESTOVACÖ VERZE!)
echo.
echo Pou§it¡:
echo           SIMLIB  model.cpp
:END
echo.
