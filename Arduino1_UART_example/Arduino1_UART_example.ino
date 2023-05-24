void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(2000);
  Serial.println("Emptying the UART Buffer : ");
  while(Serial1.available()>0){
    Serial1.read();
  }
}

void loop() {
  Serial.println("----------------------------------");
  Serial.println("Sending events: ");
  Serial1.write("2");
  char str[2];
  char badge[8];
  if(Serial1.available()>0){
    Serial1.readBytes(str,1);
    Serial.println("Reading : "+String(str[0]));
    if(str[0]=='<'){
      Serial1.readBytes(badge,8);
      Serial1.readBytes(str,1);
      Serial.println("Reading : <"+String(badge)+">");
      Serial.println("Reading : "+String(str[0]));
      
      byte firstChar = strtol(String(badge).substring(0, 2).c_str(), NULL, 16);
      byte secondChar = strtol(String(badge).substring(2, 4).c_str(), NULL, 16);
      byte thirdChar = strtol(String(badge).substring(4, 6).c_str(), NULL, 16);
      byte fourthChar = strtol(String(badge).substring(6, 8).c_str(), NULL, 16);
      
      if(str[0]=='>'){
        if(firstChar==0x15 && secondChar==0xBD  && thirdChar==0x85 && fourthChar==0x6A){
          Serial.println("Matching BadgeID");
        }
      }
    }
  }
  delay(500);
}
