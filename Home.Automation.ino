#include <SoftwareSerial.h>
SoftwareSerial bt (0, 1);
SoftwareSerial  esp8266(3, 4);
int a, b;
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
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  bt.begin(9600);
  esp8266.begin(115200);
  sendAT("AT+RST\r\n", 2000); // Reset the ESP8266
  sendAT("AT+CWMODE=1\r\n", 1000); //Set station mode Operation
  sendAT("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"\r\n", 5000);
  // sendAT("AT+CIPSTA=\"192.168.1.254\",\"192.168.1.1\",\"255.255.255.0\"\r\n", 3000); // Assign Static IP Address
  sendAT("AT+CIFSR\r\n", 1000);//You will get the IP Address of the ESP8266 from thissendAT.

  sendAT("AT+CIPMUX=0\r\n", 5000);




}

void loop() {
  // put your main code here, to run repeatedly:
  bt.listen();
  if (bt.available()) {
    char bob = bt.read();
    Serial.println(bob);
    if (bob == 'f') {
      digitalWrite(10, LOW);
      a = 0;
    }
    else if (bob == 'n') {
      digitalWrite(10, HIGH);
      a = 1;
    }
    String A = String(a);
    updateTS(A);
  }
}

void updateTS(String A)

{
  esp8266.listen();
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", 1000);
  delay(2000);
  String cmdlen;
  String cmd = "GET https://api.thingspeak.com/update?api_key=86HSEMRZ7IVVU8CV&field1=" + A + "\r\n";
  cmdlen = cmd.length();     //Used by portal to accept a data packet of that length and close connection after that
  sendAT("AT+CIPSEND=" + cmdlen + "\r\n", 2000); // length should be less than 2024 bytes to avoid error data loss
  esp8266.print(cmd);
  Serial.println("");
  sendAT("AT+CIPCLOSE\r\n", 2000);
  Serial.println("");
  delay(1500);


}
