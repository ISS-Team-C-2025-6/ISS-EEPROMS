#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nI2C Scanner");
  Wire.begin();

  // prevents infinite hang on some boards (SAMD etc.)
  Wire.setTimeout(20);  // ms
}

void loop() {
  byte error;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) Serial.println("NO I2C DEVICES FOUND");
  else {
    Serial.print("Done. Found ");
    Serial.print(nDevices);
    Serial.println(" device(s).");
  }

  delay(2000);
}