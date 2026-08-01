// 受信テスト

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Serial1.setTX(D6);
  // Serial1.setRX(D7);
  Serial1.begin(9600, SERIAL_8N1, 44, 43);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial1.available() > 0) {
    char c = Serial1.read();
    Serial.print(c);
  }
}