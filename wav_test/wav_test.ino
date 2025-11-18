/*
以下のライブラリを使用
https://github.com/aloseed/RP2040-PWM-Audio/tree/main

*/
#include "rp2040_pwm_audio.h"
#include "wav.h"

#define PIN_BUZZER 15

// 再生間隔(ms)
#define PLAY_INTERVAL 3000

void setup()
{
  // PWMオーディオ初期化(引数に出力ピンを指定)
  pwm_audio_init(PIN_BUZZER);
  // データ(rawデータ配列)をセット
  set_sound(wav, sizeof(wav) / sizeof(wav[0]));
}

void loop()
{
  play_pwm_audio();
  delay(PLAY_INTERVAL);
}