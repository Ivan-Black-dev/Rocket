#include <SPI.h>
#include <SD.h>

const int chipSelect = 3;


#include <GyverBME280.h>
GyverBME280 bme;


float startPressure;

void setup() {
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // запуск датчика и проверка на работоспособность
  if (!bme.begin(0x76)) Serial.println("Error!");
  else {
    int n = 10;
    while (n != 0) {
      startPressure = getPressure();
      n--;
    }
  }
}

void loop() {
  //Serial.println(getAltidude());
  String dataString = String(getAltidude()) + " " +  String(millis());


  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}


float getAltidude(){
  return ((startPressure - getPressure()) * 0.00750062) * 10.5;
  }

float getPressure() {
  byte n = 10;
  float p = 0;
  while (n != 0) {
    p = getPressurFiltr(bme.readPressure());
    n--;
  }
  return p;
}

float getPressurFiltr(float p) {
  static float filVal = 0;
  filVal += (p - filVal) * 0.9;
  return filVal;
}
