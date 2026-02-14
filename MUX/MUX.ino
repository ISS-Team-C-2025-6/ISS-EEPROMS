#include <Wire.h>
#define EEPROM_ADDR0 0x50  //1010000   
#define EEPROM_ADDR1 0x51 //1010001
#define EEPROM_ADDR2 0x52  //1010010
#define EEPROM_ADDR3 0x53  //1010011


uint8_t EEPROM_ADDRESS[] = {0x52, 0x51, 0x53, 0x54}; 
uint8_t EEPROM_ADDRESS1[] = {0x55,0x56,0x51,0x50};
#define TCA_ADDR 0x70 //multiplexer

void setup() {
  Serial.begin(9600);
  // pinMode(LED_BUILTIN, OUTPUT);
  delay(1000); // Wait for serial monitor to connect
  Serial.println("ISS Team C EEPROM Test");
  Wire.begin();
  Serial.println("Wire and Serial Init");
 
  uint16_t add = 0x0000;

  TCA9548A(1);
  delay(1000);
  Serial.println("BUS 0");
  for (int i=0; i<4; i++) {
      Serial.print("TCA edit - eeprom");
      Serial.println(i);
      for (int j = 0; j < 64; j++) {
        write(0, add+j, 'm', 0x50);
        Serial.print("wrote one byte ");
      }
      Serial.print("successfully wrote eeprom ");
      Serial.println(i);
      delay(500);
      readPage(0b000,0,0x50); 
      // Serial.print("ok we read #");
      // Serial.println(i);
  }



  // TCA9548A(1);
  // delay(1000);
  // Serial.println("BUS 1");
  // for (int i=0; i<4; i++) {
  //     Serial.print("TCA edit - eeprom");
  //     Serial.println(i);
  //     for (int j = 0; j < 64; j++) {
  //       write(0, add+j, 'p', EEPROM_ADDRESS1[i]);
  //     }
  //     Serial.print("successfully wrote eeprom ");
  //     Serial.println(i);
  //     delay(500);
  //     readPage(0b000,0,EEPROM_ADDRESS[i]); 
  //     // Serial.print("ok we read eeprom");
  //     // Serial.println(i);
  // }

}

void loop() {
  readPage(0b000,0,0x50); 
}

void write(unsigned int chipSelect, uint16_t address, byte data, uint8_t ADD) {
  Wire.beginTransmission(ADD);
  Wire.write((address >>8) & 0xFF); // MSB
  Wire.write(address & 0xFF); //LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}


void readPage(unsigned int chipSelect, uint16_t address, uint8_t ADD) {
  const uint16_t TOTAL = 64;
  uint16_t off = 0;
  // Serial.println("readpage running");
  while (off < TOTAL) {
    // Serial.println("yes off is less than total");
    // Set the EEPROM's internal address pointer
    Wire.beginTransmission(ADD);
    Wire.write(((address + off) >> 8) & 0xFF);   // MSB
    Wire.write((address + off) & 0xFF);          // LSB
    Wire.endTransmission(false);                 // repeated start
    // Serial.println("msb lsb idk");
    // Request up to 32 bytes (safe for AVR)
    uint8_t want = (TOTAL - off > 32) ? 32 : (TOTAL - off);
    uint8_t got  = Wire.requestFrom((uint8_t)ADD, want);
    Serial.print("got: ");
    Serial.println(got);
    // Serial.println("requestfromed");
    // Print exactly what we received, 2-digit hex
    for (uint8_t i = 0; i < got; i++) {
      // Serial.println("trying to read one");
      uint8_t b = Wire.read();
      /*
      Serial.println("got one lets see what we got");
      Serial.println(b);
      Serial.println("pretty good reset time");
      Serial.println();
      Serial.println();
      Serial.print(" ");
      */
      // if (b < 16) Serial.print("0");
      Serial.print(b, HEX);
      // Serial.print("byteeeee");
    }

    off += got;
    if (got == 0) break; // bail if something went wrong
  }
  Serial.println();
}

void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(TCA_ADDR);  // TCA9548A address is 0x70
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  Serial.println(bus);
  delay(20);
}
