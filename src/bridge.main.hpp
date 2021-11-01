#include <Arduino.h>

auto& nexSerial = Serial3;

auto setup() -> void {
  Serial.begin(115200);

  nexSerial.begin(9600);

  nexSerial.print("baud=115200\xFF\xFF\xFF");
  delay(20);
  nexSerial.flush();
  nexSerial.begin(115200);
  delay(20);

  //nexSerial.print("page 3\xFF\xFF\xFF");
}

auto loop() -> void {
  while (Serial.available()) {
    nexSerial.write(Serial.read());
  }
  while (nexSerial.available()) {
    Serial.write(nexSerial.read());
  }
}
