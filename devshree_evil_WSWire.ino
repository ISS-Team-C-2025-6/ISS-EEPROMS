#include <WSWire.h> 

#define eeprom 0x50 // eeprom i2c address when all address pins are GROUNDED

void setup()  {
  Wire.begin(); //creates a Wire object
  Serial.begin(9600);


  unsigned int address = 0; // keeps track of which memory address we are writing to
  Serial.println("writing num 1");

  int deviceaddress = eeprom;
  Wire.beginTransmission(deviceaddress);
  Serial.println("Transmission started.");



  for(address = 0; address< 5; address++) {
    Serial.println("Started loop 1");
    writeEEPROM(address, '1'); // writes 1 to 5 consecutive memory addresses in eeprom
    Serial.println("Finished loop 1");


  }

  Serial.println("About to restart transmission");

  //////////////////////////////////////////////////////
  //BRYANT - THIS IS WHERE THE CODE HANGS
  Serial.println(Wire.endTransmission());
  Serial.println("Transmission restarted");
  delay(1000);
 

  delay(5000);
  //Wire.endTransmission();
  //Serial.println("ended writing transmission")
  //delay(1000);



  for(address = 0; address< 5; address++) {
    Serial.println("Started READING loop");
    Serial.println(readEEPROM(deviceaddress, address)); // reads from 5 consecutive memory addresses in eeprom and prints it
    Serial.println("function ran");
  }

  Serial.println("All iterations of Reading loop completed");

  Wire.endTransmission();
  Serial.println("Transmission ended");
  }


void loop() {
  // ps we gotta avoid this because we aren't consistently writing/reading
}

//defines the writeEEPROM function
void writeEEPROM(unsigned int eeaddress, byte data ) {
  //Serial.println("begun transmission");
  Wire.write((int)(eeaddress >> 8));      //writes the MSB
  //Serial.println("msb written");
  Wire.write((int)(eeaddress & 0xFF));    //writes the LSB
  //Serial.println("lsb written");
  Wire.write(data);
  //Serial.println("data written");

  }

//defines the readEEPROM function
byte readEEPROM(int deviceaddress, unsigned int eeaddress) {
  byte rdata = 0xFF;
  
  //Wire.write((int)(eeaddress >> 8));      //writes the MSB
  //Wire.write((int)(eeaddress & 0xFF));    //writes the LSB
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) 
    rdata = Wire.read();
  return rdata;
  }

