# List all serial devices (helps you find which COM port the ESP32 is on).
Write-Host "=== Connected serial devices ===" -ForegroundColor Cyan
python -m platformio device list
