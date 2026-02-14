#include <Wire.h>

#define EEPROM_ADDR 0x51

void setup() {
  Serial.begin(115200);
  Wire.begin();
  byte add = 0x0000;
  // pinMode(LED_BUILTIN, OUTPUT);
  delay(1000); // Wait for serial monitor to connect

  Serial.println("ISS Team C EEPROM Test");
  Serial.println("Wire and Serial Init");
  // uint32_t i = 0;
  // for (int j = 0; j < 32000; j++) {
  //   write(0, add+i, 0xAA);

  //   if ((j%1000) == 0) {
  //     Serial.println(j);
  //   }
  //   i++;
  // }

  // Serial.println("successfully wrote and left no crumbs");
  // delay(500);


  for (int k=0; k<500; k++) {
    readPage(0b000, 64*k); 
    // Serial.println(k);
  }

  Serial.write("\nRead Cycle Finished");
}

void loop() {
  //   Wire.begin();

  //   for (uint8_t i = 0; i < 64; i+=2) {
  //   Serial.println("loop");
  //   write(0, add+i, 0xAA); //10101010
  //   write(0, add+i+1, 'p');
  // }
}

void write(unsigned int chipSelect, uint32_t address, byte data) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}


void readPage(unsigned int chipSelect, uint32_t address) {
  const uint16_t TOTAL = 64;
  uint16_t off = 0;
  Serial.println("readpage running");
  
  while (off < TOTAL) {
    // Set the EEPROM's internal address pointer
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write(((address + off) >> 8) & 0xFF);   // MSB
    Wire.write((address + off) & 0xFF);          // LSB
    Wire.endTransmission(false);                 // repeated start;
    // Request up to 32 bytes (safe for AVR)
    uint8_t want = (TOTAL - off > 32) ? 32 : (TOTAL - off);
    uint8_t got  = Wire.requestFrom((uint8_t)EEPROM_ADDR, want);
    
    // Print exactly what we received, 2-digit hex
    for (uint8_t i = 0; i < got; i++) {
      uint8_t b = Wire.read();
      if (b < 16) Serial.print("0");
      Serial.print(b, HEX);
      Serial.print(" ");
      if (b!=0xAA){
        Serial.print(b, HEX);
        Serial.print(" ");
      }
    }

    off += got;
    if (got == 0) Serial.print("GOTISZERO"); // bail if something went wrong
  }
  Serial.println();
}
