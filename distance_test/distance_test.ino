  #include <SPI.h> // SPI
  #include <MFRC522.h> // RFID
  #include <Servo.h>

  #define SDA_PIN 11
  #define RST_PIN 5
  #define LOCK_PIN 1
  #define OP_ANG 105
  #define CL_ANG 180
  #define SIG_PIN 7
  // #define LED_PIN 2
  #define BUZZER_PIN 3
  #define MOTION_PIN 0
  #define DOOR_DISTANCE 30
  //* Measuring the distance 
  int readUltrasonicDistance(int SIG_PIN_Value)
  {
    int dist_cm;
    pinMode(SIG_PIN_Value, OUTPUT);  // Clear the trigger
    digitalWrite(SIG_PIN_Value, LOW);
    delayMicroseconds(2);
    // Sets the trigger pin to HIGH state for 10 microseconds
    digitalWrite(SIG_PIN_Value, HIGH);
    delayMicroseconds(10);
    digitalWrite(SIG_PIN_Value, LOW);
    pinMode(SIG_PIN_Value, INPUT);
    // Reads the echo pin, and returns the sound wave travel time in microseconds
    dist_cm = 0.01723 * pulseIn(SIG_PIN_Value, HIGH);
    return dist_cm;
  }
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(readUltrasonicDistance(SIG_PIN));
  delay(1000);
}
