#include <GyverBME280.h>
GyverBME280 bme;


float startPressure;

void setup() {
  Serial.begin(9600);

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
  Serial.println(getAltidude());
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
  filVal += (p - filVal) * 0.8;
  return filVal;
}
