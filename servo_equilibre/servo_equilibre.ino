  #include <SPI.h> // SPI
  #include <MFRC522.h> // RFID
  #include <Servo.h>

  #define SDA_PIN 11
  #define RST_PIN 5
  #define LOCK_PIN 1
  #define SIG_PIN 7
  // #define LED_PIN 2
  #define BUZZER_PIN 3
  #define MOTION_PIN 0


  #define DOOR_DISTANCE 30
  Servo LOCK_SERVO;
  

void setup() {
  // put your setup code here, to run once:
    LOCK_SERVO.attach(LOCK_PIN); 
    delay(2000);
    LOCK_SERVO.write(185);
    delay(2000);
    delay(2000);
    LOCK_SERVO.write(110);
    //LOCK_SERVO.write(70); 
}

void loop() {
  // put your main code here, to run repeatedly: 
}
