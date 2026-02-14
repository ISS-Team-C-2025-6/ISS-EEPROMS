/*
****************************************************************************************
****************************************************************************************
20231121
Files Required to make a complete program -
 CLI_V1.0, Quest_CLI.h, Quest_Flight.h, Quest_flight.cpp
     Important Functions and their use:
         cmd_takeSphot
             This function will use the serial camera (the one with the cable) to take a photo (jpg)
             on the transfer of this photo to the Host controller it will create a text file (txt)
             with the same file name as the jpg file of the conditions of when the jpg was taken.
             NOTE: add2text can be used with this command
         cmd_takeSpiphoto
             This function is for the SPI camera (the one that plugges directly on the microlab board)
             it will take a jpg the same as the serial camera, also creating a txt file.  Also NOTE: the
             add2text function can be used on the photo text file.
         nophotophoto
             if a camera is not being used, then this will simulate the camera photo operation. The text file
             is still created, so the add2text function can be used the get data downloaded in the text file
         nophoto30K
             this dois not use a camera, instead it uses the file space for the photo the containe ascii data
             using dataappend function defined below.  The data will still have a jpg etension, but the file
             will contain ascii of the data put in buy the dataappend unction. Plus a text file is generated,
             and the add2text function will add more data.
         -----
         add2text(int value1,int value2,int value3)
             this function will take the three varables and place them at the end of the text file (the file is
             limited the 1024 charators),  if you look at the function the text output can be formatted is almost
             any way.  Look at it and change it to acccomadate your data...
         dataappend(int counts,int ampli,int SiPM,int Deadtime)
             this function will add ascii data to the 30K byte data buffer that was used a a jpg photo.  look at
             the data formating and change it necessary for you project information.  To view the data, you can
             use a hex/ascii file viewer or change the ext to a txt.  then it should be viewable with a text exitor.
.
******************************************************************************************                                  
******************************************************************************************                 
*/
#include "Quest_Flight.h"
#include "Quest_CLI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct Bitflip {
  char original_byte;
  char flipped_byte;
};


enum PageStatus {
  READ_SUCCESS,
  READ_FAILURE,
  TOO_MANY_FLIPS,
  WHOLE_PAGE_FLIPPED,
};


struct Page {
  char* byte_array;
  PageStatus success;
};


class Result {
public:
  PageStatus success;
  int not_bit_flip_count;
  int bit_flip_count;
  Bitflip* bitflips;
  Result(const Page& page, char o, int page_size) {
    if (page.success == READ_FAILURE) {
      not_bit_flip_count = 0;
      bit_flip_count = 0;
      bitflips = nullptr;
      success = READ_FAILURE;
      return;
    }

    bitflips = (Bitflip*)calloc(page_size, sizeof(Bitflip));
    not_bit_flip_count = 0;
    bit_flip_count = 0;
    success = READ_SUCCESS;

    for (int i = 0; i < page_size; i++) {  //iterate over bytes in array
      char page_byte = page.byte_array[i];
      if (page_byte == o) {
        not_bit_flip_count++;
      } else {
        bitflips[bit_flip_count] = Bitflip{ o, page_byte };
        bit_flip_count++;
        if (bit_flip_count >= 20 && bit_flip_count < 64) {
          success = TOO_MANY_FLIPS;
        } else if (bit_flip_count >= 64) {
          success = WHOLE_PAGE_FLIPPED;
        }
      }
    }

    if (success != READ_SUCCESS) {
      free(bitflips);
      bitflips = nullptr;
    }
  }
  ~Result() {        //destructor clause
    free(bitflips);  //very important!!!
  }


