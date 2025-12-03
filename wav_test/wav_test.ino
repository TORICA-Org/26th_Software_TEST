//rp2040の標準ライブラリを使用．外部ライブラリ不要
//サンプリングレート16kHz，Unsigned 8bit raw

#include <PWMAudio.h>
#include "wav.h"

//スピーカー用のピン
#define speaker 15

PWMAudio pwm(speaker);

const int audio_len = sizeof(raw_audio) / sizeof(raw_audio[0]);

void setup() {
  Serial.begin(115200);
  
  // 音源のサンプリングレートが8kHzの場合
  pwm.begin(8000);
  
  // バッファサイズ調整 (必要に応じて)
  // pwm.setBuffers(4, 64); 
}

void loop() {
  Serial.println("Playing...");
  
  for (int i = 0; i < audio_len; i++) {
    // 書き込みバッファに空きができるまで待機
    while (pwm.availableForWrite() == 0) {
      delayMicroseconds(10);
    }
    
    // PWMAudioは16bit signed (-32768 ~ +32767) を期待するため変換
    // 8bit unsigned (0~255) のデータの場合: (値 - 128) * 256
    //8bit unsignedの場合
    int16_t sample = (raw_audio[i] - 128) << 8;
    pwm.write(sample);
    

    //16bit 音源int16_tの場合
    //pwm.write((int16_t)raw_audio[i]);
  }
  
  //再生終了後
  delay(1000);
}
