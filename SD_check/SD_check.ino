#include <SPI.h>
#include <SdFat.h>

const uint8_t SD_CS_PIN = 10;   // change to your actual CS wiring
SdFat sd;

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  if (!sd.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(4)))) {
    Serial.println("sd.begin failed");
    sd.errorPrint(&Serial);
    while (1) {}
  }
  Serial.println("SD OK");
}

void loop() {}