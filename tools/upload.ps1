# Build + flash the ESP32. Auto-detects the COM port.
# Optional: pass port as arg, e.g.  .\tools\upload.ps1 COM5
param([string]$Port = "")
Write-Host "=== Uploading Deskbuddy ===" -ForegroundColor Cyan
$args = @("-m","platformio","run","--target","upload")
if ($Port) { $args += @("--upload-port",$Port); Write-Host "Port: $Port" }
& python @args 2>&1 | Tee-Object -FilePath upload.log
