#include <LiquidCrystal.h>  
// int Contrast= 120;
 LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   

 void setup()
 { 
    lcd.begin(16, 2);
    // lcd.setCursor(0, 0);
    // lcd.print("!Welcome to ISS!");
    // delay(4000);
    // lcd.setCursor(0, 1);
    // lcd.print(":p   Yayyyy   (:");

  }  
     void loop()
 {  
    lcd.setCursor(0, 0);
    lcd.print("!Cailyn!");
    delay(2000);
    lcd.setCursor(0, 1);
    lcd.print(":p :p :p :p");
    delay(5000);
    lcd.clear();
    delay(1000);
}
