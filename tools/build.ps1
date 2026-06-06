# Build the firmware. Shows output live AND saves to build.log.
Write-Host "=== Building Deskbuddy ===" -ForegroundColor Cyan
Write-Host "Log file: build.log`n"
python -m platformio run 2>&1 | Tee-Object -FilePath build.log
