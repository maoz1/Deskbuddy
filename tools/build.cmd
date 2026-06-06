@echo off
REM Build the project. Writes live log to build.log AND prints to screen.
echo === Building Deskbuddy ===
echo Log file: build.log
echo.
python -m platformio run 2>&1 | tee build.log
