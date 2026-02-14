#include "Wire.h"

// Set the I2C address of your multiplexer. Default is 0x70.
#define TCAADDR 0x70

void tcaDisableAll() {
  Wire.beginTransmission(TCAADDR);
  Wire.write(0x00);  // disable all channels
  byte err = Wire.endTransmission();
  Serial.print("Disable-all err = ");
  Serial.println(err);  // must be 0
}

// This function selects the active channel on the multiplexer.
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i); // Send a byte with a single bit set to select the channel
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  tcaDisableAll();
  while (!Serial); // Wait for serial connection
  Serial.println("\nI2C Multiplexer Scanner");
}

void loop() {
  Serial.println("Scanning...");
  
  // Loop through each channel of the multiplexer (0 to 7)
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t); // Select the current channel
    Serial.print("TCA Port #");
    Serial.println(t);

    byte error, address;
    int nDevices;
    nDevices = 0;

    // Scan for devices on the currently selected channel
    for (address = 1; address < 127; address++) {
      if (address == TCAADDR) continue;
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      
      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.print(address, HEX);
        Serial.println(" !");
        nDevices++;
      } else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      }
    }
    if (nDevices == 0) {
      Serial.println("No I2C devices found on this channel\n");
    } else {
      Serial.println("--------------------\n");
    }
  }
  Serial.println("Done.");
  delay(5000); // Wait 5 seconds for the next scan
}
// #include <Wire.h>

// bool isPresent(byte addr) {
//   Wire.beginTransmission(addr);
//   return (Wire.endTransmission() == 0);
// }

// // TCA9548A behavior: after writing a mask, reading 1 byte returns that same mask.
// bool looksLikeTCA(byte addr) {
//   // write 0x00
//   Wire.beginTransmission(addr);
//   Wire.write((uint8_t)0x00);
//   if (Wire.endTransmission() != 0) return false;

//   Wire.requestFrom((int)addr, 1);
//   if (Wire.available() < 1) return false;
//   byte r0 = Wire.read();
//   if (r0 != 0x00) return false;

//   // write 0x01
//   Wire.beginTransmission(addr);
//   Wire.write((uint8_t)0x01);
//   if (Wire.endTransmission() != 0) return false;

//   Wire.requestFrom((int)addr, 1);
//   if (Wire.available() < 1) return false;
//   byte r1 = Wire.read();
//   if (r1 != 0x01) return false;

//   return true;
// }

// void setup() {
//   Serial.begin(115200);
//   while (!Serial);

//   Wire.begin();
//   Wire.setTimeout(20);

//   Serial.println("\n=== FIND REAL TCA9548A ===");
//   Serial.println("Scanning 0x70-0x77...");

//   for (byte addr = 0x70; addr <= 0x77; addr++) {
//     if (!isPresent(addr)) continue;

//     Serial.print("Found device at 0x");
//     Serial.print(addr, HEX);

//     if (looksLikeTCA(addr)) {
//       Serial.println("  <-- THIS IS TCA9548A");
//     } else {
//       Serial.println("  (not TCA, just another device)");
//     }
//   }
// }

// void loop() {}