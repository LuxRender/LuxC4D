@echo off

rem  Here you can configure the compression tool and the version number that will
rem  be used in the archive filename.
set VERSION=0.05
set PACKER_DIR=c:\program files\7-zip
set PACKER="%PACKER_DIR%\7za.exe"
set ARCHIVE_OPT=a -tzip -mx9
set ARCHIVE_SUFFIX=zip
set DISTRI_DIR=_distri

rem  Check if packer is available (see above)
if not exist %PACKER% (
  set MSG="Packer not found! (PACKER='%PACKER%')"
  goto :PRINT_ERROR_AND_EXIT
)

rem  Check if VC is available 
if exist "%VS80COMNTOOLS%\..\IDE\devenv.exe" (
  set VC="%VS80COMNTOOLS%\..\IDE\devenv.exe"
) else if exist "%VS80COMNTOOLS%\..\IDE\vcexpress.exe" (
  set VC="%VS80COMNTOOLS%\..\IDE\vcexpress.exe"
) else (
  set MSG="No valid VC found! (VS80COMNTOOLS='%VS80COMNTOOLS%')"
  goto :PRINT_ERROR_AND_EXIT
)

rem  Add packer directory to PATH
set PATH=%PATH%;%PACKER_DIR%

rem  Recreate distribution directory  and set it up with all the resource files
rem  of LuxC4D
rmdir /s /q %DISTRI_DIR%
mkdir %DISTRI_DIR%
xcopy /q /s /i ..\..\res %DISTRI_DIR%\LuxC4D\res
xcopy /q /s /i ..\..\license %DISTRI_DIR%\LuxC4D\license

rem  Bulild all the different configurations
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
set CONFIG=R11.0_Debug
set CONFIG_ZIP=win32_r11.0_debug
call :BUILD_AND_COMPRESS
set CONFIG=R11.0_Release
set CONFIG_ZIP=win32_r11.0_release
call :BUILD_AND_COMPRESS

rem  Remove plugin files
rmdir /s /q %DISTRI_DIR%\LuxC4D

rem  Stop
goto :EOF


rem  This "function" builds one configuration of LuxC4D and creates an archive of it
:BUILD_AND_COMPRESS
%VC% LuxC4D.sln /Rebuild %CONFIG%
if not exist %CONFIG%\LuxC4D.cdl (
  echo Build of configuration "%CONFIG%" failed!
  goto :EOF
)
xcopy /q /y %CONFIG%\LuxC4D.cdl %DISTRI_DIR%\LuxC4D\
cd %DISTRI_DIR%
%PACKER% %ARCHIVE_OPT% LuxC4D_%VERSION%_%CONFIG_ZIP%.%ARCHIVE_SUFFIX% LuxC4D
cd ..
goto :EOF


rem  Prints an error message and exits the batch file
:PRINT_ERROR_AND_EXIT
echo %MSG%
exit /b 1
