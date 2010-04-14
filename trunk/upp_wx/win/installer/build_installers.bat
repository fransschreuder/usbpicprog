@del *.exe
@set PATH=%PATH%;%ProgramFiles%\NSIS
makensis.exe /V2 /DARCH=x86 setup.nsi
@echo.
makensis.exe /V2 /DARCH=amd64 setup.nsi
@pause
