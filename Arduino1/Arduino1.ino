  #include <SPI.h> // SPI
  #include <MFRC522.h> // RFID
  #include <Servo.h>

  // Pins
  #define SDA_PIN 11
  #define RST_PIN 5
  #define LOCK_PIN 1
  #define SIG_PIN 7
  #define TRIG 0
  #define ECHO 2 
  #define BUZZER_PIN 3

  #define DOOR_DISTANCE 18
  #define MAIL_DISTANCE 10
  #define OP_ANG 105
  #define CL_ANG 180
  
  //* Verify if the badgeID is corercte 
  byte VerifyAllowed(byte *ValidCode,byte *TagCode) 
  {
    if ((ValidCode[0]==TagCode[0])&&(ValidCode[1]==TagCode[1])&&
    (ValidCode[2]==TagCode[2])&& (ValidCode[3]==TagCode[3]))
      return 1; 
    else
      return 0; 
  }

  //* Measuring the distance for the door's state
  int readUltrasonicDistance(int SIG_PIN_Value){
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

  // Measuring the distance for mail detection
  int measure_distance(){   
    digitalWrite(TRIG, LOW); 
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH); 
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duration = pulseIn(ECHO, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
    return duration*0.034/2;
  }

  // Déclaration 
  MFRC522 rfid(SDA_PIN, RST_PIN); 
  Servo LOCK_SERVO;
  byte Admin_TAG[4]={0x1B, 0xFF, 0xA9, 0xD};
  byte Allowd_TAG[4]={0x1B, 0xFF, 0xA9, 0xD}; 
  //byte Allowd_TAG[4]={0x81, 0x44, 0x4E, 0x63};
  byte nuidPICC[4];
  int DOOR_STATE=0; //0 if closed 1 if opened
  int Allowed_Open=0;
  int nb_mail=0;
  unsigned long TIME_LAST_EMAIL;
  int i;
    int mail_check_dist;
    //Closing doorc
    int door_distance=0;
  

  void setup() 
  { 
    // Init RS232
    Serial.begin(9600);
    Serial1.begin(9600);    
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(TRIG, OUTPUT); // Sets the trigPin as an Output
    pinMode(ECHO, INPUT); // Sets the echoPin as an Input
    // Init SPI bus
    SPI.begin(); 
    LOCK_SERVO.attach(LOCK_PIN);
    //int door_distance = readUltrasonicDistance(SIG_PIN);
    //if(door_distance>DOOR_DISTANCE ){
      LOCK_SERVO.write(OP_ANG);
      DOOR_STATE=1;

    // Init MFRC522 
    rfid.PCD_Init();          
  }

  void loop() 
  {
    Serial.println("---------------------------------------------------------------");  
    Serial.print("NB mails : ");
    Serial.println(nb_mail);
    if(!DOOR_STATE)
    {
      Serial.println("Door closed : ");
      // Mesurer la distance pour le ultrason qui détecte les nouveaux mail (car on peut reçus nouveaux juste si la la prote est fermé)
      mail_check_dist = measure_distance();
      Serial.print(mail_check_dist);
      Serial.print(" ");
      if(mail_check_dist<=MAIL_DISTANCE && millis()-TIME_LAST_EMAIL>10000){
          TIME_LAST_EMAIL = millis();
          nb_mail++;
        Serial.println(" ---> Sending email");
        Serial1.write(String(nb_mail).c_str()); //Sending a message to the second controller 
      }else{
        Serial.println(" ---> No mail");
      }
    }
    else
    {
      door_distance = readUltrasonicDistance(SIG_PIN);
      Serial.println(" Measured distance : "+ String(door_distance));
    }
    Serial.println("Door mechanism : Door current_state : "+ String(DOOR_STATE));
    Serial.print("Testing if the box is damaged : ");
    if(!DOOR_STATE && door_distance>DOOR_DISTANCE){
      Serial.println(" Yes, Sending email");
      Serial1.write("A");
      tone(BUZZER_PIN, 5000); // allume le buzzer actif arduino
      delay(1000);
      noTone(BUZZER_PIN);  // désactiver le buzzer actif arduino
    }else{
      Serial.println(" Safe");
    }
    //Testing if the Arduino 2 is sending information
    if(Serial1.available()>0){
      char str[2];
      char UID[8];
      Serial1.readBytes(str,1);
      Serial.println("Reading : "+String(str[0]));
      //Verifying that it's about to change RFID of allowed users
      if(str[0]=='<'){
        Serial1.readBytes(UID,8);
        Serial1.readBytes(str,1);
        Serial.println("Reading : <"+String(UID)+">");
        Serial.println("Reading : "+String(str[0]));
      }
      Serial.println("Modifying Mailbox allowed RFID...");
      //Modifying the Allowed_TAG
      for(i=0;i<4;i++){
        Allowd_TAG[i]=strtol(String(UID).substring(i*2, (i*2)+2).c_str(), NULL, 16);
      }
      Serial.println("The allowed RFID to open the Mailbox has been updated successfully. Now you can open it using the RFID :"+String(UID));
    }    
    //delay(300);
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
      
    if(VerifyAllowed(Allowd_TAG,nuidPICC) || VerifyAllowed(Admin_TAG,nuidPICC)) {
      
      if(!DOOR_STATE)
      {
        Serial.println("Valid BadgeID => Openning the door");
        LOCK_SERVO.write(OP_ANG);
        nb_mail = 0;
        Allowed_Open=1;
        DOOR_STATE=1;
        Serial1.write("0");
        delay(2000);      
      }
      else{
        if(DOOR_STATE && door_distance<=DOOR_DISTANCE ){
        LOCK_SERVO.write(CL_ANG);
          if(Allowed_Open){
            Allowed_Open=0;
          }
          DOOR_STATE=0;
          Serial.println("Valid BadgeID => Closing the door");
        }
        else{
          Serial.println("Valid BadgeID => Door already openned");
        }
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