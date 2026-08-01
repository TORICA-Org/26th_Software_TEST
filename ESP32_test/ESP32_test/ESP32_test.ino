#include <SPI.h>
#include <SD.h>

// ピン配置の設定（CSは安全な5番に設定）
#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS   16

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(4, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(25, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  digitalWrite(25, LOW);

  
  Serial.println("\n--- SD Card Test Start ---");

  // 1. SPIの初期化
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // 2. SDカードの初期化
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("エラー: SDカードの初期化に失敗しました。");
    Serial.println("配線やCSピン、SDカードのフォーマット（FAT32）を確認してください。");
    return;
  }
  Serial.println("成功: SDカードを認識しました。");

  // 3. テストファイルの書き込み
  Serial.println("タスク: test.txt に書き込み中...");
  File file = SD.open("/test.txt", FILE_WRITE);
  if (file) {
    file.println("Hello ESP32 SD Test!");
    file.println("12345,67890,abcdefg");
    file.close();
    Serial.println("成功: 書き込みが完了しました。");
  } else {
    Serial.println("エラー: ファイルを開けませんでした（書き込み失敗）。");
  }

  // 4. テストファイルの読み込み
  Serial.println("タスク: test.txt を読み込み中...");
  file = SD.open("/test.txt", FILE_READ);
  if (file) {
    Serial.println("--- ファイルの中身 ---");
    while (file.available()) {
      Serial.write(file.read()); // シリアルモニタに出力
    }
    Serial.println("----------------------");
    file.close();
    Serial.println("成功: 読み込みが完了しました。");
  } else {
    Serial.println("エラー: ファイルを開けませんでした（読み込み失敗）。");
  }

  Serial.println("--- SD Card Test End ---");
}

void loop() {
  // テストなので1回実行したら何もしない
}