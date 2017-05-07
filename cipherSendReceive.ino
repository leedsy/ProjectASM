#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#define Nchars 96  //  starting at 32 up to 126 in the ascii table 
#define n_shift 3  //  shift size
#define command_length 3 // command byte length

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Set serial pins for GSM
SoftwareSerial SIM900(7, 8);

// Destination phone number
//String destinationNumber = "+61413644703";
String destinationNumber = "+61400863029";

// Message
char plaintext[160]= {"this is a text"};

// Initialise ciphertext array
char ciphertext[sizeof(plaintext)];

// Whether the string is complete
boolean stringComplete = true;  


unsigned char buffer[64];  // buffer array for data receive over serial port
int count=0;               // counter for buffer array 

void setup()
{
  Serial1.begin(19200);
  Serial.begin(19200);
  // Start SIM card
  SIM900.begin(19200);

  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();

  
  // Give time to log on to network
  delay(5000);
  // Set SIM to text mode.
  SIM900.print("AT+CMGF=1\r"); 
  delay(1000);
  // Send text command.
  SIM900.println("AT + CMGS = \"" + destinationNumber +"\"");
  
  // Print phone number to screen
  lcd.print("Sending message to: "+ destinationNumber +"");
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

void loop()
{
  String message = "";
  String incomingNumber ="";
  String date = "";
  String incomingString = "";
  
  if (Serial1.available())
  {
    while(Serial1.available())
    {
      buffer[count]= Serial1.read();
      //output[count] = incomingByte;
  
      incomingString = Serial1.readString();
      incomingString.trim();
      incomingNumber = incomingString.substring(7,19);
      date = incomingString.substring(25,33);
      message = incomingString.substring(48);

      for (int i = 0; i < 160; i++)
      {
        ciphertext[i] = message[i];
      }

      lcd.print("Message from: " + incomingNumber + " " + date);
      delay(5000);
      lcd.clear();
      lcd.print(message);
      delay(10000);
      lcd.clear();
      lcd.print("Decrypting message..");
      delay(5000);
      lcd.clear();
      decrypt();
      lcd.print(plaintext);
      delay(5000);
      lcd.clear();
      count++;
      if(count == 64)break;
    }
    
    //message = buffer;
    //Serial.write(buffer,count);
    //lcd.write(buffer[1]);

    Serial.print(buffer[47]);
    //Serial.print(message);
    //lcd.clear();
    //lcd.print(message);
    clearBufferArray();
    count = 0;
  }
  if (Serial.available())
  {
    Serial1.write(Serial.read());
  }
}

void clearBufferArray()
{
  for (int i=0; i<count;i++)
  {
    buffer[i]=NULL;
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

