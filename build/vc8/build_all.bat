@echo off
set PATH=%PATH%;c:\program files\7-zip
set VERSION=0.04
set DISTRI_DIR=_distri

rmdir /s /q %DISTRI_DIR%
mkdir %DISTRI_DIR%
xcopy /q /s /i /exclude:copyexcl.txt ..\..\res %DISTRI_DIR%\LuxC4D\res
xcopy /q /s /i /exclude:copyexcl.txt ..\..\license %DISTRI_DIR%\LuxC4D\license

set CONFIG=R9.6_Debug
set CONFIG_ZIP=win32_r9.6_debug
call :BUILD_AND_COMPRESS

set CONFIG=R9.6_Release
set CONFIG_ZIP=win32_r9.6_release
call :BUILD_AND_COMPRESS

set CONFIG=R10.1_Debug
set CONFIG_ZIP=win32_r10.1_debug
call :BUILD_AND_COMPRESS

set CONFIG=R10.1_Release
set CONFIG_ZIP=win32_r10.1_release
call :BUILD_AND_COMPRESS

set CONFIG=R10.5_Debug
set CONFIG_ZIP=win32_r10.5_debug
call :BUILD_AND_COMPRESS

set CONFIG=R10.5_Release
set CONFIG_ZIP=win32_r10.5_release
call :BUILD_AND_COMPRESS

rmdir /s /q %DISTRI_DIR%\LuxC4D

goto :EOF


:BUILD_AND_COMPRESS
"%VS80COMNTOOLS%\..\IDE\vcexpress.exe" LuxC4D.sln /Rebuild %CONFIG%
xcopy /q /y %CONFIG%\LuxC4D.cdl %DISTRI_DIR%\LuxC4D\
cd %DISTRI_DIR%
7za a -tzip -mx9 LuxC4D_%VERSION%_%CONFIG_ZIP%.zip LuxC4D
cd ..
goto :EOF
