#include <Arduino.h>
#include <SD.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioGeneratorMP3.h" // WAVの場合はAudioGeneratorWAV.h

AudioFileSourceSD *file;
AudioOutputI2SNoDAC *out;
AudioGeneratorMP3 *mp3;

void setup() {
  Serial.begin(115200);

  if(!SD.begin(16)){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  out = new AudioOutputI2SNoDAC();
  out->SetPinout(26, 27, 13); // ダミーで26,25を使用し、音声出力を13に設定
  out->SetGain(3);        // 音量調整（最大4.0）

  // 3. 再生ファイルの指定とデコーダー開始
  file = new AudioFileSourceSD("/REWRITE.mp3"); // SDカード内のファイル名
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
}

void loop() {
  // 再生中であればループを回してデコードを継続
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      Serial.println("Playback Finished");
    }
  } else {
    delay(1000);
  }
}