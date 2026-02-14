#include <WSWire.h> 

#define eeprom 0x50 // eeprom i2c address when all address pins are GROUNDED

void setup()  {
  Wire.begin(); //creates a Wire object
  Serial.begin(9600);


  unsigned int address = 0; // keeps track of which memory address we are writing to
  // Serial.println("writing num 1");

  int deviceaddress = eeprom;
  // for (byte address = 1; address < 8; ++address) {
  //                           // The i2c_scanner uses the return value of
  //                           // the Wire.endTransmission to see if
  //                           // a device did acknowledge to the address.
    Wire.beginTransmission(deviceaddress);
    byte error = Wire.endTransmission();
    Serial.println(error);
    Serial.println("...");
  }

 void loop() {
//   // ps we gotta avoid this because we aren't consistently writing/reading
 }

