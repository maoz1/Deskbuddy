# One-shot: build + upload + open the serial monitor immediately.
# Most common dev loop.
param([string]$Port = "")
Write-Host "=== Build + Upload + Monitor ===" -ForegroundColor Cyan
$uploadArgs = @("-m","platformio","run","--target","upload","--target","monitor","--monitor-port","115200")
if ($Port) { $uploadArgs += @("--upload-port",$Port,"--monitor-port",$Port) }
& python @uploadArgs
