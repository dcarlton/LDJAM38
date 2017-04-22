@echo off
cl /EHsc main.cpp /I SDL2-2.0.5\include /link /libpath SDL2-2.0.5\lib\x86\SDL2.lib
main.exe | cat
del main.exe