#include <IcsHardSerialClass.h>

const byte EN_PIN = 2;
// const byte EN_PIN = D0;
const long BAUDRATE = 115200;
const int TIMEOUT = 200; // softSerialは通信失敗する可能性があるため短めに
IcsHardSerialClass krs(&Serial1,EN_PIN,BAUDRATE,TIMEOUT); // インスタンス＋ENピン(2番ピン)およびUARTの設定、softSerial版
// ピコでUART制御する場合はSerial1かSerial2で指定する

void setup() {
  krs.begin(); // サーボモータの通信初期設定
}

void loop() {
  krs.setPos(0, krs.degPos(0));
  delay(2000);
  krs.setPos(0,krs.degPos(15)); // 15度
  delay(2000);
  krs.setPos(0,krs.degPos(0)); // 0度
  delay(2000);
  krs.setPos(0,krs.degPos(-15));
  delay(2000);


  // krs.setPos(0, 7500);
  // delay(500);
  // krs.setPos(0, 6500);
  // delay(500);
}