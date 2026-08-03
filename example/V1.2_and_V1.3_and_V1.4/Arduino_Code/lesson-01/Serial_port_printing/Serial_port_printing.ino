/*---------------------------------------------------------------
 * Serial counter demo
 * Print a stable message and an increasing number once per second.
 *--------------------------------------------------------------*/

// Stores how many times the loop has printed a message.
int counter = 0;

/**
 * @brief Configure the serial port used by the lesson.
 *
 * The serial monitor must use the same baud rate, otherwise the
 * received characters may appear garbled.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(9600);
}

/**
 * @brief Print the demo message and update the counter.
 *
 * The one-second delay makes each serial line easy to observe in
 * the monitor and gives students a clear timing reference.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop() {
  Serial.print("Hello, World! ");
  Serial.print("--- ");
  Serial.println(counter);

  counter++;
  delay(1000);
}
