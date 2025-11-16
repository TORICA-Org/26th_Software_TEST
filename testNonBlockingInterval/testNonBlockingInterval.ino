int status = LOW;

void switchLedStatus() {
  if (status == LOW) {
    status = HIGH;
  } else {
    status = LOW;
  }
  digitalWrite(LED_BUILTIN, status);
}

class Timer {
  public:
    void setInterval(int _interval) {
      interval = _interval;
    }

    void run(void (*function)()) {
      // 引数([戻り値の型] *([ポインタ変数名])([引数情報]))
      if (millis() - last_timestamp >= interval) {
        last_timestamp = millis();
        function();
      }
    }

  private:
    int interval = 0;
    unsigned long last_timestamp = 0; // unsigned int -> unsigned longに修正
};

Timer Timer1;
Timer Timer2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Timer1.setInterval(200);
  Timer2.setInterval(2000);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.print("You sent: ");
    Serial.println(Serial.readStringUntil('\n'));
  }

  Timer1.run(switchLedStatus);
  Timer2.run([]() -> void {
    // ラムダ式：一時的な関数を書くときに便利（これ一回しか使わないんだよな...って時とか）
    unsigned long now = millis(); // unsigned int -> unsigned longに修正
    Serial.println(String("This message is sent every 2000ms!: ") + String(now));
  });
}
