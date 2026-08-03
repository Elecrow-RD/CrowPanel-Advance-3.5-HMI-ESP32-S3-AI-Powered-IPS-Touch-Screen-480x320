/*---------------------------------------------------------------
 * UART bridge pins
 * Serial1 is used to move text between the USB monitor and the Zigbee
 * module interface on the board.
 *--------------------------------------------------------------*/
#define SERIAL1_RX 2
#define SERIAL1_TX 1

/**
 * @brief Initialize both serial ports used by the lesson.
 *
 * @param None.
 * @return None.
 */
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);
  Serial.println("ESP32-S3 Serial Communication Example");
}

/**
 * @brief Forward any received Serial1 text to the USB monitor.
 *
 * @param None.
 * @return None.
 */
void loop() {
  if (Serial1.available()) {
    String message = Serial1.readStringUntil('\n');
    Serial.print("The message from serial port was received.: ");
    Serial.println(message);
  }
}
