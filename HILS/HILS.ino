#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// UART用
const int SERIAL_TX = 0;
const int SERIAL_RX = 1;

// SD用SPI
const int SD_MISO = 4;
const int SD_MOSI = 7;
const int SD_CS = 5;
const int SD_SCK = 6;

const String DATA_FILE = "data.csv"; // 指定のファイル名
int ROW_SIZE = 0;          // 行カウント用

// まとめて読み出すための固定バッファ (2KB)
const int BUFFER_SIZE = 16392;
char sd_buffer[BUFFER_SIZE];
int buffer_length = 0;  // 現在バッファに入っている有効なデータ数
int buffer_index = 0;   // 次に読み出すバッファ内の位置

// ★【修正】ヘッダー行（928文字）がすっぽり入るように 1024 に拡大！
const int MAX_LINE_SIZE = 16392;
char line_data[MAX_LINE_SIZE];

File myfile;

void setup() {
  Serial.begin(115200);

  Serial1.setTX(SERIAL_TX);
  Serial1.setRX(SERIAL_RX);
  Serial1.begin(460800, SERIAL_8E1); // 各基板との通信速度を設定
  delay(2000);

  Serial.println("Initializing SPI...");
  delay(1000);

  SPI.setRX(SD_MISO);
  SPI.setTX(SD_MOSI);
  SPI.setSCK(SD_SCK);

  if (SD.begin(SD_CS) == false){
    Serial.println("SD initialization failed.");
  } else {
    Serial.println("SD initialization done.");
  }

  if(SD.exists(DATA_FILE)) {
    Serial.println(DATA_FILE + " exists.");
  } else {
    Serial.println("Cannot find " + DATA_FILE);
  }
}

// SDカードからバッファ（バケツ）へデータを一気に補充する関数
void refillBuffer() {
  int leftover = buffer_length - buffer_index;
  if (leftover > 0 && buffer_index > 0) {
    memmove(sd_buffer, &sd_buffer[buffer_index], leftover);
  }
  buffer_length = leftover;
  buffer_index = 0;

  int space_available = BUFFER_SIZE - buffer_length;
  if (space_available > 0 && myfile.available() > 0) {
    int bytes_read = myfile.read((uint8_t*)&sd_buffer[buffer_length], space_available);
    buffer_length += bytes_read;
  }
}

// バッファから1文字ずつ安全に取り出す関数
char getNextChar() {
  if (buffer_index >= buffer_length) {
    refillBuffer();
    if (buffer_index >= buffer_length) {
      return 0; 
    }
  }
  return sd_buffer[buffer_index++];
}

// バッファから1行を切り出す内部処理関数
bool readNextLine(void) {
  int line_idx = 0;
  bool has_data = false;

  while (true) {
    char c = getNextChar();
    if (c == 0) break;
    
    has_data = true; 
    if (c == '\n') break; 
    if (c != '\r') { 
      if (line_idx < MAX_LINE_SIZE - 1) {
        line_data[line_idx++] = c;
      }
    }
  }
  line_data[line_idx] = '\0';
  return has_data;
}

// バッファから1行を切り出して、そのまま一発でUART送信する関数
void transmitNextLineFromBuffer(void) {
  if (!myfile) {
    myfile = SD.open(DATA_FILE, FILE_READ);
    if (!myfile) {
      Serial.println("Cannot open " + DATA_FILE);
      return;
    }
    Serial.println("Buffered reading start...");
    buffer_length = 0;
    buffer_index = 0;
    ROW_SIZE = 0;

    // ★【ここを追加】ファイルを開いた直後に、1行目（ヘッダー行）を空読みしてスキップ！
    if (myfile.available() > 0) {
      readNextLine(); 
      Serial.println("Header line skipped.");
    }
  }

  // データ行を読み込み
  if (readNextLine()) {
    // もし末尾がカンマなら消去（今回のCSVにはありませんが、安全のため残しています）
    int len = strlen(line_data);
    if (len > 0 && line_data[len - 1] == ',') {
      line_data[len - 1] = '\0';
    }

    // ESP32へ送信
    Serial1.print(line_data); 
    Serial1.print("\n"); 
    
    // PCモニターへ表示
    Serial.print(line_data);
    Serial.print("\n");

    ROW_SIZE++;
  } else {
    myfile.close(); 
    Serial.print("finish reading. Total rows: ");
    Serial.println(ROW_SIZE);
  }
}

uint32_t last_time = 0;
void timer_100Hz(void){
  if (millis() - last_time >= 10){
    last_time = millis(); 
    transmitNextLineFromBuffer();
  }
}

void loop() {
  timer_100Hz();
}