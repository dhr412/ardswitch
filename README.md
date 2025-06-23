# ArdSwitch

**ArdSwitch** is a keyboard automation project that triggers an `Alt + Tab` keypress on a computer when an ultrasonic sensor (connected to an Arduino with WiFi support) detects motion.

This setup uses a Go-based HTTP server running on the host machine. When the ultrasonic sensor is triggered, the Arduino sends an HTTP request to the server, which then calls a C++ executable that simulates the keypress.

> **This project is intended for Windows only.**

---

## How It Works

The system is composed of three key parts:

### 1. **Arduino (Ultrasonic Sensor + WiFi)**

* Uses the `WiFiS3` library to connect to a local WiFi network.
* Measures distance using an ultrasonic sensor.
* If an object is detected within a threshold (`< 150 cm`) and cooldown has elapsed, it sends an HTTP `POST` request to the host server.

**Arduino script**: [`transmit.ino`](./src/arduino/transmit/transmit.ino)

### 2. **Go Server**

* A minimal HTTP server that listens for incoming `/trigger` requests (both GET and POST).
* On receiving a valid request, it executes a local binary (`autom.exe`) to simulate keypresses.
* Prints log messages with timestamps and request count.
* The server tries to print its local IP (based on `192.168.*`) for easier configuration on the Arduino side.

**Go server script**: [`server.go`](./src/server.go)

### 3. **C++ Automation Executable**

* A simple Windows application that sends a keyboard input of `Alt + Tab` using the `SendInput` API.

**Automation script**: [`autom.cpp`](./src/autom.cpp)

---

## WiFi Config Header File

The Arduino sketch includes a separate header file `wifi-config.h` for storing WiFi credentials. This file is not committed to the repository for security reasons.

Create your own `wifi-config.h` file in the same directory as the Arduino sketch with the following format:

```cpp
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#endif
```

Replace `"YOUR_WIFI_SSID"` and `"YOUR_WIFI_PASSWORD"` with your actual network credentials.

---

## Installing the Scripts

### From Releases (Recommended)

1. Head over to the [Releases](https://github.com/dhr412/ardswitch/releases) section of this repository.
2. Download the following files:

   * `server.exe` — Go-based HTTP server
   * `autom.exe` — C++ executable for automation
3. Place both files in the same directory.
4. Run the server from the terminal:

```sh
server.exe
```

By default, it listens on port `32768`. You can change this with the `-port` flag:

```sh
server.exe -port 12345
```

---

## Compiling from Source

### 1. **Go Server**

Requirements:

* [Go](https://golang.org/dl/) (version 1.20+ recommended)

To build:

```sh
go build -o server.exe server.go
```

### 2. **C++ Automation Executable**

Requirements:

* Windows system with a C++ compiler

To build (example with MSVC):

```sh
cl autom.cpp /Feautom.exe
```

Or with g++ (if using MinGW):

```sh
g++ autom.cpp -o autom.exe
```

Make sure the compiled `autom.exe` is placed in the same folder as `server.exe`.

---

## Final Notes

* Ensure the Arduino is connected to the same network as the computer running the server.
* Update the IP in `transmit.ino` (`serverUrl`) to match the host computer’s IP and make the host computer's IP constant in router settings
  > (Alternatively, set the host computer's IP address to always be 192.168.1.100 to bypass this step).
* The automation will not work if the system blocks simulated input (like in some full-screen apps or when UAC dialogs are active).
* This project assumes basic familiarity with flashing Arduino sketches and setting up a local network.
