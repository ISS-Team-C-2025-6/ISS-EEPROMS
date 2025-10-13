#include <Wire.h>

#define EEPROM_ADDR 0x50

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000); // Wait for serial monitor to connect
  Serial.println("ISS Team C EEPROM Test");
  Wire.begin();
  Serial.println("Wire and Serial Init");
  write(0, 0x0001, 'p');
  Serial.println("wrote 0x55 to 0x0001");
  readPage(0b000, 0x0001); // 0xff 0b0110
  Serial.write("\nRead Cycle Finished");
}

void loop() {
 // nothin
}

void write(unsigned int chipSelect, uint16_t address, byte data) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}

byte read(unsigned int chipSelect, uint16_t address) {
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

void readPage(unsigned int chipSelect, uint16_t address) {
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
}