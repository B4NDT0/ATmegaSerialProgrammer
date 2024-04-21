#ifndef ATmegaSerialProgrammer_h
#define ATmegaSerialProgrammer_h

#include "Arduino.h"
#include "LittleFS.h"

class ATmegaSerialProgrammer {
public:

    bool begin(uint8_t pinReset = 0xFF, uint8_t pinProgEn = 0xFF);
    bool begin(const String &fileName, uint8_t pinReset = 0xFF, uint8_t pinProgEn = 0xFF);
    static void chipErase();

    static bool writeLockBits(uint8_t mode);

    static bool writeLowFuseByte(uint8_t value);

    static bool writeHighFuseByte(uint8_t value);

    static bool writeExtendedFuseByte(uint8_t value);

    static uint8_t readLockBits();

    static uint8_t readLowFuseByte();

    static uint8_t readHighFuseByte();

    static uint8_t readExtendedFuseByte();

    bool loop();

    bool startProgramming();

    // Lock Bit Definitions
    const uint8_t LB_MODE_1 = 0x3; // No memory lock features enabled.
    const uint8_t LB_MODE_2 = 0x2; // Further programming of the Flash and EEPROM is disabled in Parallel and Serial Programming mode.
    const uint8_t LB_MODE_3 = 0x0; // Further programming and verification of the Flash and EEPROM is disabled. Boot Lock bits and Fuse bits are locked.

    // Extended Fuse Byte (BODLEVEL bits)
    const uint8_t EXTENDED_FUSE_DEFAULT = 0xFF; // Default state with all bits unprogrammed

    // High Fuse Byte
    const uint8_t HIGH_FUSE_OCDEN = 0x80; // Enable OCD
    const uint8_t HIGH_FUSE_JTAGEN = 0x40; // Enable JTAG
    const uint8_t HIGH_FUSE_SPIEN = 0x20; // Enable Serial Program and Data Downloading
    const uint8_t HIGH_FUSE_WDTON = 0x10; // Watchdog Timer always on
    const uint8_t HIGH_FUSE_EESAVE = 0x08; // EEPROM memory is preserved through the Chip Erase
    const uint8_t HIGH_FUSE_BOOTSZ1 = 0x04; // Select Boot Size
    const uint8_t HIGH_FUSE_BOOTSZ0 = 0x02; // Select Boot Size
    const uint8_t HIGH_FUSE_BOOTRST = 0x01; // Select Reset Vector

    // Low Fuse Byte
    const uint8_t LOW_FUSE_CKDIV8 = 0x80; // Divide clock by 8
    const uint8_t LOW_FUSE_CKOUT = 0x40; // Clock output
    const uint8_t LOW_FUSE_SUT1 = 0x20; // Select start-up time
    const uint8_t LOW_FUSE_SUT0 = 0x10; // Select start-up time
    const uint8_t LOW_FUSE_CKSEL3 = 0x08; // Select Clock source
    const uint8_t LOW_FUSE_CKSEL2 = 0x04; // Select Clock source
    const uint8_t LOW_FUSE_CKSEL1 = 0x02; // Select Clock source
    const uint8_t LOW_FUSE_CKSEL0 = 0x01; // Select Clock source

    bool isProgramming = false;

private:
    uint8_t progBPin{};
    uint8_t resetPin{};
    File sketchFile;
    unsigned int totalLines = 0;
    int linesProcessed = 0;
    int percentMarker = 0;
    uint8_t lastPercentDone = -1;

    static uint8_t transferInstruction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

    void end();

    static void busyWait();

    static void processPage(uint8_t *pageData);

    static void loadExtendedAddressByte(uint8_t extendedAddressByte);

    void parseHexLine(const String &hexLine);

    static bool convertHexToByte(const String &hex, uint8_t &result);
};

#endif
