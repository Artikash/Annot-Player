:: update (SHIFT-JIS)
:: 8/12/2012
@echo off
setlocal
cd /d %~dp0
cd /d ..\..

set PATH=%~dp0;%PATH%

set VERSION=0.1.0

set HOST=210.175.52.140
set SOURCE=/release/annot-player/win32/Annot Stream
set TARGET=.
set EXCLUDE=%VERSION%
set THREADS=4

echo --------------------------------------------------------------------
echo   Do you want to update the apps? (An Internet access is required)
echo          �\�t�g���X�V���܂����H�i�C���^�[�l�b�g���K�v�ł��j
echo --------------------------------------------------------------------
echo.
pause
echo.

echo --------------------------------------------------------------------
echo            Please close all apps in Annot Stream!
echo              �S�Ă�Annot�A�v������Ă��������I
echo --------------------------------------------------------------------
echo.
pause
echo.

:: see: http://lftp.yar.ru/lftp-man.html
lftp "%HOST%" -e "mirror --verbose=3 --delete --no-perms --no-umask --depth-first --parallel=%THREADS% --exclude '%EXCLUDE%' '%SOURCE%' '%TARGET%'; exit"

pushd update
for /r %%i in (deploy.cmd) do if exist "%%~i" call "%%~i"
popd

echo.
echo --------------------------------------------------------------------
echo                      Update complete!
echo                          �X�V�����I
echo --------------------------------------------------------------------
echo.
pause
echo.

if exist "ChangeLog.txt" explorer "ChangeLog.txt"

:: EOF
