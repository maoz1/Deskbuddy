# Open the serial monitor (live ESP32 logs from Serial.print()).
# Press Ctrl+C to exit.
param([string]$Port = "")
Write-Host "=== Serial Monitor (Ctrl+C to exit) ===" -ForegroundColor Cyan
$args = @("-m","platformio","device","monitor","--baud","115200")
if ($Port) { $args += @("--port",$Port) }
& python @args
