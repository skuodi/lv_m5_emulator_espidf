# lv_m5_emulator_espidf
This repo is a fork of [m5stack/lv_m5_emulator](https://github.com/m5stack/lv_m5_emulator) that's been ported to [Espressif IDF](https://github.com/espressif/esp-idf) from the [arduino-esp32](https://github.com/espressif/arduino-esp32) framework used in the `m5stack/lv_m5_emulator` repo, while maintaining PlatformIO compatibility.
As this fork is meant to remain a template for projects that use M5Stack devices, only the bare minimum of changes required to run on ESP-IDF have been made to this fork, though a few optimizations have been made to improve the user experience on hardware.

Tested on
- [M5Core2](https://docs.m5stack.com/en/core/core2) - functionally complete
- [M5Fire](https://docs.m5stack.com/en/core/fire) - needs custom event handlers in UI to receive input device events

Here are a few notes on the changes made in switching to ESP-IDF:

1. In [platformio.ini](platformio.ini), under the non-emulator build targets ([board_M5Core](platformio.ini#L90), [board_M5Core2](platformio.ini#L90), [emulator_CoreS3](platformio.ini#L120)) changed the `framework` from ` arduino` to `espidf` and added `m5stack/M5Unified` under library dependencies to implement optional button input support.
Optionally, add `esp32_exception_decoder` to the `monitor_filters` to parse the [backtrace](https://docs.espressif.com/projects/esp-techpedia/en/latest/esp-friends/advanced-development/debugging/backtrace-coredump.html) into meaningful output the event of a crash. Also configure the UART `monitor_speed` that the platformio terminal expects the device to be running at: the actual UART baud rate that the M5Stack hardware will use is configured in [menuconfig](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig.html).

2. In [main.cpp](src/main.cpp)
  - change all occurances of `#if defined(ARDUINO) && defined(ESP_PLATFORM)` to `#if defined(ESP_PLATFORM)`
  - include the [M5Unified header](src/main.cpp#L7) for input device support.
  - implement [app_main()](src/main.cpp#L28) which will be the entry point of the application. Note that `app_main()` [needs to be declared `extern "C" ` if implemented within a .cpp file](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/cplusplus.html)
  - optionally call `M5.begin()` to initialize input devices

3. In [lv_conf_v8.h](include/lv_conf_v8.h)
  - enable [LV_MEM_CUSTOM](include/lv_conf_v8.h#L49) for lvgl to use external PSRAM available on all M5Stack cores. PSRAM will be enabled in menuconfig
  - optionally, enable the [lvgl performance monitor](include/lv_conf_v8.h#L271) to see the real-time CPU usage and FPS count on screen.

4. In the platformio tab, under Project Tasks run `[build targets]` > `Platform` > `Run menuconfig` for each of the non-emulator build targets ([board_M5Core](platformio.ini#L90), [board_M5Core2](platformio.ini#L90), [emulator_CoreS3](platformio.ini#L120)). Set:
  - `Bootloader config` > `Bootloader log verbosity` to `Warning` or lower. Setting it to `Info` or higher inflates the bootloader size and it won't fit in to the space allocated on flash;
  - `Serial flasher config` > `Flash size` to `16 MB`
  - `Partition Table` > `Custom partition table CSV` enabled. The current [partitions.csv](partitions.csv) is for 16MB flash taken as-is from the [arduin-esp32 repo](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/m5stack_partitions_16MB_factory_4_apps.csv)
  - `Component config` > `ESP PSRAM` > `Support for external, SPI-connected RAM` enabled
  - optionally, `Component config` > `ESP System Settings` > `Channel for console output` to `Custom UART` to show the `UART console baud rate` option which you can set to the save value as in platformio.ini (step 1).

5. In [lvgl_port_m5stack.cpp](src/utility/lvgl_port_m5stack.cpp)
  - change all occurances of `#if defined(ARDUINO) && defined(ESP_PLATFORM)` to `#if defined(ESP_PLATFORM)`
  - optionally, include [M5Unified.h](src/utility/lvgl_port_m5stack.cpp#L4) to implement input device supportand implement read callback for [button](src/utility/lvgl_port_m5stack.cpp#L85) inputs
  - [register the input devices](src/utility/lvgl_port_m5stack.cpp#L161-L190)
  - optionally, set the frame buffer height `LV_BUFFER_LINE` to the size of the display to mitigate the tearing effect. M5Stack cores have >=4MB onboard PSRAM which is more than enough to for 2 fullscreen buffers.


## <-----------------------------------------------ORIGINAL README----------------------------------------------->

# Running the M5Stack LVGL device emulator via PlatformIO

Now it is more convenient to design [LVGL](https://github.com/lvgl/lvgl) UI on PC, 
which can reduce the time wasted in flash the firmware.
Using [M5GFX](https://github.com/m5stack/M5GFX), we can ensure that the display
effect on the PC side is consistent with that of the device.
Now we have also added a picture of the device shell as the border background, 
which can better simulate the display effect on the device side.

<div align=center>
<img src="images/lv_m5stack_fire.gif" width="25%"><img src="images/lv_m5stack_core2.gif" width="25%"><img src="images/lv_m5stack_cores3.gif" width="25%"><img src="images/lv_m5stack_dial.gif" width="24.78%">
</div>

## How to install & use demo

### Install Visual Studio Code

https://code.visualstudio.com/ - follow instructions there, if you don't have
vscode yet.


### Install SDL drivers

**Linux (Ubuntu, Debian, ...)**

Use one of options below:

```sh
# 64 bits
sudo apt-get install libsdl2-dev
```

```sh
# 32 bits
sudo apt-get install gcc-multilib g++-multilib libsdl2-dev:i386
```

Note:

- Use 32-bits build for more correct memory stat info.
- If you have conflicts on `libsdl2-dev:i386` install, remove 64-bits version
  and dependencies first.

**MacOS**

Use [Homebrew](https://brew.sh/):

```sh
brew install sdl2 pkg-config
```
Note: 
On MacOS you need to include (uncomment in provided example [platformio.ini](./platformio.ini) file) these lines in your platformio.ini file to import the drivers:
```
  ; SDL2 includes
  !pkg-config --cflags SDL2
  !pkg-config --libs SDL2
```

**Windows**

Use [MSYS2](https://www.msys2.org/)

```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2
```

Add the path to your Mingw-w64 `bin` folder to the Windows PATH environment
variable (usually `C:\msys64\mingw64\bin`). See [instruction, 4](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites).


### Install flasher drivers (optional)

If you plan to upload firmware & debug hardware, read notes in PlatformIO
[install docs](http://docs.platformio.org/en/latest/installation.html#troubleshooting).


### Choose LVGL version(V8 or V9)

If you want build with lvgl v8, this was default option, you don't need change any files!
But if you want try the lvgl  latest version v9, you need to modify the version control macro definition in [platformio.ini](./platformio.ini) file as:
```
  -D LVGL_USE_V8=0  ; lvgl v8
  -D LVGL_USE_V9=1  ; lvgl v9
```
And don't forget to modify the links of the dependent libs to this:
```
  ; lvgl=https://github.com/lvgl/lvgl/archive/refs/tags/v8.3.0.zip  ; lvgl v8
  lvgl=https://github.com/lvgl/lvgl#master  ; lvgl v9
```

### Build/Run

1. Clone this repository or download as zip.
2. In vscode, open folder via `File` -> `Open Folder...` menu.
   - If you do it first time - agree, when it suggests to install PlatformIO
     plugin, and wait when PlatformIO then install build tools and package
     dependencies.

To build/execute, on PlafomIO tab expand desired env and click target:

<img src="support/pio_explorer.png" width="60%">

Note, for emulator env `upload` also executes compiled binary.

### Zoom and rotate the window at runtime

The emulator support zoom and rotate the window at runtime. So you can easily zoom and rotate the window to view UI details.

1. Zoom

    Press :keyboard: **1 ~ 6** number key to zoom the window size.

1. Rotate

    Press :keyboard: **"L"** **"R"** letter key to rotate the window.
