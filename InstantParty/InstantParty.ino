/* Instant party example
  trigger from motion sensor, play a tune 
  and swtich on a power switch tail. 
  Derived from Sparkfun example: 
https://learn.sparkfun.com/tutorials/mp3-player-shield-hookup/example-sketch-mp3-trigger

NOTE:
  Setup the SDCard with the MP3 Tracks renamed:
  TRACK001.mp3, TRACK002.mp3, etc.
  The play track function then uses the number (e.g integer 1 plays TRACK001.mp3
*/
#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object
// These variables are used in the MP3 initialization to set up
// some stereo options:
const uint8_t volume = 0; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 0;  // Mono setting 0=off, 3=max

int numtracks = 6;
int trackplaying=1;
#define TIMETOPLAY 15000
long time_elapsed; // keep track of how long the track has played.
unsigned long time_started;
boolean triggerEnabled = false;

#define MOTIONPIN A0
#define POWERSWITCH A4
#define BUTTONPIN A5
#define LEDPIN A1
void setup() {
  Serial.begin(115200);
  pinMode(POWERSWITCH,OUTPUT);
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(POWERSWITCH,LOW);
  pinMode(MOTIONPIN, INPUT);
  pinMode(BUTTONPIN,INPUT);
  // enable pullup for PIR motion sensor input
  digitalWrite(MOTIONPIN,HIGH);
  digitalWrite(BUTTONPIN,HIGH);
  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}

void loop() {

  if (digitalRead(BUTTONPIN) == LOW){
    triggerEnabled = !triggerEnabled;
    if(triggerEnabled){
      Serial.println("trigger enabled");
      digitalWrite(LEDPIN,HIGH);
      delay(5000); //5 seconds to get out of the way.
    }
    else{
      Serial.println("trigger disabled");
      digitalWrite(LEDPIN,LOW);
    }
    delay(100);
  }
  if (MP3player.isPlaying()){
    if (millis() -time_started > TIMETOPLAY){
      MP3player.stopTrack();
      digitalWrite(POWERSWITCH,LOW);
      delay(5000);// wait 5 seconds before we start looking again
    }
  }
  if  ((digitalRead(MOTIONPIN) == LOW) && triggerEnabled)
  {
     /* If track is playing return */
     if (MP3player.isPlaying())
        return;
    /* Use the playTrack function to play a numbered track: */
    uint8_t result = MP3player.playTrack(trackplaying);
    digitalWrite(POWERSWITCH,HIGH);
    Serial.println("motion detected");
    Serial.print("playing track: ");
    Serial.println(trackplaying);

    // Tracks go from 1-numtracks
    trackplaying = trackplaying>=numtracks?1:trackplaying+1;

    time_started = millis();
    
  }
  //delay(1000);
}

// initSD() initializes the SD card and checks for an error.
void initSD()
{
  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
    sd.initErrorHalt();
  if(!sd.chdir("/")) 
    sd.errorHalt("sd.chdir");
}

// initMP3Player() sets up all of the initialization for the
// MP3 Player Shield. It runs the begin() function, checks
// for errors, applies a patch if found, and sets the volume/
// stero mode.
void initMP3Player()
{
  uint8_t result = MP3player.begin(); // init the mp3 player shield
  if(result != 0) // check result, see readme for error codes.
  {
    // Error checking can go here!
  }
  MP3player.setVolume(volume, volume);
  MP3player.setMonoMode(monoMode);
}

void blinkLED(int n)
{
  pinMode(LEDPIN,OUTPUT);
  for(int i =0; i < n; i++){
    digitalWrite(LEDPIN,HIGH);
    delay(750);
    digitalWrite(LEDPIN,LOW);
    delay(750);
  }
}
  
