#include <Arduino.h>
#include "UARTHelper.h"

//Analog UARTの宣言 (<- ?)
#define SerialICS Serial1 //ICSのUART // `SerialICS`を`Serial1`としてマクロを登録
#define SerialUnder Serial2 //UnderのUART // `SerialUnder`を`Serial2`としてマクロを登録

//PIO UARTの宣言
SerialPIO SerialGPS(2, 3, 1024); // GPS
SerialPIO SerialESP(17, 16, 1024); // ESP32との通信
// SerialPIO.h内プロトタイプ宣言：`SerialPIO(pin_size_t tx, pin_size_t rx, size_t fifoSize = 32);`
// 本プログラムにおいては`fifosize`について考慮の余地あり

char trans_buff[512]; // 送信する文字列を保存するためのバッファ


void initUART () {
  // ===== Note ============================
  // 25代でのボーレートと設定（今回は適当）
  // SerialWireless.begin(9600);
  // SerialSD_ICS.begin(115200,SERIAL_8E1);
  // SerialAir.begin(460800);
  // SerialUnder.begin(460800);
  // SerialCondition.begin(9600);
  // Serial.begin(115200);
  // =======================================

  //UARTピン設定
  SerialICS.setTX(0);
  SerialICS.setRX(1);
  SerialUnder.setTX(4);
  SerialUnder.setRX(5);

  // UART初期化（<-まだ通信の開始処理はされていない）
  SerialICS.setFIFOSize(1024); // バッファ(受信したデータの一時保管場所)サイズ指定(1024byte)
  SerialUnder.setFIFOSize(1024); // バッファ(受信したデータの一時保管場所)サイズ指定(1024byte)

  // パラメータ設定とともに通信を開始
  // ICS通信の仕様に合わせ，`SERIAL_8E1`としている．
  // `8`:データビットの長さ
  // `E`:偶数パリティ(`N`:パリティなし，`O`:奇数パリティ)
  // `1`:ストップビット(データフレームの終わりを示すビット)の長さ
  // デフォルトでは`SERIAL_8N1`となっている．
  SerialICS.begin(115200, SERIAL_8E1);
  SerialGPS.begin(9600, SERIAL_8E1);
  SerialESP.begin(460800, SERIAL_8E1);
  SerialUnder.begin(460800, SERIAL_8E1);
  
  Serial.begin(115200); // デバッグ用にパリティはいらないかな
  Serial.print("loading...\n\n");
}


void transmitHeader() {
  // この関数は`setup()`内なのでブロッキング関数（処理の流れが止まる関数）であっても構わない
  for (int i = 0; i < 5; i++) {

    switch (i) {
      case 0: {
        sprintf(trans_buff, "time_ms, data_air_bno_accx_mss, data_air_bno_accy_mss, \
          data_air_bno_accz_mss, data_air_bno_qw, data_air_bno_qx, data_air_bno_qy, \
          data_air_bno_qz, data_air_bno_roll, data_air_bno_pitch,"); //10個
      }
      case 1: {
        sprintf(trans_buff, "data_air_bno_yaw, estimated_altitude_lake_m, \
          altitude_bmp_urm_offset_m.get(), flight_phase, speed_level, \
          data_air_bmp_pressure_hPa, data_air_bmp_temperature_deg, data_air_bmp_altitude_m, \
          data_under_bmp_pressure_hPa, data_under_bmp_temperature_deg,"); //10個
      }
      case 2: {
        sprintf(trans_buff, "data_under_bmp_altitude_m, data_under_urm_altitude_m, \
          data_air_bmp_pressure_hPa, data_air_bmp_temperature_deg, data_air_bmp_altitude_m, \
          data_air_sdp_differentialPressure_Pa, data_air_sdp_airspeed_ms, \
          data_air_AoA_angle_deg, data_air_AoS_angle_deg,"); //10個
      }
      case 3: {
        sprintf(trans_buff, "data_ics_angle, data_air_bno_accx_mss, data_air_bno_accy_mss, \
          data_air_bno_accz_mss, data_air_bno_qw, data_air_bno_qx, data_air_bno_qy, \
          data_air_bno_qz, data_air_bno_roll, data_air_bno_pitch, data_air_bno_yaw,"); //10個
      }
      case 4: {
        sprintf(trans_buff, "data_air_gps_hour, data_air_gps_minute, data_air_gps_second, \
          data_air_gps_centisecond, data_air_gps_latitude_deg, data_air_gps_longitude_deg, \
          data_air_gps_altitude_m,data_air_gps_groundspeed_ms\n"); // 8個
      }
    }

    //バッファをクリアしてから新しいデータを書き込み
    SerialESP.flush();
    SerialUnder.flush();
    SerialESP.print(trans_buff);
    SerialUnder.print(trans_buff);

    delayMicroseconds(10); // 遅延あったほうがいいと思う
  }
}


void transmitLog (int trans_mode) { // 関数分けるのは面倒なので引数（0~3）でモード変更
  switch (trans_mode) {
    case 0: {
      sprintf(trans_buff, "%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",  //10個
        time_ms, takeoff, speed_level, data_air_bno_accx_mss, data_air_bno_accy_mss, 
        data_air_bno_accz_mss, data_air_bno_qw, data_air_bno_qx, data_air_bno_qy, data_air_bno_qz);
      break;
    }
    case 1: {
      sprintf(trans_buff, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", //10個
        data_air_bno_roll, data_air_bno_pitch, data_air_bno_yaw, data_air_bno_cal_system,
        data_air_bno_cal_gyro, data_air_bno_cal_accel, data_air_bno_cal_mag, data_air_bmp_pressure_hPa,
        data_air_bmp_temperature_deg, data_air_bmp_altitude_m); 
      break;
    }
    case 2: {
      sprintf(trans_buff, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",  //10個
        data_air_gps_hour, data_air_gps_minute, data_air_gps_second, data_air_gps_centisecond,
        data_air_gps_latitude_deg, data_air_gps_longitude_deg, data_air_gps_altitude_m,
        data_air_gps_groundspeed_ms, data_air_sdp_differentialPressure_Pa, data_air_sdp_airspeed_ms);
      break;
    }
    case 3: {
      sprintf(trans_buff, "%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,",   //10個
        data_air_AoA_angle_deg, data_air_AoS_angle_deg, data_ics_angle, data_under_bmp_pressure_hPa,
        data_under_bmp_temperature_deg, data_under_bmp_altitude_m, data_under_urm_altitude_m,
        data_under_tsd20_altitude_m, estimated_altitude_lake_m, data_altitude_bmp_urm_offset_m);
      break;
    }
    default: {
      Serial.println("The parameter value is out of range.");
      break;
    }
  }

  //バッファをクリアしてから新しいデータを書き込み
  SerialESP.flush();
  SerialUnder.flush();
  SerialESP.print(trans_buff);
  SerialUnder.print(trans_buff);
}