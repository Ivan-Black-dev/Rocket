//==================================================================== Подлкючение бибилотек ====================================================================


// Библиоткеа SD карты
#include <SPI.h>
#include <SD.h>
const int chipSelect = 3;


// Библиотека барометра
#include <GyverBME280.h>
GyverBME280 bme;
// Переменная для запоминания начального давления
float startPressure;


// Подключение бибилотек для GPS
const uint8_t pinRX = 6;
const uint8_t pinTX = 5;
#include <iarduino_GPS_NMEA.h>
#include <SoftwareSerial.h>
iarduino_GPS_NMEA gps;
SoftwareSerial    SerialGPS(pinRX, pinTX);


// Библиотека радио
#include "nRF24L01.h"
#include "RF24.h"
RF24 radio(9, 10);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; // возможные номера труб




//==================================================================== Настройка модулей ====================================================================
void setup() {
  // Настраиваем радио
  radio.begin();            // активировать модуль
  radio.setAutoAck(1);      // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);  // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload(); // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32); // размер пакета, в байтах

  radio.openWritingPipe(address[0]);   // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x7a);   // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();        // начать работу
  radio.stopListening();  // не слушаем радиоэфир, мы передатчик

  // Настраиваем SD карту
  Serial.begin(9600);
  while (!Serial) {}
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");


  // Настраиваем GPS
  SerialGPS.begin(9600);
  gps.begin(SerialGPS);
  gps.timeOut(1500);


  // Настраиваем барометр
  if (!bme.begin(0x76)) Serial.println("Error!");
  else {
    int n = 10;
    while (n != 0) {
      startPressure = getPressure();
      n--;
    }
  }
}




//==================================================================== Основной цикл программы ====================================================================
void loop() {

  // Обнуление строк с данными
  String sendStr = "";
  String writeStr = "";


  // Отправка строки с координатами
  gps.read();
  if (!gps.errPos) {
    String sendStr = String(gps.latitude, 6) + " " + String(gps.longitude, 6); // Формируем строку для отправки
    radio.write(&sendStr, sizeof(sendStr)); // Отправляем строку
  }

  // Запись строки с высотой
  writeStr = String(getAltidude(), 2) + " " + String(millis());
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(writeStr);
    dataFile.close();
    Serial.println(writeStr);
  }
  else {
    Serial.println("error opening datalog.txt");
  }
}




//==================================================================== ФУНКЦИИ ПОЛУЧЕНИЯ И ФИЛЬТРАЦИИ ВЫСОТЫ ====================================================================
float getAltidude() {
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
