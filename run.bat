@echo off
cl /EHsc main.cpp /I SDL2-2.0.5\include /I SDL2_ttf-2.0.14\include /link /libpath SDL2-2.0.5\lib\x86\SDL2.lib /libpath SDL2_ttf-2.0.14\lib\x86\SDL2_ttf.lib
main.exe | cat
del main.exe