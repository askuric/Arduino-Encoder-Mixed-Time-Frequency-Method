/*
 *  Mixed time and frequency approach of velocity calucaiton
 *  
 *  This approach was introduced in the paper:
 *  "An embedded system for position and speed measurement adopting incremental encoders." 
 *  by Petrella, Roberto, and Marco Tursini. 
 *  IEEE Transactions on industry applications 44.5 (2008): 1436-1444.
 * 
 * The main benefit of this approach is that has very low error for 
 * both low and high speed calculation using the incremental encoder
 * 
 * MIT License
 * Antun Skuric 
 * antun.skuric@outlook.com
 * March, 2020
 */


// encoder variables
long int counter = 0;  // impulse counter
long long impulse_timestamp_us = 0;   // last impulse timestmap  
int A1_=0;
int B1_=0;
// Only pins 2 and 3 are supported
int  encoderPinA = 2;   // Arduino UNO interrupt 0
int encoderPinB = 3;  // Arduino UNO interrupt 1


void setup () {

  // encoder pins
  pinMode(encoderPinA, INPUT_PULLUP); // new method of enabling pullups
  pinMode(encoderPinB, INPUT_PULLUP); 
  
  // pina
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  // pinb
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  // debugging port
  Serial.begin(115200);
}

void loop() {
    delay(10); 
    Serial.println(velocity_MTF());
}

// funciton using mixed time and frequency measurement technique
// returns the float velocity value
//
// MAKE SURE TO IMPLEMENT MISSING LINE OF CODE
float velocity_MTF()
{
  static float Th_1,freq;   
  static long N, prev_timestamp_us;

  // timestamp 
  long timestamp_us = micros();
  // sampling time calculation
  float Ts = (timestamp_us - prev_timestamp_us)*1e-6;
  // time from last impulse
  float Th = (timestamp_us - impulse_timestamp_us)*1e-6;
  long dN = counter - N;

  // Velocity calculation (Eq.3.)
  // dN - impulses received
  // Ts - sampling time - time in between function calls
  // Th - time from last impulse 
  // Th_1 - time form last impulse of the previous call
  // only increment if some impulses received
  freq = dN != 0 ? dN /(Ts + Th_1 - Th) : freq; 

  // modify this line to get the real velocity value
  // velocity = frequency * 2 * PI / CPR
  float vel = freq;
  
  // save variables for next pass
  prev_timestamp_us = timestamp_us;
  // save velocity calculation variables
  Th_1 = Th;
  N = counter;
  return vel;
}

//  Encoder methods
//  enabling CPR=4xPPR behaviour
// A channel
void doEncoderA(){
  int A = digitalRead(encoderPinA);
  if( A!= A1_ ){
    if(A1_ == B1_){
      counter += 1;
    }else{
      counter -= 1;
    }
   A1_ = A;
   impulse_timestamp_us = micros();
  }
}

// B channel
void doEncoderB(){
  int B = digitalRead(encoderPinB);
  if( B!= B1_ ){
    if( A1_ != B1_ ){
      counter += 1;
    }else{
      counter -= 1;
    }
    B1_ = B;
    impulse_timestamp_us = micros();
  }
}

