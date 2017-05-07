#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define Nchars 96  //  starting at 32 up to 126 in the ascii table 
#define n_shift 3  //  shift size
#define command_length 3 // command byte length

// Set LCD pins
LiquidCrystal_I2C lcd(0x27, 20, 4, 1);

// Set serial pins for GSM
SoftwareSerial SIM900(7, 8);

// Destination phone number
String destinationNumber = "";

String header = "+61";

// Message
char plaintext[160];

// Initialise ciphertext array
char ciphertext[sizeof(plaintext)];

// Whether the string is complete
boolean stringComplete = true;  

unsigned char buffer[64];  // buffer array for data receive over serial port
int count=0;               // counter for buffer array 

void setup() {
  
  // Initialise the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  
  Serial.begin(19200);

  // Start SIM card
  SIM900.begin(19200);
  lcd.print("GSM Connected");
  delay(3000);
  lcd.clear();
  displayMenu();
}

void loop() {
 //receiveMessage();
 String menuOption = "";
 bool messaegReceived = false;
 
 // send data only when you receive data:
 while (Serial.available()) 
 {
    menuOption = Serial.readString();
    // Remove carrige return character
    menuOption.trim();
    
    if (menuOption == "1")
    {
      Serial.println("Send Message");
      sendMessage();
    }
    else if (menuOption == "2")
    {
      Serial.println("Receive Message");
      receiveMessage();
    }
    else
    {
      Serial.println("Unkown Command");
    }

    // Clear option
    menuOption = "";
    displayMenu();
 }

 //receiveMessage();
 
}

void displayMenu()
{
  Serial.println("ASM Menu");
  Serial.println("Enter the number of your option.");
  Serial.println("--------------------");
  Serial.println("1. Send Message");
  Serial.println("2. Receive Message");
  //Serial.println("3. Clear SIM");
  Serial.println("--------------------");
}

bool getPhoneNumber(String originalNumber)
{
  bool result = false;
  char charNumber[9];
  String convertedNumber ="";

  if(originalNumber == "" || originalNumber == NULL)
  {
    result = false;
  }
  else
  {
    //Remove the leading 0 in number
    for (int i = 1; i < originalNumber.length(); i++)
    {
      charNumber[i-1] = originalNumber[i];
    }
    
    convertedNumber = String(charNumber);
    destinationNumber = header + convertedNumber;

    result = true;
  }
  
  return result;
}

void getInformation()
{
  String message = "";
  String phoneNumber = "";
  bool result = false;
  String exitResponse = "";
  bool exitSending = false;
  // read the incoming byte:

  Serial.println("To exit press x");
    exitResponse = Serial.readString();
    exitResponse.trim();
    if (exitResponse == "x" || exitResponse == "X")
    {
        exitSending = true;
        Serial.println("Exit");
        //break;
    }
  
  Serial.println("Enter phone number:");
    lcd.print("Enter phone number: ");
  while(phoneNumber == "")
    {
    phoneNumber = Serial.readString();
    phoneNumber.trim();
    if (phoneNumber == "x" || phoneNumber == "X")
    {
        exitSending = true;
        Serial.println("Exit");
        break;
    }
    lcd.print(phoneNumber);
    delay(3000);
    result = getPhoneNumber(phoneNumber);
    //phoneNumber = "";
    }
    lcd.clear();
    Serial.println("Enter message:");
    lcd.print("Enter message:      ");
    if(result == true)
    {
      while(message == "")
      {
       
       message = Serial.readString();
       message.trim();
       message.toCharArray(plaintext, 160);
       result = false;
       //message = "";
      }
    } 
    lcd.print(plaintext);
}

void sendMessage()
{ 
  //Serial.begin(19200);
  getInformation();
  lcd.clear();
  // Set SIM to text mode.
  SIM900.print("AT+CMGF=1\r"); 
  delay(1000);
  // Send text command.
  SIM900.println("AT + CMGS = \"" + destinationNumber +"\"");
  
  // Print phone number to screen
  lcd.print("Sending message to: "+ destinationNumber);
  delay(3000);
  lcd.clear(); 

  lcd.print("Encrypting message..");
  delay(5000);
  lcd.clear();
  
  while(Serial)
  {
    if(stringComplete==true){
      encrypt();
    }
    else{
      break;
    }
  }
}

