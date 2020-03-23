/*
    Mixed time and frequency approach of velocity calucaiton

    This approach was introduced in the paper:
    "An embedded system for position and speed measurement adopting incremental encoders."
    by Petrella, Roberto, and Marco Tursini.
    IEEE Transactions on industry applications 44.5 (2008): 1436-1444.

   The main benefit of this approach is that has very low error for
   both low and high speed calculation using the incremental encoder

   MIT License

   Antun Skuric
   antun.skuric@outlook.com
   March, 2020
*/


// Encoder variables
#define CPR 600  // counts per revolution
#define PPR (4*CPR) // pulses per revolution

// Only pins 2 and 3 are supported
#define encoderPinA 2             // Arduino UNO interrupt 0
#define encoderPinB 3             // Arduino UNO interrupt 1

// global encoder variables
long int impulse_counter = 0;         // impulse counter
long long impulse_timestamp_us = 0;   // last impulse timestmap


// utility defines
#define P2RAD (2 * 3.1415 / PPR)  // pulses to radians
#define P2DEG (360 / PPR)   // pulses to degres
#define P2RPM (60 / PPR)    // pulses to rpm

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
  // display the position and velocity to setial port
  Serial.print(encoder_position());
  Serial.print("rad");
  Serial.print("\t");
  Serial.println(encoder_velocity());
  Serial.println("rad/s");
}

// function calculating position using the incremental encoder
float encoder_position(){
  // rad
  return impulse_counter * P2RAD;
  // deg
  // return impulse_counter * P2DEG;
}

// function calculating angular velocity using the incremental encoder
float encoder_velocity(){
  // rad/s
  return pulse_per_sec_MTF(impulse_counter, impulse_timestamp_us) * P2RAD;
  // rpm
  // return pulse_per_sec_MTF(impulse_counter, impulse_timestamp_us) * P2RPM;
}


// funciton using mixed time and frequency measurement technique
// returns the float pulses per second value
float pulse_per_sec_MTF(long counter, long last_impulse_timestamp_us) {
  static float Th_1, pps;
  static long N, prev_timestamp_us;

  // timestamp
  long timestamp_us = micros();
  // sampling time calculation
  float Ts = (timestamp_us - prev_timestamp_us) * 1e-6;
  // time from last impulse
  float Th = (timestamp_us - last_impulse_timestamp_us) * 1e-6;
  long dN = counter - N;

  // Pulse per second calculation (Eq.3.)
  // dN - impulses received
  // Ts - sampling time - time in between function calls
  // Th - time from last impulse
  // Th_1 - time form last impulse of the previous call
  // only increment if some impulses received
  pps = dN != 0 ? dN / (Ts + Th_1 - Th) : pps;

  // save variables for next pass
  prev_timestamp_us = timestamp_us;
  // save velocity calculation variables
  Th_1 = Th;
  N = counter;
  return pps;
}

//  Encoder interrupt callback functions
//  enabling CPR=4xPPR behaviour
int A_prev = 0;
int B_prev = 0;
// A channel
void doEncoderA() {
  int A = digitalRead(encoderPinA);
  if ( A != A_prev ) {
    if (A_prev == B_prev) {
      impulse_counter += 1;
    } else {
      impulse_counter -= 1;
    }
    A_prev = A;
    impulse_timestamp_us = micros();
  }
}
// B channel
void doEncoderB() {
  int B = digitalRead(encoderPinB);
  if ( B != B_prev ) {
    if ( A_prev != B_prev ) {
      impulse_counter += 1;
    } else {
      impulse_counter -= 1;
    }
    B_prev = B;
    impulse_timestamp_us = micros();
  }
}

