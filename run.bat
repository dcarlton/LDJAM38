@echo off
cl /EHsc main.cpp /I SDL2-2.0.5\include /I SDL2_ttf-2.0.14\include /I SDL2_mixer-2.0.1\include /link /libpath SDL2-2.0.5\lib\x86\SDL2.lib /libpath SDL2_ttf-2.0.14\lib\x86\SDL2_ttf.lib /libpath SDL2_mixer-2.0.1\lib\x86\SDL2_mixer.lib
main.exe | cat
del main.exe