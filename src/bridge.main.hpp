#include <Arduino.h>

auto setup() -> void {
  Serial.begin(115200);

  Serial1.begin(9600);
  Serial1.print("baud=115200\xFF\xFF\xFF");
  delay(20);
  Serial1.flush();

  Serial1.begin(115200);
  delay(20);
}

auto loop() -> void {
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}
