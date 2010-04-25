@echo off
strip ..\..\_plugins_win32\*.dll
strip ..\..\*.dll
strip ..\..\*.exe
upx --brute ..\..\vsxu_artiste.exe
upx --brute ..\..\vsxu_developer.exe
upx --brute ..\..\vsxu_player.exe
upx --brute ..\..\vsxu_engine.dll
upx --brute ..\..\vsxfst.dll
upx --brute ..\..\vsxg.dll
upx --brute ..\..\_plugins_win32\*.dll