#include "ATMegaSerialProgrammer.h"

#include "SPI.h"
#include "LittleFS.h"

#define SPI_CLOCK 1000000UL
#define EXTENDED_ADDRESS_BOUNDARY 0x10000UL
#define PAGE_SIZE 0x100UL
#define MAIN_COMMAND 0xAC
#define CHIP_ERASE_CMD 0x80
#define READ_LOCK_BITS_CMD 0x58
#define READ_HIGH_BITS_CMD 0x58
#define READ_HIGH_BITS_SECOND_CMD 0x08
#define READ_LOW_FUSE_BYTE_CMD 0x50
#define READ_EXTENDED_FUSE_BYTE_CMD 0x50
#define READ_EXTENDED_FUSE_BYTE_SECOND_CMD 0x08
#define WRITE_LOCK_BITS_CMD 0xE0
#define WRITE_LOW_FUSE_BYTE_CMD 0xA0
#define WRITE_HIGH_FUSE_BYTE_CMD 0xA8
#define WRITE_EXTENDED_FUSE_BYTE_CMD 0xA4
#define START_PROGRAMMING_CMD 0x53
#define WRITE_PAGE_CMD 0x4C
#define WRITE_LOW_CMD 0x40
#define WRITE_HIGH_CMD 0x48
#define LOAD_EXTENDED_ADDRESS_BYTE_CMD 0x4D
#define LOAD_BUSY_CMD 0xF0
#define EMPTY 0x00

#define LOW_FUSE_SETTING 0xf7
#define HIGH_FUSE_SETTING 0xD0
#define EXTENDED_FUSE_SETTING 0xFD

uint8_t pageBuffer[PAGE_SIZE];
uint16_t bufferIndex = 0;
uint32_t currentAddress = 0;
uint8_t extendedCounter = 0;

bool ATMegaSerialProgrammer::begin(const String &fileName, uint8_t pinReset, uint8_t pinProgEn) {
    if (fileName.isEmpty() || !fileName.endsWith(".hex"))
        return false;

    LittleFS.begin();
    sketchFile.close();
    sketchFile = LittleFS.open(fileName, "r");
    if (!sketchFile) return false;

    return begin(pinReset, pinProgEn);
}

bool ATMegaSerialProgrammer::begin(uint8_t pinReset, uint8_t pinProgEn) {
    extendedCounter = 0;
    bufferIndex = 0;
    currentAddress = 0;
    memset(pageBuffer, 0xFF, PAGE_SIZE);
    isProgramming = false;

    if (pinReset != 0xFF) {
        resetPin = pinReset;
    } else {
        resetPin = SS;
    }
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);

    if (pinProgEn != 0xFF) {
        progBPin = pinProgEn;
        pinMode(progBPin, OUTPUT);
        digitalWrite(progBPin, LOW);
    }
    delay(50);

    pinMode(SCK, OUTPUT);
    digitalWrite(SCK, LOW);
    digitalWrite(resetPin, HIGH);
    delay(10);
    digitalWrite(resetPin, LOW);
    delay(20);
    SPI.end();
    SPI.begin();
    SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
    transferInstruction(MAIN_COMMAND, START_PROGRAMMING_CMD, EMPTY, EMPTY);
    busyWait();
    return true;
}

void ATMegaSerialProgrammer::chipErase() {
    transferInstruction(MAIN_COMMAND, CHIP_ERASE_CMD, EMPTY, EMPTY);
    busyWait();
}

uint8_t ATMegaSerialProgrammer::transferInstruction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    SPI.transfer(a);
    SPI.transfer(b);
    SPI.transfer(c);
    return SPI.transfer(d);
}

void ATMegaSerialProgrammer::busyWait() {
    byte busybit;
    do {
        busybit = transferInstruction(LOAD_BUSY_CMD, EMPTY, EMPTY, EMPTY);
    } while (busybit & 0x01);
}

bool ATMegaSerialProgrammer::writeLockBits(uint8_t mode) {
    transferInstruction(MAIN_COMMAND, WRITE_LOCK_BITS_CMD, EMPTY, mode);
    busyWait();
    return mode == readLockBits();
}

bool ATMegaSerialProgrammer::writeLowFuseByte(uint8_t value) {
    transferInstruction(MAIN_COMMAND, WRITE_LOW_FUSE_BYTE_CMD, EMPTY, value);
    busyWait();
    return value == readLowFuseByte();
}

bool ATMegaSerialProgrammer::writeHighFuseByte(uint8_t value) {
    transferInstruction(MAIN_COMMAND, WRITE_HIGH_FUSE_BYTE_CMD, EMPTY, value);
    busyWait();
    return value == readHighFuseByte();
}

bool ATMegaSerialProgrammer::writeExtendedFuseByte(uint8_t value) {
    transferInstruction(MAIN_COMMAND, WRITE_EXTENDED_FUSE_BYTE_CMD, EMPTY, value);
    busyWait();
    return value == readExtendedFuseByte();
}

uint8_t ATMegaSerialProgrammer::readLockBits() {
    return transferInstruction(READ_LOCK_BITS_CMD, EMPTY, EMPTY, EMPTY);
}

uint8_t ATMegaSerialProgrammer::readLowFuseByte() {
    return transferInstruction(READ_LOW_FUSE_BYTE_CMD, EMPTY, EMPTY, EMPTY);
}

uint8_t ATMegaSerialProgrammer::readHighFuseByte() {
    return transferInstruction(READ_HIGH_BITS_CMD, READ_HIGH_BITS_SECOND_CMD, EMPTY, EMPTY);
}

