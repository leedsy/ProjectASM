#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <AESLib.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Set serial for GSM
SoftwareSerial SIM900(7, 8);

//String destinationNumber = "+61413644703";
String destinationNumber = "+61400863029";
String text = "";

void setup()
{
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  
  Serial.begin(19200);
  uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int limit = 16;
  char data[16] = "Hello, world!"; //16 chars == 16 bytes
  
  
  SIM900.begin(19200);
  delay(10000); // give time to log on to network.
  //SIM900.print("AT+CSCS=\"UCS2\"\r");
  //SIM900.print("AT+CSMP=17,167,0,8\r");
  SIM900.print("AT+CMGF=1\r");
  delay(1000);
  SIM900.println("AT + CMGS = \"" + destinationNumber +"\"");
  lcd.print("Sending message to\""+ destinationNumber +"\"");
  delay(3000);
  lcd.clear(); 
  
  aes128_enc_single(key, data);
  lcd.print("Encrypted:");
  delay(5000);
  lcd.clear();

  for(int i = 0; i < limit; i++)
  {
    Serial.println(data[i]);
    text.concat(data[i]);
  }

  lcd.print(text);
  delay(5000);

  Serial.print(text);

  //char textChar[16];
  //text.toCharArray(textChar, 16);
  //Serial.print(textChar);
  
  aes128_dec_single(key, data);
  lcd.clear();
  lcd.print("Decrypted:");
  delay(5000);
  lcd.clear();
  lcd.print(data);
  delay(5000);
  lcd.clear();

  SIM900.print(text);
  delay(1000);
  SIM900.write((char)26); //ctrl+z
  delay(5000);
}

void loop()
{
}
