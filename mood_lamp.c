/****************************************
 * Vecsei Gábor
 *
 * vecseigabor.x@gmail.com
 * https://gaborvecsei.wordpress.com/
 * https://github.com/gaborvecsei
 * https://www.linkedin.com/in/vecseigabor
 *
 * 2016 Summer
 ****************************************/

 /****************SUMMARY****************
  * This is a project for my girlfriend.
  * A mood light what you can control with claps.
  * 
  * The states are:
  * Turn on -> Cycle thru colors -> Stop at a color -> Turn off
  * 
  * We only need an RGB led,
  * 3 resistors
  * Wires
  * and of course...Arduino (I use Arduino nano)
  ****************************************/

/*
 * Sound sensor:
 * VCC -> 3V3
 * GND -> GND
 * OUT -> PIN 4
 */

int micPin = 4;
//This is where we record our Sound Measurement
int micVal = HIGH; 
boolean bAlarm = false;
//Record the time that we measured a sound
unsigned long lastSoundDetectTime;
//Number of milli seconds to keep the sound alarm high
int soundAlarmTime = 500;

/*
 * RGB Led:
 * CATHODE -> GND
 * RED -> PIN 11
 * GREEN -> PIN 10
 * BLUE -> PIN 9
 * 
 * I use 280 Ohm resistors on red, gree, blue pins
 */

const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

//The built in led on the board
const int ledOnArduino = 13;

/*
 * States are:
 * 0 - turned off
 * 1 - turned on with color cycle
 * 2 - turned on with a single color
 */
int state = 0;


/*
 * Clap detection, returns true if we hear a clap
 * else it returns false
 */
bool ClapDetection(){
  //Read the value in from the sound sensor
  micVal = digitalRead (micPin) ;
  bool iHearClap = false;

  if (micVal == LOW){
    //If we hear a sound
    //Record the time of the sound alarm
    lastSoundDetectTime = millis();
    if (!bAlarm){
      Serial.println("I hear sound!");
      bAlarm = true;
      iHearClap = true;
      //Change states here
      state ++;
      if(state > 2) state = 0;
    }
  } else {
    if((millis() - lastSoundDetectTime) > soundAlarmTime  &&  bAlarm){
      bAlarm = false;
      iHearClap = false;
    }
  }
  return iHearClap;
}

/*
 * Sets the color with PWM
 * Color range: 0 - 255
 */
void SetColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}


/*
 * Color translation smoothly
 * We can determine the speed with dly
 */

int rgbColour[3];
 
int RgbCycle2(int dly){
  if(state == 1){
    //Start with red.
    rgbColour[0] = 255;
    rgbColour[1] = 0;
    rgbColour[2] = 0;  
  
    //Choose the colours to increment and decrement.
    for (int decColour = 0; decColour < 3; decColour += 1) {
      int incColour = decColour == 2 ? 0 : decColour + 1;
  
      //Cross-fade the two colours.
      for(int i = 0; i < 255; i += 1) {
        rgbColour[decColour] -= 1;
        rgbColour[incColour] += 1;
        //Set the color
        SetColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
        //Delay like this because we can detect claps between the delays
        //delay(dly);
        for(int i = 0; i< dly; i++){
          delay(1);
          if(ClapDetection()){
            //Exit from the for loop
            return 0;
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void setup ()
{
  Serial.begin(9600);  
  //Input from sound sensor
  pinMode (micPin, INPUT);

  //Turn it off because I don't want it to light up
  pinMode (ledOnArduino, OUTPUT);
  digitalWrite(ledOnArduino, LOW);

  //Led is off at the start
  SetColourRgb(0,0,0);
}

void loop ()
{

  //We have to detect the first clap here
  ClapDetection();

  if(state == 0){
    //Colors are turned off
    SetColourRgb(0,0,0);
  } else if(state == 1){
    //Cycle thru the colors
    RgbCycle2(10);
  } else if(state == 2){
    //Stop at the actual color
    SetColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
  } else{
    //Turned off
    SetColourRgb(0,0,0);
  }
}