#include <Wire.h> 

#define eeprom 0x50 // eeprom i2c address when all address pins are GROUNDED

void setup()  {
  Wire.begin(); //creates a Wire object
  Serial.begin(9600); 

  unsigned int address = 0; // keeps track of which memory address we are writing to
  Serial.println("writing num 1");
  for(address = 0; address< 5; address++) 
    writeEEPROM(eeprom, address, '1'); // writes 1 to 5 consecutive memory addresses in eeprom
  
  Serial.println("write complete.")

  for(address = 0; address< 5; address++) {
    Serial.print(readEEPROM(eeprom, address), HEX); // reads from 5 consecutive memory addresses in eeprom and prints it
    }
  }

void loop() {
  // ps we gotta avoid this because we aren't consistently writing/reading
}

//defines the writeEEPROM function
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));      //writes the MSB
  Wire.write((int)(eeaddress & 0xFF));    //writes the LSB
  Wire.write(data);
  Wire.endTransmission();
  }

//defines the readEEPROM function
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));      //writes the MSB
  Wire.write((int)(eeaddress & 0xFF));    //writes the LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) 
    rdata = Wire.read();
  return rdata;
}
