// Oussama's => Receive the Notify input , synchronize with the API , send badgeID
void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.print("Emptying the UART Buffer : ");
  char str[2];
  while(Serial1.available()>0){      
    Serial1.readBytes(str,1);
    Serial.print(str[0]);
  }
}

void loop() {
  Serial.println("----------------------------------");
  // Reading from the second arduino
  char str[2];
  if(Serial1.available()>0){
    Serial1.readBytes(str,1);
    Serial.print("Reading : ");
    Serial.print(str[0]);
    Serial.print("-");  
    Serial.println(str[0]=='1');
  }

  // char badge[7]="<15BD>";
  String badge_ = "<15BD856A>";
  Serial1.print(badge_);

  // Sending tp the second arduino


  delay(500);
}
