/*Created by Lauri 
 +V, +5v (or 9-12v depending on the model)
 SCL clock – connect to pin A5 on the Uno
 SDA data – connect to pin A4 on the Uno
 Gnd
 Potentiometer goes to analog pin 0
 Button goes digital pin 2
 Digital pin 3 is for output HIGH or LOW depending thermostat settings
 Last edited: 06.02.2015 Status: Working fully
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int analogPin     = 0;  /*potentiometer for set low or high hysteresis value*/
const int button1Pin    = 2;  /*button for selectiong set low hysteresis value*/
const int button2Pin    = 3;  /*button for selectiong set high hysteresis value*/
const int digitalIn     = 11;  /*Output pid for controlling */
const int ledPin        = 13; // 1k resistor

int button1PushCounter = 0;   // counter for the number of button presses
int button1State = 0;         // current state of the button
int lastButton1State = 0;     // previous state of the button

int button2PushCounter = 0;   // counter for the number of button presses
int button2State = 0;         // current state of the button
int lastButton2State = 0;     // previous state of the button


//int but_mode           = 0;  /*changes button set mode:1=set low hyst,2=set high hyst, 3rd press=normal */
int low_hys            = 22; // Defaults    
int high_hys           = 25; // Defaults
int temper             = 0;

int reading1;
int reading2;
int val;
int relStat;
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.

void setup()
{
  Serial.begin(9600); 
  lcd.init();                      // initialize the lcd
  Wire.begin();
  lcd.backlight();
  pinMode(ledPin,OUTPUT);
  pinMode(digitalIn, INPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
}

void loop()
{
  relStat=digitalRead(digitalIn);
  temper=myGetTemp();
  ThermoControll();
  write_lcd();
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  // compare the buttonState to its previous state
  if (button1State != lastButton1State) {
    if (button1State == HIGH) {
      button1PushCounter++;
      Serial.println(button1PushCounter);
      while(button1State==HIGH){
        val=analogRead(analogPin);
        low_hys=map(val, 0, 1023, 0, 25);
        button1State = digitalRead(button1Pin); 
//        Serial.println(low_hys); 
        lcd.setCursor(3,1);
        lcd.print(low_hys);  
      }
    }
  }
  if (button2State != lastButton2State) {
    // if the state has changed, increment the counter
    if (button2State == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      button2PushCounter++;
      Serial.println("on");
      Serial.println(button2PushCounter);
      while(button2State==HIGH){
        val=analogRead(analogPin);
        high_hys=map(val, 0, 1023, 25, 51);
        button2State = digitalRead(button2Pin); 
//        Serial.println(high_hys); 
        lcd.setCursor(10,1);
        lcd.print(high_hys);  
      }
    }
   }
//   Serial.println(relStat); 
   delay(1000);
} 
//#################################################################     
/*Read temperature value from TCN75A sensor*/
int myGetTemp()
{
  Wire.beginTransmission(0x48); /*TNC75A i2c address: 0x48 : 1001000 */
  Wire.requestFrom(0x48, 2);    /* adress jumpers are all open A2, A1 ja A0 */
  if(2 <= Wire.available())
  {
    temper = Wire.read();
    Wire.endTransmission();
  }
  return temper;
}
//#################################################################     
/*writes data to lcd display*/
int write_lcd()
{
  lcd.setCursor(0,0);
  lcd.print("Temperature: ");  
  lcd.print(temper);

  lcd.setCursor(0,1);
  lcd.print("LH:");  
  lcd.print(low_hys,DEC);

  lcd.setCursor(7,1);
  lcd.print("HH:");
  lcd.print(high_hys,DEC);
  
  if(relStat == LOW){
    lcd.setCursor(12,1);
    lcd.print(" NC");
   }
  
  if(relStat == HIGH){
    lcd.setCursor(12,1);
    lcd.print(" NO");
  }
  //lcd.setCursor(14,1);
}

//#################################################################     
/*Controls thermostat ouput high or low depending settings of hysteresis*/
int ThermoControll()
{
  if(relStat==LOW){
    if(low_hys >= temper) // If temperature goes lower than low_hys, output 7 goes HIG
    {
      digitalWrite(ledPin, HIGH);
    }
    else if(high_hys <= temper) // If temperature goes higher than high_hys, output 7 goes low 
    {
      digitalWrite(ledPin, LOW);
     }
  }
  
  if(relStat==HIGH){
     if(low_hys >= temper) // If temperature goes lower than low_hys, output 7 goes HIG
     {
     digitalWrite(ledPin, LOW);
     }
     else if(high_hys <= temper) // If temperature goes higher than high_hys, output 7 goes low 
     {
     digitalWrite(ledPin, HIGH);
     }
  }
}

