# Tail the last build log in real time (use when a build is running in background).
param([string]$File = "build.log")
if (-not (Test-Path $File)) { Write-Host "No $File yet — start a build first." -ForegroundColor Yellow; exit }
Write-Host "=== Tailing $File (Ctrl+C to exit) ===" -ForegroundColor Cyan
Get-Content $File -Wait -Tail 30
