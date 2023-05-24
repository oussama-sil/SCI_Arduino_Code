// Djad's => Send one option and read badge ID
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
    Serial.print("Emptying the UART Buffer : ");
  char str[2];
  while(Serial1.available()>0){
    Serial1.readBytes(str,1);
    Serial.print(str[0]);
  }
}

void loop() {
  Serial.println("----------------------------------");

  // put your main code here, to run repeatedly:
  Serial.println("Sending events: ");
  Serial1.write("1");
//  Serial1.write("2");
//  Serial1.write("3");
//  Serial1.write("4");
  
  char str[2];
  char badge[4];
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


  // Serial1.println("1"); 
  delay(500);
}
