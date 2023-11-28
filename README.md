| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# _BoardLock Microcontroller_

Firmware for an alarm system on an ESP32-S3 chip. Written for PlatformIO in C. 

## Description

BoardLock utilizes Bluetooth and Wi-Fi connectivity to send the status of the user's skateboard directly to the BoardLock mobile application for Android devices. If the BoardLock device is unable to connect via Bluetooth, it will automatically switch to Wi-Fi in order to keep the user connected.

The BoardLock device will have an accessible button that will allow the user to pair their mobile device via Bluetooth. There will also be a button on the inside of the encapsulation that will reset the user information and the Bluetooth connection.

An RBG LED indicator will be visible on the BoardLock device to inform the user of the device's charge and connectivity status.
* Flashing Blue = Pairing Device
* Solid Blue (For a short period of time) = Device Paired
* Solid Green = Device Fully Charged
* Flashing Orange = Device Charging
* Solid Red (For a short period of time) = Device in Locked State

When in the locked state (triggered by the mobile app), the BoardLock device will be used to detect theft threats. BoardLock will continuously check for abnormal accelerations of the board and will use that data to determine if there is an actual theft threat. If a threat is detected, a notification will appear on the user’s BoardLock mobile application, and an onboard speaker will verbally warn thieves of an impending alarm. If motion is still detected after the warning, an alarm will sound via a piezoelectric buzzer in hopes to deter the thieves.

## Disclaimer
:warning: May not be updated past 12/2023 :warning:

## How to use example
1. Install PlatformIO via VSCode Extensions.
2. Clone the project. 
3. Open [wifi.c](src/wifi.c) and edit WIFI_SSID, USERNAME, and PASSWORD. (If wifi is not WPA2, comment out line 21)
3. Flash onto a BoardLock PCB (found in [hardware_pcb](https://github.com/ECE477Group13/hardware_pcb))

## Folder contents

The project **BoardLock** contains 16 source files in C language. The file is located in folder [src](src). Each file in [src](src) has a corresponding .h file in [include](include) with function declarations and some definitions.

Below is cursory explanation of files in the project folder.

```
├── build                       Build files
├── include                     Custom include files
├── lib                         Nothing here since only native functions are used.
├── main
│   ├── CMakeLists.txt
│   ├── Kconfig.projbuild       Adds extra menuconfig options
│   ├── battbaby.c              Battery Babysitter interface using I2C functions from i2cmicro.c
│   ├── ble.c                   NimBLE code
│   ├── enables.c               Function to enable GPIO pins.
│   ├── fsm.c                   File with the finite state machine and corresponding flag update/read functions.
│   ├── gps.c                   GPS interface using I2C functions from i2cmicro.c
│   ├── hled.c                  Function to initialize the heartbeat LED
│   ├── i2cmicro.c              Functions to initialize I2C and read/write.
│   ├── i2smicro.c              Functions to initialize I2S bus to audio amplifier
│   ├── imu.c                   IMU interface using I2C functions from i2cmicro.c
│   ├── interrupt.c             Functions to initialize interrupts that poll the various peripherals and buttons
│   ├── main.c                  Main file that calls function to initialize whole system
│   ├── mqtt.c                  MQTT protocol for wifi functionality (uses a raspberry pi broker)
│   ├── rgb.c                   Function to initialize the rgb LED with custom colors.
│   ├── sd.c                    SD card interface via SPI (mounted)
│   ├── timer.c                 Timers for polling and countdown for warning
│   └── wifi.c                  Wifi initialize and deinitializing. 
├── test                        Test files (coming soon!)
├── CMakeLists.txt         
├── README.md                  This is the file you are currently reading
└── platformio.ini             Necessary for PlatformIO
```
