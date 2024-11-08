//Oxigen Sensor
#include "DFRobot_OxygenSensor.h"

/**
 * i2c slave Address, The default is ADDRESS_3.
 * ADDRESS_0   0x70  i2c device address.
 * ADDRESS_1   0x71
 * ADDRESS_2   0x72
 * ADDRESS_3   0x73
 */
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER  10             // collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;
float oxygenData =0;


//SCD30
#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

//Oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//DS3231 
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

//SD
#include <SPI.h>
#include <SD.h>
const int chipSelect = 53;
int z=0;

//MQ7 Kohlenmonoxid

#include "MQ7.h"

#define A_PIN 0
#define VOLTAGE 5

// init MQ7 device
MQ7 mq7(A_PIN, VOLTAGE);

void setup() {
//Oxigen Sensor
  Serial.begin(9600);
  while(!oxygen.begin(Oxygen_IICAddress)){
    Serial.println("I2c device number error !");
    delay(1000);
  }
  Serial.println("I2c connect success !");

  //SCD 30
  Serial.println("SCD30 Example");
  Wire.begin();

  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }

  //The SCD30 has data ready every two seconds
  
  //oled
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

//DS3231
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

    Serial.println("Setting the time...");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
//
//rtc.adjust(DateTime(2024, 10, 18, 14, 51, 0));

//MQ7 Kohlenmonoxid

	while (!Serial) {
		;	// wait for serial connection
	}

	Serial.println(""); 	// blank new line

	Serial.println("Calibrating MQ7");
	mq7.calibrate();		// calculates R0
	Serial.println("Calibration done!");

    //SD
    Serial.print("Initializing SD card...");

if (!SD.begin(chipSelect)){
  Serial.println("Card failed, or not present");
  return;
}
Serial.println("card initialized");
  }



void loop() {
//Oxigen Sensor
  oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);
  Serial.print(" oxygen concentration is ");
  Serial.print(oxygenData);
  Serial.println(" %vol");


//SCD 30
  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature() -3.3, 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }else{
  Serial.println("failur to find SCD 30 Data");
}
delay(500);



//Oled
  display.clearDisplay();
  delay(1000);
OledSCD30();
delay(2000);



//DS3231

    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.println();
    delay(1000);

    //MQ7 Kohlenmonoxid 

    	Serial.print("Kohlenmonoxid [PPM] = "); Serial.println(mq7.readPpm());

	Serial.println(""); 	// blank new line

    //SD

      File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile){
    dataFile.print("O2 gehalt[%]  ");
    dataFile.println(oxygenData);
        dataFile.print("CO2 gehalt[ppm]  ");
    dataFile.println(airSensor.getCO2());
        dataFile.print("temp(C)  ");
    dataFile.println(airSensor.getTemperature() -3.3, 1);
        dataFile.print("humidity(%)  ");
    dataFile.println(airSensor.getHumidity(), 1);
    dataFile.print("Kohlenmonoxid [PPM] = "); dataFile.println(mq7.readPpm());
        dataFile.print("Tag:  ");
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(" (");
    dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    dataFile.println(") ");
    dataFile.print("Uhrzeit:  ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);

    dataFile.close();

    Serial.print("Data stored to SD-Card: ");
    Serial.println(z);
    delay(1000);
  }
else{
  Serial.println("error opening datalog.txt");
}
z++;
//Oled
  display.clearDisplay();
delay(1000);
OledO2();
delay(2000);

}

void OledSCD30(){
  if (airSensor.dataAvailable()){
    display.setCursor(0, 10);
  // Display static text
  display.println("CO2 gehalt:  ");
  display.println(airSensor.getCO2());
    display.println("temp(C):  ");
  display.println(airSensor.getTemperature() -3.3, 1);
    display.println("humidity(%):  ");
  display.println(airSensor.getHumidity(), 1);
  display.display();  
  }
delay(3000);
}

void OledO2(){
    display.setCursor(0, 10);
  // Display static text
  display.println("O2 gehalt:  ");
  display.print(oxygenData);   display.println("%");
  display.print("CO gehalt:  ");   display.print(mq7.readPpm());   display.println("ppm");
    
    //DS3231
    DateTime now = rtc.now();
    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.print(now.day(), DEC);
    display.print(" (");
    display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    display.println(") ");
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();
  display.display();  
  }