  //disables copy which is important cause that way all the instances of Result can be freed
  Result(const Result&) = delete;
  Result& operator=(const Result&) = delete;
  // caller responsible for freeing memory (very important!!)
  char* to_string(int eeprom_num, int page_num) {
    int buffer_size = 100 + bit_flip_count * 30;
    char* buffer = (char*)calloc(buffer_size, sizeof(char));  // we can calculate buffer size better later.
    int len = strlen(buffer);
    if (success == READ_SUCCESS) {
      // sets up char buffer, then formats the data into the buffer.
      int len = strlen(buffer);
      snprintf(buffer + len, buffer_size - len, "%d:%d ", bit_flip_count, not_bit_flip_count);
      for (int i = 0; i < bit_flip_count; i++) {  //iterate over bitflips
        Bitflip bitflip = bitflips[i];
        int len = strlen(buffer);
        snprintf(buffer + len, buffer_size - len, " (%02X -> %02X @ %d,%d)", (uint8_t)bitflip.original_byte, (uint8_t)bitflip.flipped_byte, eeprom_num, page_num);
      }
    } else if (success == TOO_MANY_FLIPS) {
      snprintf(buffer + len, buffer_size - len, "%d:%d", bit_flip_count, not_bit_flip_count);
    } else if (success == WHOLE_PAGE_FLIPPED) {
      snprintf(buffer + len, buffer_size - len, ":( !!!");
    } else if (success == READ_FAILURE) {
      snprintf(buffer + len, buffer_size - len, "page fail");
    }
    return buffer;
  }

};
//////////////////////////////////////////////////////////////////////////
//    This defines the timers used to control flight operations
//////////////////////////////////////////////////////////////////////////
#define SpeedFactor 1  // = times faster
//
//////////////////////////////////////////////////////////////////////////
//
#define one_sec 1000           //one second = 1000 millis
#define one_min 60 * one_sec   // one minute of time
#define one_hour 60 * one_min  // one hour of time
#define one_day 24 * one_hour
constexpr uint8_t EEPROM_ADDRESS[] = { 0b1010011, 0b1010100, 0b1010010, 0b1010111 };  //new
// constexpr uint8_t EEPROM_ADDRESS[] = { 0b1010001, 0b1010101, 0b1010110, 0b1010011 }; //old
constexpr char EEPROM_NAME[] = { 'M', 'A', 'R', 'C' };
//constexpr int BII[] = { 0, 1, 2, 3 };  //cactus cactii octopus octopii bus bii
// constexpr char PATTERN = 0xAA;
//
#define TimeEvent1_time ((3 * one_day) / SpeedFactor)  //read time
#define Sensor1time ((6 * one_day) / SpeedFactor)      //make a new file time -> 6min
// #define Sensor2time         ((one_sec * 20)  / SpeedFactor)
//
//
///////////////////////////////////////////////////////////////////////////
/**
  @brief Flying function is used to capture all logic of an experiment during flight.
*/
//************************************************************************
//   Beginning of the flight program setup
//
//
void Flying() {
  int time_since_last = 0;
  int sensor1count = 0;  //counter of times the sensor has been accessed
  int sensor2count = 0;  //counter of times the sensor has been accessed
  Serial.println("\n\rRun flight program\n\r");
  //
  uint32_t TimeEvent1 = millis();    //set TimeEvent1 to effective 0
  uint32_t fileTimer = millis();     //clear sensor1Timer to effective 0
  uint32_t one_secTimer = millis();  //set happens every second
  uint32_t sec60Timer = millis();    //set minute timer
  //*****************************************************************
  //   Here to set up flight conditions i/o pins, atod, and other special condition
  //   of your program
  //
  //
  //
  //******************************************************************
  //------------ flying -----------------------
  Serial.println("Flying NOW  -  x=abort");              //terminal output for abort back to test
  Serial.println("Terminal must be reset after abort");  //terminal reset requirement upon soft reset
  missionMillis = millis();                              //Set mission clock millis, you just entered flight conditions
  //
  //
  /////////////////////////////////////////////////////
  //----- Here to start a flight from a reset ---------
  /////////////////////////////////////////////////////
  //
  DateTime now = rtc.now();                  //get time now
  currentunix = (now.unixtime());            //get current unix time, don't count time not flying
  writelongfram(currentunix, PreviousUnix);  //set fram Mission time start now counting seconds unix time
  //
  //***********************************************************************
  //***********************************************************************
  //  All Flight conditions are now set up,  NOW to enter flight operations
  //
  //***********************************************************************
  //***********************************************************************
  // write_whole_eeprom(PATTERN[0],EEPROM_ADDRESS[0]);
  // for (int i = 0; i < 4; i++) {
  //   write_whole_eeprom(0xAA, EEPROM_ADDRESS[i]);
  // }
  // write_whole_eeprom(0x41,0x53);

  while (1) {
    //----------- Test for terminal abort command (x) from flying ----------------------
    while (Serial.available()) {  //Abort flight program progress
      byte x = Serial.read();     //get the input byte
      if (x == 'x') {             //check the byte for an abort x
        return;                   //return back to poeration sellection
      }                           //end check
    }                             //end abort check
                                  //------------------------------------------------------------------
                                  //*********** Timed Event 1 test ***************************************
                                  //  Serial.println("PATTERN: ");
                                  //  Serial.println(PATTERN[0]);
                                  //  Serial.println(PATTERN[1]);
                                  //  Serial.println(PATTERN[2]);
                                  //  Serial.println(PATTERN[3]);
    if ((millis() - TimeEvent1) > TimeEvent1_time) {
      TimeEvent1 = millis();
      Serial.println("begin read loop");
      for (int i = 0; i < 4; i++) {
        delay(1000);
        Serial.println("WHATs'UP");
        read_whole_eeprom(0xAA, EEPROM_ADDRESS[i], EEPROM_NAME[i]);
        Serial.print("Reading EEPROM");
        Serial.print(i + 1);
        Serial.println(" has finished");
        nophoto30K();
        Serial.println("Finished logging data...");

      }  //end of reading
      Serial.println("one read cycle finished, see u in 3h");
      time_since_last = millis();
    }

    // ------------------------------------------------------------------
    //*******************************************************************************
    //*********** One second counter timer will trigger every second ****************
    //*******************************************************************************
    //  Here one sec timer - every second
    //
    if ((millis() - one_secTimer) > one_sec) {  //one sec counter
      one_secTimer = millis();                  //reset one second timer
      DotStarYellow();                          //turn on Yellow DotStar to Blink for running
      //
      //****************** NO_NO_NO_NO_NO_NO_NO_NO_NO_NO_NO_ *************************
      // DO NOT TOUCH THIS CODE IT IS NECESARY FOR PROPER MISSION CLOCK OPERATIONS
      //    Mission clock timer
      //    FRAM keep track of cunlitive power on time
      //    and RTC with unix seconds
      //------------------------------------------------------------------------------
      DateTime now = rtc.now();        //get the time time,don't know how long away
      currentunix = (now.unixtime());  //get current unix time
      Serial.print(currentunix);
      Serial.print(" ");                                                        //testing print unix clock
      uint32_t framdeltaunix = (currentunix - readlongFromfram(PreviousUnix));  //get delta sec of unix time
      uint32_t cumunix = readlongFromfram(CumUnix);                             //Get cumulative unix mission clock
      writelongfram((cumunix + framdeltaunix), CumUnix);                        //add and Save cumulative unix time Mission
      writelongfram(currentunix, PreviousUnix);                                 //reset PreviousUnix to current for next time
                                                                                //
                                                                                //********* END_NO_END_NO_END_NO_END_NO_END_NO_END_NO_ **************************
      //
      //  This part prints out every second
      //
      Serial.print(": Mission Clock = ");       //testing print mission clock
      Serial.print(readlongFromfram(CumUnix));  //mission clock
      Serial.print(" is ");                     //spacer
      //
      //------Output to the terminal  days hours min sec
      //
      getmissionclk();
      Serial.print(xd);
      Serial.print(" Days  ");
      Serial.print(xh);
      Serial.print(" Hours  ");
      Serial.print(xm);
      Serial.print(" Min  ");
      Serial.print(xs);
      Serial.println(" Sec");
      //
      //
      DotStarOff();
    }  // end of one second routine
  }    // End of while
}  //End nof Flighting
//
//
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//    This is a function to adds three values to the user_text_buffer
//    Written specificy for 2023-2024 Team F, Team B,
//    Enter the function with "add2text(1st interger value, 2nd intergre value, 3rd intergervalue);
//    the " - value1 " text can be changed to lable the value or removed to same space
//    ", value2 " and ", value 3 " masy also be removed or changed to a lable.
//    Space availiable is 1024 bytes, Note- - each Data line has a ncarrage return and a line feed
//
//example of calling routine:
//       //
//      int value1 = 55;
//      int value2 = 55000;
//      int value3 = 14;
//      add2text(value1, value2, value3);
//      //
//EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
//
//------end of Function to add to user text buffer ------
//
//=============================================================================
//
////FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//  Function to write into a 30K databuffer
//    char databuffer[30000];         // Create a character buffer with a size of 2KB
//    int databufferLength = 0;       // Initialize the buffer length
//  Append data to the large data buffer buffer always enter unit time of data added
//  enter: void dataappend(int counts, int ampli, int SiPM, int Deadtime) (4 values)
//


