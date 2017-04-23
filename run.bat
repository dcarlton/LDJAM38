@echo off
cl /EHsc TinyDungeon.cpp /I lib\SDL2-2.0.5\include /I lib\SDL2_ttf-2.0.14\include /I lib\SDL2_mixer-2.0.1\include /link /libpath lib\SDL2-2.0.5\lib\x86\SDL2.lib /libpath lib\SDL2_ttf-2.0.14\lib\x86\SDL2_ttf.lib /libpath lib\SDL2_mixer-2.0.1\lib\x86\SDL2_mixer.lib
TinyDungeon.exe | cat
del TinyDungeon.exe