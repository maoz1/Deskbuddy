# Building Deskbuddy with PlatformIO (no Arduino IDE)

This is an alternative to the Arduino IDE workflow in `SETUP_GUIDE.md`. PlatformIO
Core builds and flashes entirely from the command line, auto-downloads all
libraries, and configures the display via build flags — so you do **not** need to
edit the TFT_eSPI library's `User_Setup.h`.

## 1. Install PlatformIO Core

Requires Python 3. Then:

```bash
pip install -U platformio
```

Run it with `python -m platformio ...` (or `pio ...` if the Scripts dir is on PATH).

## 2. Project layout

```
platformio.ini      # board, libraries, and display configuration
src/main.ino        # the Deskbuddy sketch (.ino enables auto-prototyping)
```

> The sketch must be `src/main.ino` (not `.cpp`). The `.ino` extension makes
> PlatformIO auto-generate function prototypes, exactly like the Arduino IDE.
> As a plain `.cpp` the build fails with "X was not declared in this scope"
> because functions are used before they are defined.

## 3. WiFi credentials

Edit the placeholders in `src/main.ino` before flashing:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
```

Do **not** commit real credentials to a public repo.

## 4. Build and flash

```bash
# Compile only
python -m platformio run

# List connected serial ports (find your board)
python -m platformio device list

# Flash to the board (replace COM7 with your port)
python -m platformio run --target upload --upload-port COM7

# Watch serial output (115200 baud) — prints the web UI IP after WiFi connects
python -m platformio device monitor --port COM7 --baud 115200
```

After a successful boot the serial monitor prints something like:

```
Deskbuddy web: http://192.168.x.x
```

Open that address in a browser on the same network to configure theme, weather
location, timezone, units, timer presets, notes, and nickname.

## 5. Display driver — IMPORTANT

The "Cheap Yellow Display" (ESP32-2432S028) ships with **two different display
controllers** depending on the batch. The wrong one produces a **solid white
screen** even though the board boots and connects to WiFi normally.

In `platformio.ini`, the driver is selected by one build flag:

| Symptom | Try this flag |
|---|---|
| Solid white screen | swap `-DILI9341_DRIVER=1` ⇄ `-DST7789_DRIVER=1` |
| Wrong/inverted colors | toggle `-DTFT_RGB_ORDER=TFT_BGR` vs `TFT_RGB` |
| Negative/inverted image | add `-DTFT_INVERSION_ON=1` (or `_OFF`) |
| Mirrored / rotated wrong | adjust `const int ROT` in `src/main.ino` |

This board was confirmed to use **`ILI9341_DRIVER`**.

## 6. Display pin map (this board)

These are set as build flags in `platformio.ini` and mirror the repo's
`User_Setup.h`:

```
TFT_MISO=12  TFT_MOSI=13  TFT_SCLK=14
TFT_CS=15    TFT_DC=2     TFT_RST=-1
TFT_BL=21    (backlight, active HIGH)
SPI_FREQUENCY=40000000
```

Touch (XPT2046) pins are defined in `src/main.ino`.
