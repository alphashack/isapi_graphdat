@echo off

REM -- Check command line parameters
if [%1]==[] goto usage
if [%2]==[] goto usage

REM -- Get epoch to use to get seconds to sign the build
for /f "tokens=2,3,4 delims=/ " %%f in ('date /t') do set y=%%h
for /f "tokens=2,3,4 delims=/ " %%f in ('date /t') do set d=%%f%%g
for /f "tokens=1,2,3 delims=: " %%f in ('echo %time:~0,8%') do set t=%%f%%g
set version=%2.%d%.%t%

REM -- sign the build with the users version + the time from above
@echo Signing Build %1 with version %version%
verpatch.exe %1 "%version%" /pv "%version%" /sc "Graphdat ISAPI Filter" /s product "Graphdat ISAPI Filter" /s desc "For the Graphdat Agent used in http://www.graphdat.com" /s copyright "Copyright (c) 2012" /s company "Alphashack"
goto :eof

REM -- in case you forgot how to call it
:usage
@echo Usage: version-isapi-filter.cmd [path to file] [version number]
@echo example: version-isapi-filter.cmd graphdat_filter.dll 0.2
exit /B 1
