/*#include <SPI.h>
#include <SD.h>
const int chipSelect = 8;
int z=0;*/

//#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
/*#include <MHZ.h>
#include "FastLED.h"
#include <BMP180.h>


//Anzahl der LEDs
#define NUM_LEDS 23
CRGB leds[NUM_LEDS];
*/
//SCD30
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;
float scd30_co2;
float scd30_temp;
float scd30_hum;



/*OLED
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= Stern/   U8X8_PIN_NONE); 
*/
//Aktivitätsanzeige
boolean state = true;

//Mittelwertbildung
float scd30_co2Value[5] = {400, 401, 402, 403, 404};

void setup() {
  Wire.begin();
  Serial.begin(9600);

 // setup_u8g2();
  setup_scd30();
  //setup_scd30_ForcedRecalibration();  //Recalbration erst nach mind. 2 Minuten in stabiler "400ppm" Umgebung durchführen
  //LedAnfang ();
  delay(2000);
/*
Serial.print("Initializing SD card...");

if (!SD.begin(chipSelect)){
  Serial.println("Card failed, or not present");
  return;
}
Serial.println("card initialized");
*/
}

void loop() {
  //bmp180_auslesen();
  scd30_auslesen();
 // werte_oled_anzeigen();
  //LED_anzeige ();
  delay(1000);
  /*File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile){
    dataFile.println("1");
    dataFile.println(z);
    dataFile.close();

    Serial.print("Data stored to SD-Card: ");
    Serial.println(z);
    delay(1000);
  }
else{
  Serial.println("error opening datalog.txt");
}
z++;
*/
}

/*####################### Unterprogramme SETUP ########################
void setup_u8g2 () {
  u8g2.begin();

  //FastLED.addLeds<NEOPIXEL, 5>(leds, NUM_LEDS);
  //FastLED.setBrightness(  100 );
}
*/
void setup_scd30_ForcedRecalibration() {
  airSensor.setForcedRecalibrationFactor(400);
  Serial.println("ForcedRecalibration DONE!");
}


void setup_scd30() {

  Serial.println("SCD30 Example");


  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  airSensor.setMeasurementInterval(2); //Change number of seconds between measurements: 2 to 1800 (30 minutes)

  //My desk is ~1600m above sealevel
  airSensor.setAltitudeCompensation(600); //Set altitude of the sensor in m

  //Pressure in RW ist .... mbar
  airSensor.setAmbientPressure(960);


  airSensor.setTemperatureOffset(0);
  float offset = airSensor.getTemperatureOffset();
  Serial.print("Current temp offset: ");
  Serial.print(offset, 2);
  Serial.println("C");

}

/*void LedAnfang () {
  for (int x = 1; x < 4; x++) {
    for (int n = 1; n <= NUM_LEDS; n++) {

      for (int i = 0; i < n; i++) {
        leds[i] = CRGB::Red;
      }
      for (int i = n; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
      }

      FastLED.show();
      delay(20);
    }
    for (int n = NUM_LEDS; n >= 1; n--) {

      for (int i = 0; i < n; i++) {
        leds[i] = CRGB::Red;
      }
      for (int i = n; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
      }

      FastLED.show();
      delay(20);
    }
  }
}
*/

//###############Unterprogramme LOOP #############################

void scd30_auslesen() {
  if (airSensor.dataAvailable()  )
  {

    //    airSensor.setTemperatureOffset(scd30_temp - bmp_temp);
    //    float offset = airSensor.getTemperatureOffset();
    //    Serial.print("Current temp offset: ");
    //    Serial.print(offset, 2);
    //    Serial.println("C");
    //
    //    airSensor.setAmbientPressure(bmp_press / 100);

    scd30_co2 = airSensor.getCO2();
    scd30_temp = airSensor.getTemperature();
    scd30_hum = airSensor.getHumidity();

    Serial.println("Werte SCD30: ");
    Serial.print("co2(ppm):");
    Serial.print(scd30_co2);

    Serial.print(" temp(C):");
    Serial.print(scd30_temp, 1);

    Serial.print(" humidity(%):");
    Serial.println(scd30_hum, 1);

    for (int i = 0; i < 4; i++) {               //CO2-Werte um eine Position verschieben
      scd30_co2Value[i] = scd30_co2Value[i + 1];
    }
    scd30_co2Value[4] = scd30_co2;              //aktuellen CO2-Wert anfügen

    scd30_co2 = (scd30_co2Value[0] + scd30_co2Value[1] + scd30_co2Value[2] + scd30_co2Value[3] + scd30_co2Value[4]) / 5;
    Serial.print("Mittelwert CO2 : ");
    Serial.println(scd30_co2);

    Serial.println(); Serial.println();
  }
  else
    Serial.print(".");

}
/*
void werte_oled_anzeigen() {

  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB12_tr);
  u8g2.drawStr(5, 13, "CO2-Messung");

  u8g2.setFont(u8g2_font_ncenB08_tr);  //Aktivitätsanzeige
  if (state) {
    u8g2.drawStr(62, 30, "* ");
    state = !state;
  }
  else {
    u8g2.drawStr(62, 30, " *");
    state = !state;
  }

  u8g2.setCursor(10, 56);             //Nur CO2
  u8g2.setFont(u8g2_font_ncenB24_tr);
  u8g2.print(scd30_co2, 0);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(90, 56, "ppm");

//  u8g2.setCursor(6, 50);             //CO2 und
//  u8g2.setFont(u8g2_font_ncenB18_tr);
//  u8g2.print(scd30_co2, 0);
//  u8g2.setFont(u8g2_font_ncenB08_tr);
//  u8g2.drawStr(18, 63, "ppm");
//
//  u8g2.setCursor(70, 50);             //Temperatur
//  u8g2.setFont(u8g2_font_ncenB18_tr);
//  u8g2.print(scd30_temp, 1);
//  u8g2.setFont(u8g2_font_ncenB08_tr);
//  u8g2.drawStr(94, 63, "C");
//  u8g2.drawCircle(90, 55, 1);

  u8g2.sendBuffer();
}

void LED_anzeige () {

  if (scd30_co2 <= 800) {

    int n = map(scd30_co2, 400, 800, 0, NUM_LEDS);

    //Serial.print("n :");  Serial.println(n);

    for (int i = 0; i < n; i++) {
      leds[i] = CRGB::Red;
    }
    for (int i = n; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();
  }

  else if (scd30_co2 > 800 && scd30_co2 <= 1000) {
    for (int i = 0; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
  } else {

    for (int i = 1; i <= 3; i++) {
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      delay(500);
      FastLED.show();
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      delay(500);
      FastLED.show();
    }
  }
}
*/