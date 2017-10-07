@echo off
REM This generates build time stamp comprising of YY/MM/DD + HH:MM format
setlocal
for /f "tokens=2,3,4 delims=/ " %%f in ('date /t') do set d=%%h%%f%%g
for /f "tokens=1,2 delims=: " %%f in ('time /t') do set t=%%f%%g
echo -D BUILD_TIME=%d:~2,8%%t%
endlocal