void dataappend(const char* page) {  //entry, add line with values to databuffer
  //----- get and set time to entry -----
  DateTime now = rtc.now();  //get time of entry
                             //  char* time_buffer = (char*)calloc(64, sizeof(char));
                             //  snprintf(time_buffer, 16, "%lu", (unsigned long)now.unixtime()); //avoid calloc every time stamp
                             //  appendToBuffer(time_buffer);                    //Sent unix time to databuffer
                             //  free(time_buffer);
                             //  //----- add formated string to buffer -----
                             //  appendToBuffer(page);                     //Send formated string to databuff
                             //  //
                             //  //  Serial.println(databufferLength);                                   //print buffer length for testing only
  char time_buffer[16];
  snprintf(time_buffer, 16, "%lu", (unsigned long)now.unixtime());  //avoid calloc every time stamp

  appendToBuffer(time_buffer);
  appendToBuffer(" - ");
  appendToBuffer(page);
  appendToBuffer("\r\n");
}
//-----------------------                                               //end dataappend
//----- sub part od dataappend -- append to Buffer -----
//-----------------------
void appendToBuffer(const char* data) {                           //enter with charator string to append
  int dataLength = strlen(data);                                  //define the length of data to append
                                                                  // ----- Check if there is enough space in the buffer                           //enough space?
  if (databufferLength + dataLength < (int)sizeof(databuffer)) {  //enouth space left in buffer
    // ----- Append the data to the buffer
    strcat(databuffer, data);        //yes enough space, add data to end of buffer
    databufferLength += dataLength;  //change to length of the buffer
  } else {
    Serial.println("Buffer is full. Data not appended.");  //Not enough space, say so on terminal
  }                                                        //end not enough space
}  //end appendToBuffer                                                 //end appendToBuffer
//
void write(unsigned int chipSelect, uint16_t address, byte data, uint8_t ADD) {
  Wire.beginTransmission(ADD);
  Wire.write((address >> 8) & 0xFF);  // MSB
  Wire.write(address & 0xFF);         //LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}


struct Page readPage(unsigned int chipSelect, uint16_t address, uint8_t ADD) {
  char* page_bytes = (char*)calloc(64, sizeof(char));  // 64 char array
  Page page = { page_bytes, READ_SUCCESS };
  int page_pos = 0;
  const uint16_t TOTAL = 64;
  uint16_t off = 0;
  while (off < TOTAL) {
    // Set the EEPROM's internal address pointer
    Wire.beginTransmission(ADD);
    Wire.write(((address + off) >> 8) & 0xFF);  // MSB
    Wire.write((address + off) & 0xFF);         // LSB
    Wire.endTransmission(false);                // repeated start
    // Request up to 32 bytes (safe for AVR)
    uint8_t want = (TOTAL - off > 32) ? 32 : (TOTAL - off);
    uint8_t got = Wire.requestFrom((uint8_t)ADD, want);
    // Print exactly what we received, 2-digit hex
    for (uint8_t i = 0; i < got; i++) {

      uint8_t b = Wire.read();
      // Serial.print(b);


      page.byte_array[page_pos] = b;
      page_pos++;
    }
    off += got;
    if (got != want) {
      free(page_bytes);
      page.byte_array = nullptr;
      page.success = READ_FAILURE;
      return page;
    }  // bail if something went wrong
  }
  return page;
}
void write_whole_eeprom(byte data, uint8_t ADD) {
  for (int j = 0; j < 32000; j++) {
    if (j % 6400 == 0) Serial.println("wrote 100 pages!");
    write(0, j, data, ADD);
  }
}
void read_whole_eeprom(char data, uint8_t ADD, char eeprom_name) {
  Serial.println("1");
  char time_buffer[32];
  Serial.println("2");
  snprintf(time_buffer, 32, "clock: %lu\n EEPROM%c", (unsigned long)rtc.now().unixtime(), eeprom_name);
  Serial.println("3");
  appendToBuffer(time_buffer);
  Serial.println("4");
  appendToBuffer("\r\n");
  Serial.println("5");
  int problem_count = 0;
  for (int k = 0; k < 500; k++) {
    Serial.println(k);
    Page page = readPage(0b000, 64 * k, ADD);
    Result result(page, data, 64);
    if (result.success == WHOLE_PAGE_FLIPPED) problem_count++;
    free(page.byte_array);
    char* result_str = result.to_string(ADD, k);
    appendToBuffer(result_str);
    appendToBuffer("\r\n");
    free(result_str);
  }
  if (problem_count >= 500) write_whole_eeprom(data, ADD);
}