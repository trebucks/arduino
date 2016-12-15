

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define RED_LED_PIN 7
#define GREEN_LED_PIN 8
#define YELLOW_LED_PIN 4
#define BUTTON_PIN 5
#define SERVO_PIN 9

#define LOCK_POS  90
#define OPEN_POS  0


int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers



// int getFingerprintIDez();
uint8_t getFingerprintID();

uint8_t getFingerprintEnroll(uint8_t id);


// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo myservo;  // create servo object to control a servo




void setup()  
{
  Serial.begin(9600);
  Serial.println("fingertest");
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }

  digitalWrite(RED_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(GREEN_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(YELLOW_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for half a second
  digitalWrite(RED_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(GREEN_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(YELLOW_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for half a second
  myservo.write(LOCK_POS);    

  Serial.println("Waiting for valid finger...");
}

void signalRed()
{
  digitalWrite(RED_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for half a second
  digitalWrite(RED_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
}

void signalGreen()
{
  digitalWrite(GREEN_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for half a second
  digitalWrite(GREEN_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
}

void checksetupButton()
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

     if (buttonState == HIGH) {
          uint8_t id = 0;
          Serial.print("Enrolling ID #");
          Serial.println(id);
          
          while (!  getFingerprintEnroll(id) );

     }
    }
  }

  // // set the LED:
  // digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

}

void loop()
{

  // if enroll button is pressed, light yellow led when enrolling, green for ok, red for error
  // when powering up try recognize for 10 seconds, red and green for success failure
  getFingerprintID();
  delay(50);            //don't need to run this at full speed.
  checksetupButton();
} 



void waitForOK()
{
  uint8_t p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
}

void dumpMessage(uint8_t p)
{
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      break;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      break;
    default:
      Serial.println("Unknown error");
      break;
  }

}


// void loopEnroll()                     // run over and over again
// {
//   Serial.println("Type in the ID # you want to save this finger as...");
//   uint8_t id = 0;
//   while (true) {
//     while (! Serial.available());
//     char c = Serial.read();
//     if (! isdigit(c)) break;
//     id *= 10;
//     id += c - '0';
//   }
//   Serial.print("Enrolling ID #");
//   Serial.println(id);
  
//   while (!  getFingerprintEnroll(id) );
// }

uint8_t getFingerprintEnroll(uint8_t id) {
  uint8_t p = -1;
  Serial.println("Waiting for valid finger to enroll");
  digitalWrite(YELLOW_LED_PIN, HIGH);
  waitForOK();
  

  // OK success!

  p = finger.image2Tz(1);
  dumpMessage(p);
  digitalWrite(YELLOW_LED_PIN, LOW);
  if (p != FINGERPRINT_OK)
  {
    signalRed();
    return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  p = -1;
  Serial.println("Place same finger again");
  digitalWrite(YELLOW_LED_PIN, HIGH);
  waitForOK();
  // OK success!

  p = finger.image2Tz(2);
  dumpMessage(p);
  digitalWrite(YELLOW_LED_PIN, LOW);
  if (p != FINGERPRINT_OK)
  {
    signalRed();
    return p;
  }
  
  
  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    signalRed();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    signalRed();
    return p;
  } else {
    Serial.println("Unknown error");
    signalRed();
    return p;
  }   
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    signalGreen();
    return 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    signalRed();
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    signalRed();
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    signalRed();
    return p;
  } else {
    Serial.println("Unknown error");
    signalRed();
    return p;
  }   
}


/*-------------------------*/






// void loopIdentify()                     // run over and over again
// {
// }

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  dumpMessage(p);
  if (p != FINGERPRINT_OK)
    return p;

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    signalGreen();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    signalRed();
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  myservo.write(OPEN_POS);    
  delay(3000);              // wait for half a second
  myservo.write(LOCK_POS);    

}

// // returns -1 if failed, otherwise returns ID #
// int getFingerprintIDez() {
//   uint8_t p = finger.getImage();
//   if (p != FINGERPRINT_OK)  return -1;

//   p = finger.image2Tz();
//   if (p != FINGERPRINT_OK)  return -1;

//   p = finger.fingerFastSearch();
//   if (p != FINGERPRINT_OK)  return -1;
  
//   // found a match!
//   Serial.print("Found ID #"); Serial.print(finger.fingerID); 
//   Serial.print(" with confidence of "); Serial.println(finger.confidence);



//   return finger.fingerID; 




/*---------*/

// // twelve servo objects can be created on most boards

// int pos = 0;    // variable to store the servo position

// void setup() {
  
// }

// void loop() {
// //  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
// //    // in steps of 1 degree
// //    myservo.write(pos);              // tell servo to go to position in variable 'pos'
// //    delay(5);                       // waits 15ms for the servo to reach the position
// //  }
// //  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
// //    myservo.write(pos);              // tell servo to go to position in variable 'pos'
// //    delay(5);                       // waits 15ms for the servo to reach the position
// //  }
// }


/*----------*/

// // the setup function runs once when you press reset or power the board
// void setup() {
//   // initialize digital pin 13 as an output.
//   pinMode(4, OUTPUT);
//   pinMode(7, OUTPUT);
//   pinMode(8, OUTPUT);
// }

// // the loop function runs over and over again forever
// void loop() {
//   digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
//   digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
//   digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
//   delay(1000);              // wait for a second
//   digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
//   digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
//   digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
//   delay(1000);              // wait for a second
// }



/*---------*/




// // constants won't change. They're used here to
// // set pin numbers:
// const int buttonPin = 5;    // the number of the pushbutton pin
// const int ledPin = 13;      // the number of the LED pin

// // Variables will change:
// int ledState = HIGH;         // the current state of the output pin

// void setup() {
//   pinMode(buttonPin, INPUT);
//   pinMode(ledPin, OUTPUT);

//   // set initial LED state
//   digitalWrite(ledPin, ledState);
// }

// void loop() {
// }


