#include <SoftwareSerial.h>
#include "config.h"
#include "HT1632.h"

#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6
#define DEBUG false

states sys_state = READ_STATE;

//SoftwareSerial impSerial(8, 9);
SoftwareSerial impSerial(10, 11);
String s;
String dataFeed;

String top = "top!top!top!";
String bottom = "bot!bot!bot!";
String top2 = "init";
String bottom2 = "init";
String bikeText = "init";
String weather = "init";

// use this line for three matrix
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3);

void setup() {
  Serial.begin(9600);
  impSerial.begin(9600);
  
  matrix.begin(HT1632_COMMON_16NMOS);  
  matrix.fillScreen();
  matrix.clearScreen();
  delay(500);
}

void loop() {
  if (impSerial.available() > 0) {
    s = impSerial.readString();
    Serial.println(s);
    if (s.length() > 3) {
      sys_state = PARSE_STATE;
    } else {
      weather = s;
    }
  }
   
  // draw some text!
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  matrix.setTextColor(1);   // 'lit' LEDs
  
//  Serial.println("LOOP Write");
  writeStations(top, bottom);
  
  switch (sys_state)
  {
    case WRITE_STATE:
    {
      Serial.println("WRITE");
      matrix.clearScreen();
      
      top = trains[0] + " " + trains[1] + "M";
      bottom = trains[2] + " " + trains[3] + "M";
      top2 = trains[4] + " " + trains[5] + "M";
      bottom2 = trains[6] + " " + trains[7] + "M";
      bikeText = trains[8] + " Bikes";
      weather = weather + " Deg F";
      
//      Serial.println("Section 1");
      writeStations(top, bottom);
      delay(8000);
      
      matrix.clearScreen();
      
//      Serial.println("Section 2");
      writeStations(top2, bottom2);
      delay(10000);
      
      matrix.clearScreen();
      
//      Serial.println("Bikes");
      writeBike(bikeText, weather);
      delay(5000);

      matrix.clearScreen();
      
      writeStations(top, bottom);
      
      Serial.println("Printed to display...moving to read");
      
      sys_state = READ_STATE;
      break;
    }
    case READ_STATE:
    {
      break;
    }
    case PARSE_STATE:
    {
      delay(100);
      
      Serial.println("Cleaning...");
      for (int i = 0; i < 9; i++) {
        trains[i] = "";
      }
      
      Serial.println("Parsing...");
      for (int i = 0; i < 9; i++) {
        trains[i] = getValue(s, '/', i);
      }
      
      delay(100);
      
      stationTranslator();
      
      if (DEBUG) {
        parseDebug();
      }
      
      delay(100);
      
      Serial.println("...Done");
      sys_state = WRITE_STATE;
      
      break;
    }
    case ERROR_STATE:
    default:
    {
      break;
    }
  }

}

void writeStations(String x, String y) {
  
  
  
  matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
  matrix.print(x);
  matrix.setCursor(0, 8);   // next line, 8 pixels down
  matrix.print(y);
  matrix.writeScreen();
}

void writeBike(String x, String y) {
  matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
  matrix.print(x);
  matrix.setCursor(0, 8);   // next line, 8 pixels down
  matrix.print(y);
  matrix.writeScreen();
}

void stationTranslator() {
  for (int i = 0; i < 9; i++) {
    if (trains[i] == "Shady") {
      trains[i] = "S Grove ";
    } else if (trains[i] == "Silve") {
      trains[i] = "S Spring";
    } else if (trains[i] == "Train") {
      trains[i] = "Train   ";
    } else if (trains[i] == "No Pa") {
      trains[i] = "No Pass ";
    } else if (trains[i] == "Grosv") {
      trains[i] = "Gros-Str";
    } else if (trains[i] == "Glenm") {
      trains[i] = "Glenmont";
    } else if (trains[i] == "No Tr") {
      trains[i] = "No Train";
    } else {
//      trains[i] = trains[i].substring(0,7);
      trains[i] = trains[i];
    }
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
  
void parseDebug() {
  for (int i = 0; i < 9; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(trains[i]);
  }
}
  
