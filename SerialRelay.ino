#include <SoftwareSerial.h>

SoftwareSerial SIM900(7, 8);

String outMessage = "Hello world!";
String destinationNumber = "+61400863029";

void setup()
{
  SIM900.begin(19200); 
  delay(20000); // give time to log on to network.
  SIM900.print("AT+CMGF=1\r");
  delay(1000);
  SIM900.println("AT + CMGS = \"" + destinationNumber +"\"");
  delay(1000);
  SIM900.print(outMessage);
  delay(1000);
  SIM900.write((char)26); //ctrl+z
  delay(1000);
}

void loop()
{
}
