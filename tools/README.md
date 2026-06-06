# Deskbuddy Dev Utilities

PowerShell scripts for common dev tasks. Run from the project root:

```powershell
.\tools\build.ps1
```

| Script | What it does |
|---|---|
| `build.ps1` | Compile the firmware. Live output + saved to `build.log`. |
| `upload.ps1 [COMx]` | Compile + flash to ESP32. Optional COM port. |
| `monitor.ps1 [COMx]` | Open serial monitor at 115200 baud (live ESP32 logs). Ctrl+C to exit. |
| `flash-and-watch.ps1 [COMx]` | Build + upload + open monitor (the main dev loop). |
| `watch-log.ps1 [file]` | Tail `build.log` (or any file) in real time. |
| `ports.ps1` | List connected serial devices to find your COM port. |
| `clean.ps1` | Wipe `.pio/` build cache (forces a fresh build). |

## If PowerShell blocks the scripts
Run once (lets local scripts run, signed remote scripts still required):
```powershell
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```
