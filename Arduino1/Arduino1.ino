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

  //* Verify if the badgeID is corercte 
  byte VerifyAllowed(byte *ValidCode,byte *TagCode) 
  {
    if ((ValidCode[0]==TagCode[0])&&(ValidCode[1]==TagCode[1])&&
    (ValidCode[2]==TagCode[2])&& (ValidCode[3]==TagCode[3]))
      return 1; 
    else
        return 0; 
  }

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


  // Déclaration 
  MFRC522 rfid(SDA_PIN, RST_PIN); 
  Servo LOCK_SERVO;
  byte Allowd_TAG[4]={0x1B, 0xFF, 0xA9, 0xD}; 
  byte nuidPICC[4];
  int DOOR_STATE=0; //0 if closed 1 if opened
  int MOTION_STATE=0;
  int Allowed_Open=0;
  unsigned long PIR_HOLD;
  unsigned long CURRENT_TIME;
  const long HOLD_TIME=10000; //HOLD of motion sensor


  void setup() 
  { 
    // Init RS232
    Serial.begin(9600);
    Serial1.begin(9600);    // pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(MOTION_PIN, INPUT);
    // Init SPI bus
    SPI.begin(); 
    LOCK_SERVO.attach(LOCK_PIN);
    // Init MFRC522 
    rfid.PCD_Init();
          
    delay(50); 
  }

  void loop() 
  {

    Serial.println("---------------------------------------------------------------");  


    CURRENT_TIME=millis();
    //Closing doorc
    int door_distance =   readUltrasonicDistance(SIG_PIN);
    Serial.println(" Measured distance : "+ String(door_distance));
    Serial.print("Door mechanism : Door current_state : "+ String(DOOR_STATE)+"----->");
    // if(DOOR_STATE && door_distance<=DOOR_DISTANCE){
    //   LOCK_SERVO.write(0);
    //   if(Allowed_Open){
    //     Allowed_Open=0;
    //   }
    //   digitalWrite(LED_PIN,LOW);
    //   DOOR_STATE=0;
    //   Serial.println("Door closed");
    // }else{
    //   Serial.println("Door opened");
    // }
    Serial.print("Testing if the box is damaged : ");
    if(!DOOR_STATE && door_distance>DOOR_DISTANCE){
      Serial.println(" Yes, Sending email");
      Serial1.write("3");

      tone(BUZZER_PIN, 5000); // allume le buzzer actif arduino
      delay(5000);
      noTone(BUZZER_PIN);  // désactiver le buzzer actif arduino
    }else{
      Serial.println(" Safe");
    }

    delay(300);
    Serial.println("Testing if there is a badge");
    // Initialisé la boucle si aucun badge n'est présent 
    if ( !rfid.PICC_IsNewCardPresent())
      return;
    
    Serial.println("Testing if there is a new  badgeID");
    // Vérifier la présence d'un nouveau badge 
    if ( !rfid.PICC_ReadCardSerial())
      return;

    Serial.println("Reading the badgeID from buffer");
    // Enregistrer l'ID du badge (4 octets) 
    for (byte i = 0; i < 4; i++) 
    {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
      
    if(VerifyAllowed(Allowd_TAG,nuidPICC)) {
      
      if(!DOOR_STATE)
      {
        Serial.println("Valid BadgeID => Openning the door");
        LOCK_SERVO.write(360);
        Allowed_Open=1;
        DOOR_STATE=1;
        Serial1.write("2");
        delay(1000);      
      }
      else{
        if(DOOR_STATE && door_distance<=DOOR_DISTANCE ){
        LOCK_SERVO.write(0);
          if(Allowed_Open){
            Allowed_Open=0;
          }
          // digitalWrite(LED_PIN,LOW);
          DOOR_STATE=0;
          Serial.println("Valid BadgeID => Closing the door");
        }
        else{
          Serial.println("Valid BadgeID => Door already openned");
        }
        // Serial.println("Prote déja ouverte");
      }
    }
    else{
      Serial.println("Invalid BadgeID");
      if(!DOOR_STATE){
        // Serial.println("Code érroné");
        tone(BUZZER_PIN, 5000); // allume le buzzer actif arduino
        delay(1000);
        noTone(BUZZER_PIN);  // désactiver le buzzer actif arduino
      } 
    }
    // Re-Init RFID
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
  }