void clearFields()
{
 for( int i = 0; i < sizeof(plaintext);  ++i )
  {
   plaintext[i] = (char)0;
  }
   
  destinationNumber="";
}

void receiveMessage()
{
  Serial1.begin(19200);
  String message = "";
  String incomingNumber ="";
  String date = "";
  String incomingString = "";
  bool messageReceived = false;
  String response = "";
  String exitResponse = "";

  Serial.println("Waiting for message...");
  Serial.println("To exit press x");
  while(messageReceived == false)
  {
      
  if (Serial1.available())
  {
    while(Serial1.available())
    {
      buffer[count]= Serial1.read();
      
      incomingString = Serial1.readString();
      incomingString.trim();
      incomingNumber = incomingString.substring(7,19);
      date = incomingString.substring(25,33);
      message = incomingString.substring(48);

      for (int i = 0; i < 160; i++)
      {
        ciphertext[i] = message[i];
      }

      Serial.println("Message received");
      lcd.print("Message received from: " + incomingNumber + " " + date);
      delay(10000);
      lcd.clear();
      Serial.println("Do you want to view this message? y/n");
      while (response == "")
      {
        response = Serial.readString();
      }

      response.trim();

      if(response == "y" || response == "Y")
      {
        lcd.print(message);
        delay(10000);
        lcd.clear();
        lcd.print("Decrypting message..");
        delay(5000);
        lcd.clear();
        decrypt();
        Serial.println(plaintext);
        delay(5000);
        count++;
        if(count == 64)break;
      }
      else if (response == "n" || response == "N")
      {
        messageReceived = true;
        Serial.println("Exit");
        break;
      }
      else
      {
        Serial.println("Please answer with y or n.");
        response = "";
      } 

      //Serial.flush();
    }

    //Serial.print(buffer[47]);
    clearBufferArray();
    count = 0;
    response = "";
  }
  
  if (Serial.available())
  {
      exitResponse = Serial.readString();
      exitResponse.trim();
      if (exitResponse == "x" || exitResponse == "X")
      {
        messageReceived = true;
        Serial.println("Exit");
        break;
      }
  }

  }
  
  //SIM900.print("AT+CMGD=2\r");
  Serial1.end(); 
}

void clearBufferArray()
{
  for (int i=0; i<count;i++)
  {
    buffer[i]= NULL;
  }
}

void encrypt()
{
  uint8_t sizeofarray = sizeof(plaintext);
  uint8_t i;
  
  for(i = 0; i < sizeofarray; i++)
  {
    uint16_t retrieved;
    retrieved = plaintext[i];
    if(retrieved == 0){
      ciphertext[i] = 0;
      goto bailout; 
    }
    
    //retrieved -=32; // Subract Ascii_offset
    uint16_t c = ((retrieved-32) + n_shift)% Nchars; // check blog post for more info on this  
    c +=32; // add the Ascii_offset
    ciphertext[i] = c;
    delay(50);
  }
bailout://  :)
  displayResults();
  stringComplete = false;
}

void displayResults(){
  // Send encrypted message
  SIM900.print(ciphertext);
  delay(1000);
  // End the text [ctrl+z]
  SIM900.write((char)26);
  lcd.clear();
  lcd.print("Sending message...");
  delay(5000);
  lcd.clear();
  lcd.print("Encrypted message:  ");
  lcd.print(ciphertext); 
  delay(5000); 
  lcd.clear();
}

void decrypt()
{
  uint8_t sizeofarray = sizeof(ciphertext);
  uint8_t i;
  
  for(i = 0; i < sizeofarray; i++)
  {
    uint16_t retrieved;
    retrieved = ciphertext[i];
    if(retrieved == 0){
      plaintext[i] = 0;
      goto bailout; 
    }
    
    //retrieved -=32; // Subract Ascii_offset
    uint16_t p = ((retrieved-32) - n_shift)% Nchars; // check blog post for more info on this  
    p +=32; // add the Ascii_offset
    plaintext[i] = p;
    delay(50);
  }
bailout://  :)
  stringComplete = false;
}


