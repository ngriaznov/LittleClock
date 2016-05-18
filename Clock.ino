#include <MIDI.h>

#include <FrequencyTimer2.h>

#define                       CLOCK                 0xF8                // clock pulse message    
#define                       START                 0xFA                // clock start message
#define                       STOP                  0xFC                // clock stop message
#define                       SENSING               0xFE                // active sensing message

float                         bpm                 = 130.0;             
volatile unsigned long        clockInterval;     

const int                     ledPin              = LED_BUILTIN;        // pin with a LED
int                           ledState            = LOW;                // blink every whole beat 1/1
volatile unsigned long        clockCount          = 0;                  // clock counter, used for LED 
volatile bool                 started             = false;              // we'll send start just once 
volatile bool                 note                = false;              // we'll send start just once 

void setup(void) {
  // reserve led action
  pinMode(ledPin, OUTPUT);

  // limit range
  bpm = constrain(bpm, 60.0, 240.0);
  
  // initialize clock interrupt, interval is in microseconds
  clockInterval = 60000000/(24 * bpm);

  // give some time for host device to initialize (optional)
  delay(2000);

  // set the timer. this zero-jitter timer, but only one available
  FrequencyTimer2::setPeriod(clockInterval);
  FrequencyTimer2::setOnOverflow(clockTick);
  FrequencyTimer2::enable();
}

// functions called by IntervalTimer 
void clockTick(void) {
  
  if (!started){
    usbMIDI.sendRealTime(START);
    started = true;
  }
  clockCount++;
  usbMIDI.sendRealTime(CLOCK);

  // blink led at 1/4 of the beat
  if (clockCount%24 == 0){
    notify(true);
    clockCount = 0;
  }
  else {
    notify(false);
  }
}

// notify the user with whole note beat using on-board LED
void notify(bool on){
  if (on) {    
    ledState = HIGH;      
  } 
  else {
    ledState = LOW;
  }
  digitalWrite(ledPin, ledState);
}

void loop(void) {

  // here we need to skip incoming messages
  while (usbMIDI.read()) {
  }
}
