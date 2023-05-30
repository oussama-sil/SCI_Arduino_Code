#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

char ssid[] = "Ico";
char pass[] = "oussouss";

// API IP address and PORT
char serverAddress[] = "192.168.87.193";  // server address
int port = 5000;

//Number of mails in the box
int mailbox_ID = 1;
int nb_mails =0;


WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);
    Serial1.begin(9600);
  
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  Serial.println("Emptying the UART Buffer : ");
  while(Serial1.available()>0){      
    Serial1.read();  
  }

  
}

void loop() {
  Serial.println("----------------------------------");
  Serial.println("making GET request");
  test_api();
  char str[2];
  if(Serial1.available()>0){
    Serial1.readBytes(str,1);
    Serial.print("Reading : ");
    Serial.println(str[0]);
    if(str[0]=='1'){
      publish_event(1);
      
    }
    if(str[0]=='2'){
      publish_event(2);
      
    }
    if(str[0]=='3'){
      publish_event(3);
    }
  }
  delay(5000);
  synchronize_badgeID();
    
  delay(5000);

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
    nb_mails +=1;
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
void synchronize_badgeID(){
  Serial.println("Retrieving the badgeID from the API");
  client.get("/api/arduino/"+String(mailbox_ID));
  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  if( statusCode==200){
    if(response != ""){
      Serial.println("-"+response.substring(0, 10)+"+"); // Badge
      Serial.println("+"+response.substring(11)+"+"); //Email
      Serial1.print(response.substring(0, 10));
    }else{ // case no owner , the second arduino must delete the badge (keep only the default one)
      Serial1.print("<00000000>"); 
    }
  }
}

