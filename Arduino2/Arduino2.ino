#include <EMailSender.h>
#include <ArduinoHttpClient.h>

//* LEDs
#define GREEN 0  // WIFI CONNECTED 
#define RED 1    // WIFI NOT CONNECTED
#define INFO 2   // SENDING EMAIL
//* Pins of the Ultrasonic sensor 
#define TRIG 3
#define ECHO 4 
#define MAX_DISTANCE 20
#define LED_WIFI 6
#define MAIL_DISTANCE 10


//
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

//* Access point credentials 
char ssid[] = "Ico";
char password[] = "oussouss";
unsigned long CONNECTING_LAST = 0;
  
//* EmailSender informations 
EMailSender emailSend("oussama4123silem11@gmail.com", "seniphfgkzibmpje","Intelligent Mailbox");
String receiver = "oussama4123silem11@gmail.com";
EMailSender::Response resp;
EMailSender::EMailMessage mail_reception_message;//1
EMailSender::EMailMessage door_openning_message;//2
EMailSender::EMailMessage damaged_mailbox_message;//3

//* API Informations
char serverAddress[] = "192.168.217.193";  // API IP address
int port = 5000; // PORT 
unsigned long SYN_LAST = 0;

//* Informations about the mailbox
int mailbox_ID = 1; // Unique ID to identify the mailbox on the API
int nb_mails = 0; // Number of mails in the box

//* HTTP Objects 
WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

  //* Try connecting to the access point 
  uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr){
      digitalWrite(LED_WIFI, LOW); 
      static uint16_t attempt = 0;
      Serial.print("Connecting to ");
      if(nSSID) {
          Serial.println(nSSID);
          WiFi.begin(nSSID, nPassword);
      }

      uint8_t i = 0;
      while(WiFi.status()!= WL_CONNECTED && i++ < 25)
      {
          digitalWrite(LED_WIFI, LOW); 
          delay(200);
          digitalWrite(LED_WIFI, HIGH); 
          delay(200);
          Serial.print(".");
          
      }
      ++attempt;
      Serial.println("");
          digitalWrite(LED_WIFI, LOW); 
      if(i == 51) {
          Serial.print("Connection: TIMEOUT on attempt: ");
          Serial.println(attempt);
          if(attempt % 2 == 0)
              Serial.println("Check if access point available or SSID and Password\r\n");
          return false;
      }
          digitalWrite(LED_WIFI, HIGH); 
      Serial.println("Connection: ESTABLISHED");
      Serial.print("Got IP address: ");
      Serial.println(WiFi.localIP());
      return true;
  }
  
  //* Waiting while attempting to connect to the wifi 
  void Awaits(){
      uint32_t ts = millis();
      int nb_attempts = 0;
      int NB_ATTEMPTS    =0;
      while(!connection_state && nb_attempts < NB_ATTEMPTS )
      {
          nb_attempts++;  
          delay(50);
          if(millis() > (ts + reconnect_interval) && !connection_state){
              connection_state = WiFiConnect();
              ts = millis();
          }
      }
  }

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);


    
    pinMode(TRIG, OUTPUT); // Sets the trigPin as an Output
    pinMode(ECHO, INPUT); // Sets the echoPin as an Input
    pinMode(LED_WIFI, OUTPUT); // LED PIN

    // Initialisation of the messages to send to the Owner through the email
    mail_reception_message.subject = "Intelligent Mailbox";
    mail_reception_message.message= "You received a mail";
    door_openning_message.subject = "Intelligent Mailbox";
    door_openning_message.message= "Your mailbox was opened";
    damaged_mailbox_message.subject = "Intelligent Mailbox";
    damaged_mailbox_message.message= "Your mailbox was damaged";

    //*Connecting to the wifi
    digitalWrite(LED_WIFI, LOW); 
    connection_state = WiFiConnect(ssid, password);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    //* Emptieng the Serial1 buffer   
    Serial.println("Emptying the UART Buffer : ");
    while(Serial1.available()>0){
      Serial1.read();
    }
}

