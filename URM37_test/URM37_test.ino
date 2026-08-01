/*
 * Raspberry Pi Pico (RP2040) + URM37 V5.0 (UART/TTL Passive Mode)
 * 距離データをシリアル通信で直接取得するスケッチ
 */

// 距離測定要求コマンド 
// 形式: 0x22 + サーボ角度 + ダミー + チェックサム
// ※サーボを繋がない、または正面を測る場合は角度0x00でOKです
uint8_t RequestDistCmd[4] = {0x22, 0x00, 0x00, 0x22}; 

uint8_t DistData[4];
unsigned int Distance = 0;

void setup()
{
  // PCのシリアルモニター表示用（USBシリアル）
  Serial.begin(115200);
  
  // URM37センサ接続用（Hardware UART / Serial1）
  Serial1.begin(9600);
  
  delay(1000); // センサの起動待ち
  Serial.println("URM37 UART Distance Mode Initialized.");
}

void loop()
{
  // 1. URM37（Serial1）へ距離データ要求コマンドを送信
  for (int i = 0; i < 4; i++) {
    Serial1.write(RequestDistCmd[i]);
  }
  
  // 2. センサの測定とデータ返信を少し待つ（目安50〜75ms）
  delay(60); 
  
  // 3. 応答データ（4バイト）が届いているか確認
  if (Serial1.available() >= 4)  
  {
    for (int i = 0; i < 4; i++) {
      DistData[i] = Serial1.read();
    }  
    
    // 応答の先頭バイトが 0x22 であることを確認（正しく距離データが返ってきたか）
    if (DistData[0] == 0x22) 
    {
      // 2バイト目（上位）と3バイト目（下位）を合体させて16ビットの数値にする
      Distance = (DistData[1] << 8) + DistData[2];
      
      // 測定エラー（範囲外など）のときは 0xFFFF (65535) が返ってきます
      if (Distance == 0xFFFF) 
      {
        Serial.println("Distance: Out of Range / Invalid");
      } 
      else 
      {
        // 取得できる値は最初から「cm単位」の数値です
        Serial.print("Distance: ");
        Serial.print(Distance);
        Serial.println(" cm");
      }
    }
  }
  
  // 次の測定まで少し間隔をあける（データシート推奨100ms以上）
  delay(100); 
}