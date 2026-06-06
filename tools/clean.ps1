# Remove project build artifacts (.pio folder). Doesn't touch the global cache.
Write-Host "=== Cleaning project build ===" -ForegroundColor Cyan
python -m platformio run --target clean
if (Test-Path .pio) { Remove-Item -Recurse -Force .pio; Write-Host ".pio folder deleted." -ForegroundColor Green }