void loop()
{
    Serial.println("---------------------------------------------------------------");  
    
    if(WiFi.status()== WL_CONNECTED){
        // If connected 
        digitalWrite(LED_WIFI, HIGH); 
        print_WiFi_Status();
    }else{
        // If not connected , every 20(s) try to connect again
          if(millis()-CONNECTING_LAST>20000){
            Serial.println("Attempting to connect to Wifi : "+String(CONNECTING_LAST)+"   "+String(millis()));          
            digitalWrite(LED_WIFI, HIGH);
            WiFi.begin(ssid,password);
            CONNECTING_LAST = millis();
          }
          digitalWrite(LED_WIFI, LOW);     
    }

    Serial.print("Reading mail distance : ");
    int mail_distance = measure_distance();
    Serial.println(String(mail_distance)+"  ");
    if(mail_distance<MAIL_DISTANCE){
        Serial.println("New Mail => Sending email");
        nb_mails += 1;
        if(WiFi.status()== WL_CONNECTED){
          notify_user(1); // Sending a notification to the owner 
          publish_event(1); // Recording the event on the database
        }
    }

    
    if ( Serial1.available() > 0) { 
        char str[2];
        Serial1.readBytes(str,1);
        Serial.print("UART : ");
        Serial.print(str[0]);
        Serial.print("-"); 
        Serial.println(str[0]=='1');
        if(WiFi.status()== WL_CONNECTED){
          if(str[0]=='2'){
            notify_user(2);
            publish_event(2); // Recording the event on the database
          }
          if(str[0]=='3'){
            notify_user(3);
            publish_event(3); // Recording the event on the database

          }
        }            
    }

  //* Synchronizing the Arduino with the database   
  if(millis()-SYN_LAST>60000 && WiFi.status()== WL_CONNECTED){
    SYN_LAST = millis();
    synchronize_device();    
  } 
}

int measure_distance(){   
  digitalWrite(TRIG, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); 
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  return duration*0.034/2;
}



  void notify_user(int opt){
      
      // digitalWrite(INFO, HIGH);
    // return ;
    //  String message_  =" Your mail box was opened"; 
      
    if(opt==1)  
      resp = emailSend.send(receiver, mail_reception_message);
    if(opt==2)
      resp = emailSend.send(receiver, door_openning_message);
    if(opt==3)
      resp = emailSend.send(receiver, damaged_mailbox_message);


      Serial.println("Sending status: ");

      Serial.println(resp.status);
      Serial.println(resp.code);
      Serial.println(resp.desc);

      // delay(
      //   2000
      // );

  }

void print_WiFi_Status() {
  // print the SSID of the connected network
  Serial.print("SSID: ");
  Serial.print(WiFi.SSID());

  // print the MAC address of the WiFi module
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print(" MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }

  // print the IP address of the WiFi module
  IPAddress ip = WiFi.localIP();
  Serial.print("   IP Address: ");
  Serial.print(ip);

  // print the signal strength of the connected network
  long rssi = WiFi.RSSI();
  Serial.print("   Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void send_email(String receiver,String subject,String message_) {
    EMailSender::EMailMessage message;
    message.subject = subject;
    message.message = message_;

    EMailSender::Response resp = emailSend.send("oussama4123silem11@gmail.com", message);
    Serial.println("Sending status: ");

    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);


}


void test_api(){
  Serial.println("Making GET request to test the api");
  client.get("/");
  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  if(response=="ok" && statusCode==200){
    Serial.println("API is Working");
  }
}
void publish_event(int event_type){
  Serial.println("Sending event "+String(event_type)+" to the API");
  // New mail received 
  if(event_type==1){
    client.get("/api/event/"+String(mailbox_ID)+"/New%20Mail%20Received/"+String(nb_mails));
  }
  // Mailbox opened 
  if(event_type==2){
    nb_mails = 0;
    client.get("/api/event/"+String(mailbox_ID)+"/Mailbox%20Emptied/"+String(nb_mails));

  }
  // Mailbox damaged
  if(event_type==3){
    nb_mails = 0;
    client.get("/api/event/"+String(mailbox_ID)+"/Mailbox%20Damaged/"+String(nb_mails));
  }
  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  if(response=="ok" && statusCode==200){
    Serial.println("API is Working");
  }
}
void synchronize_device(){
  Serial.println("Retrieving the badgeID from the API");
  client.get("/api/arduino/"+String(mailbox_ID));
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  if( statusCode==200){
    if(response != ""){
      Serial1.print(response);
    }
  }
}




