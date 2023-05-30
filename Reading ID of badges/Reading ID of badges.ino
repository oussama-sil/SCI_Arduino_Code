 #include <SPI.h> // SPI
#include <MFRC522.h> // RFID
#define SDA_PIN 11
#define RST_PIN 5
#define LOCK_PIN 1
#define SIG_PIN 7
// #define LED_PIN 2
#define BUZZER_PIN 3
#define MOTION_PIN 0

MFRC522 rfid(SDA_PIN, RST_PIN); 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  // Init MFRC522 
  rfid.PCD_Init();

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Testing if there is a new  badgeID");
  // Vérifier la présence d'un nouveau badge 
  delay(1000);
  if ( !rfid.PICC_IsNewCardPresent())
    
      return;
  if ( !rfid.PICC_ReadCardSerial())
    return;

  Serial.println("Reading the badgeID from buffer");
  // Enregistrer l'ID du badge (4 octets) 
  for (byte i = 0; i < 4; i++) 
  {
    Serial.println(rfid.uid.uidByte[i]);
  }
}
