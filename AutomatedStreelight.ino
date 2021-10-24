#include <SoftwareSerial.h>
SoftwareSerial  esp8266(3, 4);
int Bulb_status;
int Light_status;
String ssid = "ChakraView";
String password = "peace and quiet";

String sendAT(String command, const int timeout)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.print(response);

  return response;
}

void setup() {
  pinMode(A0, INPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200);
  Serial.println("Automated Streetlight");
  sendAT("AT+RST\r\n", 2000); // Reset the ESP8266
  sendAT("AT+CWMODE=1\r\n", 1000); //Set station mode Operation
  sendAT("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"\r\n", 5000);
  // sendAT("AT+CIPSTA=\"192.168.1.254\",\"192.168.1.1\",\"255.255.255.0\"\r\n", 3000); // Assign Static IP Address
  sendAT("AT+CIFSR\r\n", 1000);//You will get the IP Address of the ESP8266 from thissendAT.

  sendAT("AT+CIPMUX=0\r\n", 5000);


}

void loop() {
  // put your main code here, to run repeatedly:
  int sun = analogRead(A0);
  Serial.println(sun);
  int data = map (sun, 0, 1023, 0, 100);
  if (sun <= 50) {
    digitalWrite(10, HIGH);
    Bulb_status = 100;
  }
  else if (sun >= 50) {
    digitalWrite(10, LOW);
    Bulb_status = 0;
  }
  String ldr = String(data);
  Serial.print("Sun light intensity");
  Serial.println(data);

  String Light_status = String(Bulb_status);
  Serial.print("Bulb status:");
  Serial.println(Bulb_status);
  updateTS(ldr,Light_status );
  delay(3000);
}

void updateTS(String T, String H)

  {
    Serial.println("");
    sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", 1000);
    delay(2000);
    String cmdlen;
    String cmd = "GET https://api.thingspeak.com/update?api_key=FEQZG6CEYEZ37V3Vd&field1=" + T + "&field2=" + H + "\r\n";
    cmdlen = cmd.length();     //Used by portal to accept a data packet of that length and close connection after that
    sendAT("AT+CIPSEND=" + cmdlen + "\r\n", 2000); // length should be less than 2024 bytes to avoid error data loss
    esp8266.print(cmd);
    Serial.println("");
    sendAT("AT+CIPCLOSE\r\n", 2000);
    Serial.println("");
    delay(1500);

  
}
