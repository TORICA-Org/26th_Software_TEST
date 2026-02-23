#include <Arduino.h>
#include "Parameters.h"
#include "UARTHelper.h"

void setup() {
  initUART();
  transmitHeader();
}

void loop() {
  // ISRとはInterrupt Service Routine（割り込みサービスルーチン）の略である．
  // 割り込みハンドラ(interrupt handler)ともいい，割り込みが発生したときに起動され，対応する処理を行うもの．
  uint32_t ISR_now_time = millis(); // 現在のシステム稼働時間
  static uint32_t ISR_last_time = 0; // 最終実行時のシステム稼働時間を保持: `static`キーワードにより，静的変数として初期化され，システムが終了するまで失われない（この行が実行されるのは1回だけ）．
  if (ISR_now_time - ISR_last_time >= 10) { // 前回記録されたシステム稼働時間から10ms経過しているか？
    // 100Hzで実行
    ISR_last_time = millis(); // この時点でのシステム稼働時間を格納
    func_100Hz();
  }
}

void func_100Hz() {
  //uint32_t time_us = micros();

  // 操舵角・機体下電装部・エアデータ電装部読み取り(GPSはTWELITEを別コアで動かすため，共有するUARTを用いているGPSも別コアで動かす)
  // polling_UART();

  // フライトフェーズ判断(メインの加速度も測定)
  // determine_flight_phase();

  // 気圧センサ×3・超音波センサから高度推定
  // calculate_altitude();

  // SDに記録
  // send_SD();

  // Flight_phaseとSpeed_levelを2層目に送る
  // send_Condition();

  // if (micros() - time_us > 9900) {  //MAX10000=100Hz
  //   Serial.print("ISR100Hz_overrun!!!");
  // }
  // Serial.print("ISR_us:");
  // Serial.println(micros() - time_us);

  static uint8_t trans_count = 0;
  transmitLog(trans_count);
  trans_count++;
  if (trans_count > 3) trans_count = 0;
}