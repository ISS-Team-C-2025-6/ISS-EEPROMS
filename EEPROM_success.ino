#include <Wire.h>

#define EEPROM_ADDR 0x50

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000); // Wait for serial monitor to connect
  Serial.println("ISS Team C EEPROM Test");
  Wire.begin();
  Serial.println("Wire and Serial Init");
  byte add = 0x0000;
  /*alternating pattern
  for (uint8_t i = 0; i < 64; i+=2) {
    write(0, add+i, 0xAA); //10101010
    write(0, add+i+1, 'p');
  }*/
  
  uint32_t i = 0;
  for (int j = 0; j < 32000; j++) {
    write(0, add+i, 0x67); //10101010 btw i removed add+ bc it confusing
    /*int reqfr = Wire.requestFrom(EEPROM_ADDR,0);
    Serial.println("yo this got requested yo");
    Serial.println(reqfr);*/

    if ((j%64) == 0) {
      Serial.println(j);
    }
    i++;
  }

  Serial.println("successfully wrote and left no crumbs");
  delay(500);


  for (int k=0; k<500; k++) {
    readPage(0b000, 64*k); 
    Serial.println(k);
  }

  Serial.write("\nRead Cycle Finished");
}

void loop() {
}

void write(unsigned int chipSelect, uint32_t address, byte data) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}

byte read(unsigned int chipSelect, uint32_t address) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.endTransmission(false);

  Serial.println(Wire.requestFrom(EEPROM_ADDR, 1));
  
  if (Wire.available()) {
    return Wire.read();
  }

  delay(5);
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
    uint8_t got  = Wire.requestFrom((uint8_t)EEPROM_ADDR, (uint8_t)32);
    // Print exactly what we received, 2-digit hex
    for (uint8_t i = 0; i < got; i++) {
      uint8_t b = Wire.read();
      if (b < 16) Serial.print("0");
      if (b!=0x67){
        Serial.print(b, HEX);
        Serial.print(" ");
      }
    }

    off += got;
    if (got == 0) break; // bail if something went wrong
  }
  Serial.println();
}

/*void readPage(unsigned int chipSelect, uint16_t address) {
  byte bytes[64];
  char line[9];

  int buffer[60];

  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.endTransmission(false);

  Wire.requestFrom(EEPROM_ADDR, 64);

  int idx = 0;
  
  while (Wire.available()) {
    bytes[idx] = Wire.read();
    idx++;
  }

  int charIdx = 0;

  for (int i = 0; i<64; i++) {
    char ascii = (char)bytes[i];
    line[charIdx] = ascii;
    charIdx = 0;

    String hex = String(bytes[i], HEX);

    Serial.print(hex);

    if (hex.length() == 1) {
      Serial.print(" ");
    }

    Serial.print(" ");
  }

  delay(5);
}*/
