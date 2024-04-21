# ATmegaSerialProgrammer

## Overview
ATmegaSerialProgrammer is an Arduino-compatible library designed to facilitate the programming of ATmega microcontrollers through serial communication. This library leverages the ArduinoISP protocol and SPI communication to interact directly with ATmega chips, supporting various operations including chip erase, fuse bit programming, and lock bit settings directly from your Arduino environment.

## Features
- **ArduinoISP Protocol**: Utilizes the well-established ArduinoISP protocol for reliable microcontroller programming.
- **SPI Communication**: Employs SPI (Serial Peripheral Interface) for fast data transfer and control of the programming operations.
- **OTA Programming**: Enables OTA programming of ATmega microcontrollers using Quectel or Espressif modules as programmers.
- **Easy Integration with Arduino**: Works seamlessly with Arduino setups, allowing you to program ATmega microcontrollers without additional hardware.
- **Comprehensive Fuse Bit Management**: Provides methods to read and write low, high, and extended fuse bits.
- **Lock Bit Operations**: Enable or disable further programming of the microcontroller with configurable lock bit settings.
- **Chip Erase**: Simplify the process of preparing the microcontroller for new programs.

## Installation
1. Download the library as a ZIP file from this GitHub repository.
2. Open your Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library` and select the downloaded file.
3. Include the library in your Arduino sketch as follows:
   ```cpp
   #include "ATmegaSerialProgrammer.h"
   ```

## Usage
To use the library, create an instance of the `ATmegaSerialProgrammer` class, and initialize it with the programming pin configuration. Here’s a simple example that demonstrates initiating the programmer using SPI and ArduinoISP:

```cpp
#include "ATmegaSerialProgrammer.h"

ATmegaSerialProgrammer programmer;

void setup() {
  // Initialize the programmer
  programmer.begin(10, 11); // Pin 10 for reset, Pin 11 for program enable
}

void loop() {
  // Start programming mode
  if (programmer.startProgramming()) {
    programmer.chipErase();
    // Proceed with other programming tasks like setting fuses or uploading sketches
  }
}
```

## OTA Programming
Create OTA updates for ATmega microcontrollers using this library with a Quectel or Espressif module as the programmer. This setup allows you to wirelessly update microcontrollers embedded in difficult-to-reach places or in mobile applications.

## Contributing
Contributions are welcome! We are particularly interested in:
- **Testing with More Microcontrollers**: Help us test and support more ATmega microcontrollers and different microcontroller platforms as programmers. Currently tested platforms include ESP12 as the programmer and ATmega2560 as the target.
- **Enhance OTA Capabilities**: Contribute to the development of OTA programming features using various cellular or WiFi modules.
- **Code Improvements and Bug Fixes**: If you have improvements or bug fixes, please fork the repository and use a pull request to add your features or fixes.

To create a comprehensive API Reference section for the README.md document, I'll detail each public function of the `ATmegaSerialProgrammer` library based on the functionalities you've provided. This will include descriptions for initializing the library, operations on fuse and lock bits, and managing the programming process. Here's the updated API Reference section:

---

## API Reference

Below are the detailed descriptions of the public functions provided by the ATmegaSerialProgrammer library:

- **`begin(uint8_t pinReset, uint8_t pinProgEn)`**:
    - Initializes the programmer with specified pins for the reset and program enable functions. Setting these pins sets up the SPI and prepares the ArduinoISP protocol handling. This function must be called before any programming activities.
    - Parameters:
        - `pinReset`: Pin number connected to the ATmega's reset pin.
        - `pinProgEn`: Pin number used to enable programming mode.

- **`begin(const String &fileName, uint8_t pinReset, uint8_t pinProgEn)`**:
    - Initializes the programmer and prepares a firmware file stored on LittleFS for programming. This is particularly useful for OTA updates.
    - Parameters:
        - `fileName`: The path to the firmware file stored on the filesystem.
        - `pinReset`: Pin number connected to the ATmega's reset pin.
        - `pinProgEn`: Pin number used to enable programming mode.

- **`chipErase()`**:
    - Erases the entire chip, clearing all flash memory. This is usually the first step in a comprehensive programming cycle.

- **`writeLockBits(uint8_t mode)`**:
    - Programs the lock bits of the ATmega microcontroller to control further programming and readback protections.
    - Parameters:
        - `mode`: The lock bit configuration to set.

- **`readLockBits()`**:
    - Reads the current lock bit configuration from the ATmega microcontroller.
    - Returns:
        - The lock bit value as a uint8_t.

- **`writeLowFuseByte(uint8_t value)`**:
    - Programs the low fuse byte.
    - Parameters:
        - `value`: The value to program into the low fuse byte.

- **`writeHighFuseByte(uint8_t value)`**:
    - Programs the high fuse byte.
    - Parameters:
        - `value`: The value to program into the high fuse byte.

- **`writeExtendedFuseByte(uint8_t value)`**:
    - Programs the extended fuse byte.
    - Parameters:
        - `value`: The value to program into the extended fuse byte.

- **`readLowFuseByte()`**:
    - Reads the low fuse byte from the microcontroller.
    - Returns:
        - The value of the low fuse byte as a uint8_t.

- **`readHighFuseByte()`**:
    - Reads the high fuse byte from the microcontroller.
    - Returns:
        - The value of the high fuse byte as a uint8_t.

- **`readExtendedFuseByte()`**:
    - Reads the extended fuse byte from the microcontroller.
    - Returns:
        - The value of the extended fuse byte as a uint8_t.

- **`loop()`**:
    - Processes the next line of the firmware file during a programming session. This should be called repeatedly within the Arduino `loop()` function to ensure continuous programming.

- **`startProgramming()`**:
    - Starts the programming mode, preparing the microcontroller for receiving new programs.
    - Returns:
        - `true` if successfully entered programming mode, `false` otherwise.

- **`end()`**:
    - Ends the programming session, ensuring all operations are cleanly terminated and the microcontroller is reset to run the new program.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Support
If you have any issues or feature requests, please open an issue on this repository. Your feedback is valuable in improving this library.

---