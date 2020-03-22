// -- Pin change interrupt
#include <PciManager.h>
#include <PciListenerImp.h>

long int encoderPos = 0;  // a counter for the dial
long int lastReportedPos = 1;   // change management
static boolean rotating=false;      // debounce management

// interrupt service routine vars
boolean A_set = false;              
boolean B_set = false;

int  encoderPinA = A0;   // put your pin B pin number
int encoderPinB = A1;  // put your pin B pin number

void doEncoderA();
void doEncoderB();
PciListenerImp listenerA(encoderPinA, doEncoderA);
PciListenerImp listenerB(encoderPinB, doEncoderB);


void setup() {
  Serial.begin(115200);
  
  // ENCODER PULLUP
  pinMode(encoderPinA, INPUT_PULLUP); // new method of enabling pullups
  pinMode(encoderPinB, INPUT_PULLUP); 
    
  PciManager.registerListener(&listenerA);
  PciManager.registerListener(&listenerB);
  Serial.println("Ready.");
}

void loop() { 
  delay(2);
  Serial.println(encoderPos);
}

// A channel
void doEncoderA(){

  // Test transition, did things really change? 
  if( digitalRead(encoderPinA) != A_set ) {  // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set ) 
      encoderPos += 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// B channel
void doEncoderB(){
  
  if( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if( B_set && !A_set ) 
      encoderPos -= 1;

    rotating = false;
  }
}