uint8_t ATMegaSerialProgrammer::readExtendedFuseByte() {
    return transferInstruction(READ_EXTENDED_FUSE_BYTE_CMD, READ_EXTENDED_FUSE_BYTE_SECOND_CMD, EMPTY, EMPTY);
}

void ATMegaSerialProgrammer::loadExtendedAddressByte(uint8_t extendedAddressByte) {
    transferInstruction(LOAD_EXTENDED_ADDRESS_BYTE_CMD, EMPTY, extendedAddressByte, EMPTY);
    busyWait();
}

bool ATMegaSerialProgrammer::convertHexToByte(const String &hex, uint8_t &result) {
    char *endptr;
    long int value = strtol(hex.c_str(), &endptr, 16);
    if (*endptr == '\0' && value <= 0xFF && value >= 0) {
        result = static_cast<uint8_t>(value);
        return true;
    }
    return false;
}


void ATMegaSerialProgrammer::parseHexLine(const String &hexLine) {
    uint8_t recordType;
    if (!convertHexToByte(hexLine.substring(7, 9), recordType)) {
        Serial.println("Error: Invalid record type.");
        return;
    }
    uint8_t byteCount;
    if (!convertHexToByte(hexLine.substring(1, 3), byteCount)) {
        Serial.println("Error: Invalid byte count.");
        return;
    }
    if (recordType == 0) {
        int byteStartPos = 9;
        for (int i = 0; i < byteCount; ++i) {
            uint8_t dataByte;
            if (!convertHexToByte(hexLine.substring(byteStartPos, byteStartPos + 2), dataByte)) {
                Serial.println("Error: Invalid data byte.");
                return;
            }
            pageBuffer[bufferIndex++] = dataByte;
            byteStartPos += 2;

            if (bufferIndex >= PAGE_SIZE) {
                if ((currentAddress & 0xFFFFFF80) == EXTENDED_ADDRESS_BOUNDARY) {
                    loadExtendedAddressByte(++extendedCounter);
                }
                processPage(pageBuffer);
                bufferIndex = 0;
            }
        }
    } else if (recordType == 1) {
        if (bufferIndex > 0 && bufferIndex < PAGE_SIZE) {
            memset(pageBuffer + bufferIndex, 0xFF, PAGE_SIZE - bufferIndex);
            processPage(pageBuffer);
        }
    }

    unsigned int percentDone = (++linesProcessed * 100) / totalLines;
    if (percentDone != lastPercentDone) {
        Serial.print("\rProgress: ");
        Serial.print(percentDone);
        Serial.print("% done.    ");
        lastPercentDone = percentDone;
    }
}

void ATMegaSerialProgrammer::processPage(uint8_t *pageData) {
    int x = 0;
    unsigned int page = currentAddress & 0xFFFFFF80;
    while (x < 256) {
        if (page != (currentAddress & 0xFFFFFF80)) {
            transferInstruction(WRITE_PAGE_CMD, (page >> 8) & 0xFF, page & 0xFF, EMPTY);
            page = currentAddress & 0xFFFFFF80;
        }
        transferInstruction(WRITE_LOW_CMD,
                            currentAddress >> 8 & 0xFF,
                            currentAddress & 0xFF,
                            pageData[x++]);
        transferInstruction(WRITE_HIGH_CMD,
                            currentAddress >> 8 & 0xFF,
                            currentAddress & 0xFF,
                            pageData[x++]);
        currentAddress++;
    }
    transferInstruction(WRITE_PAGE_CMD, (page >> 8) & 0xFF, page & 0xFF, EMPTY);
    busyWait();
}

void ATMegaSerialProgrammer::end() {
    digitalWrite(resetPin, HIGH);
    currentAddress = 0;
    bufferIndex = 0;
    totalLines = 0;
    linesProcessed = 0;
    percentMarker = 0;
    lastPercentDone = -1;
    extendedCounter = 0;
    SPI.end();
    if (progBPin != 0xFF) {
        digitalWrite(progBPin, LOW);
        delay(200);
        digitalWrite(progBPin, HIGH);
    }
    pinMode(MOSI, INPUT);
    pinMode(SCK, INPUT);
    pinMode(resetPin, INPUT);
}

bool ATMegaSerialProgrammer::startProgramming() {
    if (isProgramming) return false;
    if (!sketchFile) return false;
    linesProcessed = 0;
    percentMarker = 0;
    lastPercentDone = -1;
    totalLines = sketchFile.size() / 44;

    chipErase();
    writeLockBits(LB_MODE_1);
    if (!writeLowFuseByte(LOW_FUSE_SETTING)) {
        Serial.println("Failed to write Low Fuse Byte");
        return false;
    } else if (!writeHighFuseByte(HIGH_FUSE_SETTING)) {
        Serial.println("Failed to write High Fuse Byte");
        return false;
    } else if (!writeExtendedFuseByte(EXTENDED_FUSE_SETTING)) {
        Serial.println("Failed to write Extended Fuse Byte");
        return false;
    }
    loadExtendedAddressByte(extendedCounter);
    isProgramming = true;
    return true;
}

bool ATMegaSerialProgrammer::loop() {
    if (!isProgramming) return false;
    if (!sketchFile) return false;

    if (sketchFile.available()) {
        parseHexLine(sketchFile.readStringUntil('\n'));
    } else {
        isProgramming = false;
        writeLockBits(0x0F);
        sketchFile.close();
        end();
    }
    return true;
}
