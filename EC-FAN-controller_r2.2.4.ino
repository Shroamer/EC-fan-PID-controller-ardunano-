//#define DEBUG_MODE
//#define DEBUG_LOCATE
//#define BURN_NEW_EEPROM 1 // uncomment for the FIRST time you start with UPDATED EEprom struct (it will save a new defaults)

#define REV_NO "r2.2.4 25.01.22"
/*
  (c) Shroamer, dec`2021 (shroamer(at)gmail(dot)com)
27.12.21
  r2.2.3
    + aproximate >1/x readings
    + fix SENSOR FAIL screen
    + adjust some screens layout
    + adopt icons for:
      read temp   display.write(char(ICONtempRead));
      goal temp   display.write(char(ICONtempGoal));
      power       display.write(char(ICONoutput));
      error       display.write(char(ICONerror));
      PIDerror    display.write(char(PIDerror));
    + adopt icons for plot screens:
      min range   display.write(char(ICONminRange));
      max range   display.write(char(ICONmaxRange));
    + fixed: if waked by sensFail, it will remember 0 as lastScreen and will never wake again (wake to 0)
    + fixed: when sensFail occured while manual mode, after recovery it will not return to manual, so isConstPower flag is introduced
    + fixed: plotting continues even in isConstMode.

26.12.21  
  r2.2.2
    + increase ds18b20 ADC reading resolution from 9 to 11 bits
      * increased PID output smoothness
    + skipping ds18b20 readings delay
      * UI becomes extremely fast (FPS x10)
      * plot sampling becomes more even
    + change Tadj increment from 0.5 to 0.1 degree because of precise readings
      * more precise Tadjusment is possible
    + fix a bug when wake up by encoder did not perform recall of last screenMode
      * waking uo with encoder with no problems
    + fixed tAdj bar when negative
    + change PIDx multiplier bar style
    + move PIDx screen after P, I, D
    + fix prefilled by nulls plot data. now it is filled with actual temperature
     
  
  Local Search links (copy it and search to find corresponding place):
  // ***** PUT NEW SCREEN HERE *****
  // ***** INSERT NEW SCREEN INTO SCREEN SEQUENCE HERE *****
  // ***** DEFINE WHAT TO APPLY AFTER EXITING SETMODE HERE *****
  // ***** PUT YOUR NEW EEprom DATA HERE *****

  ENCODER LIB LINK: https://github.com/mathertel/RotaryEncoder/blob/master/src/RotaryEncoder.cpp
  PID code sample took here :http://electronoobs.com/eng_arduino_tut24_code3.php

  hardware:
  thermal sensor dallas ds

  arduino wiring:
  VIN ---> +10v input from FAN source
  GND ---> GND input from FAN source
  D3  ---> encoder switch (to GND)
  D7  ---> 1-wire bus for DS18b20
  D11 ---> PWM output (thru amplifyer to 0...10v FAN input)
  A2  ---> encoder left (to GND)
  A3  ---> encoder right (to GND)
  A4  ---> ssd1306 display SDA
  A5  ---> ssd1306 display SCL
*/
#define PWM_OUT 11 // pin for PWM output (3,5,6,9,10,11)
#define ONE_WIRE_BUS 7 // data pin for dallas ds18b20 sensor
#define PIN_IN1 A2 // rotary encoder pin1
#define PIN_IN2 A3 // rotary encoder pin2
#define PIN_SW 3 // rotary encoder button
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#include <Arduino.h>
//#include <RotaryEncoder.h> // https://github.com/mathertel/RotaryEncoder/blob/master/src/RotaryEncoder.cpp
#include <Rotary.h> // http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html?m=1
#include <EEPROM.h> // EEprom for storing settings
#include <OneWire.h> // for dallas ds18b20 sensor reading
#include <DallasTemperature.h> // for dallas ds18b20 sensor reading
#if defined(DEBUG_MODE)
#include <SPI.h> // include to enable serial debugging
#endif
#include <Wire.h> // i2c for ssd1306
#include <Adafruit_GFX.h> // for ssd1306 display
#include <Adafruit_SSD1306.h> // for ssd1306 display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

OneWire  oneWire(ONE_WIRE_BUS);  // init OneWire on pin 10 (a 4.7K resistor connecting +5 with DATA is necessary)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature ds18b20 sensor.

// A pointer to the dynamic created rotary encoder instance. This will be done in setup()
//RotaryEncoder *encoder = nullptr;

//   UI VARIABLES
byte screenMode = 1; // screen mode: 0=off; 1=main screen;
byte screenLast = 1; // store last screen mode to wake into
byte storeScreen = 0; // should we store screen when go sleep? (wears EEPROM if menu used actively)
byte setMode = 0; // 0: scroll to list screenMode, click to enter setMode; 1: scroll to change value, click to apply;
//   UI TIME:
unsigned int screenTimeout = 150; //store screen timeout in seconds
volatile unsigned long lastActionTime; // store millis() at lst call action. dim after timeout (float?)
//   UI FLAGS:
bool displayMissing = 0; // flag set if no display found
bool displaySleep = 0; // flag display sleeping
volatile byte displayRotate = 0; // rotate display flag
bool isConstPower = 0; // flag to act in const power mode (for manual, nosensor, or nodisplay mode
bool isAutoSave = 0; // should we automatically save settings?
bool returnToManual = 0; // flag if we should return to manual mode after sensFail fixed

//   UI PLOT SCREEN VARIABLES:
int screenTempArray[128];
byte screenPidArray[128];
byte keepSample = 1; // take 1 sample of keepSample. If set to 4 it will skip 3 and keep 1 next.
byte keepSampleIndex = 0; // index of sample to keep
unsigned long tempApproxSum =0;
unsigned long powerApproxSum =0;
byte arrayIndex = 0; // where we're in array
//int rangeMin = 0; // keep min/max values of array so we can set up vertical scale
//int rangeMax = 0;

// ***** INSERT NEW SCREEN INTO SCREEN SEQUENCE HERE *****
#define UIMAIN_SCREEN 1
#define UIP_SCREEN 2
#define UII_SCREEN 3
#define UID_SCREEN 4
#define UIPID_SCREEN 5
#define UIPOWERMIN_SCREEN 6
#define UIPOWERMAX_SCREEN 7
#define UITADJ_SCREEN 8
#define UITIMEOUT_SCREEN 9
#define UISCREENROTATE_SCREEN 10
#define UISTORESCREEN_SCREEN 11
#define UISETSAVE_SCREEN 12 // value of save-settings screenmode, so we don't store this screen into memory
#define UISETLOAD_SCREEN 13
#define UIPLOTTEMP_SCREEN 14
#define UIPLOTPID_SCREEN 15
#define UICONST_SCREEN 16 // value of const-mode screenmode; set the last available ScreenMode to cycle

#define ICONtempRead    0xb
#define ICONtempGoal    0xc
#define ICONoutput      0xe3
#define ICONminRange    0x18
#define ICONmaxRange    0x19
#define ICONerror       0x01
#define ICONPIDerror    0xf0
#define ICONselect      0x10

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 2 and 3.
Rotary rotary = Rotary(PIN_IN1, PIN_IN2);

//   ENCODER VARIABLES:
#define ENC_MULT 50 // encoder acceleration divider (more - slower)

//   INPUT HANDLERS:
int encIncrement = 0; // used outside of interrupt routine
volatile int encoderIncrement = 0; // stores negative each step CCW, positive each step CW, 0 if no or null sum action is registered

//   INPUT FLAGS:
volatile bool switchAction = 0; // 1 if an interrup action registered
//bool encoderCheck = 0; // call to check news from encoder
bool encoderAction = 0; //some specific action found from encoder(increment!=0)



//   TEMPERATURE CONTROL VARIABLES:
int tempIntC = 0; // integer of current temperature value in decadegree (1/10 degree) celsius
int tempGoal = 190; // goal temperature in decadegrees (180 = 18.0 degree)
int tempAdj = 0; // shifting sensor data by this value (1/10 deg)
int PID_err = 0; //= tempGoal - tempIntC; // to calculate temperature
int PID_err_prev = 0;
int PID_value = 0; // Resulting output PID value within min-max range
int PID_output = 0; // output is truncated by min/max Power
//   PID constants
int kp = 50; //90
int ki = 8; //30
int kd = 23; //80
int kpid = 100; //afterall coefitient(actually, it will be divided by /100)
//   PID values
int PID_p = 0;
int PID_i = 0;
int PID_d = 0;
//   PID time management:
float elapsedTime, Time, timePrev;

//   OUTPUT POWER MANAGEMENT
byte minPower = 35;
byte maxPower = 255;
byte constPower = 96;

//   SENSOR FLAGS:
bool sensFail = 0; // temperature sensor failed flag
bool sensNewData=0; // we have new readings
#define SENSOR_RESOLUTION 11  //set internal ds18b20 ADC resolution 9...12  
unsigned long sensLastRead=0;
int sensReadDelay = 750 / (1 << (12 - SENSOR_RESOLUTION));

//   EEPROM data storage:
// ***** ADD YOUR NEW EEprom VALUE ADDRESS HERE ***** (TODO)
#define ADDR_tempGoal       0 // +2 int
#define ADDR_kp             2 // +2 int
#define ADDR_ki             4 // +2 int
#define ADDR_kd             6 // +2 int
#define ADDR_kpid           8 // +2 int
#define ADDR_minPower       10 // +1 byte
#define ADDR_maxPower       11 // +1 byte
#define ADDR_isConstPower   12 // +1 byte
#define ADDR_constPower     13 // +1 byte
#define ADDR_tempAdj        14 // +2 int
#define ADDR_timeOut        16 // +2 u_int
#define ADDR_storeScreen    18 // +1 byte
#define ADDR_displayRotate  19 // +1 byte
#define ADDR_isAutoSave     20 // +1 byte
#define ADDR_keepSample     21 // +1 byte
#define ADDR_screenMode     1023 // byte, store it in the last bit 1023 = 0x3FF

void setup() {
#if defined(DEBUG_MODE)
  Serial.begin(115200);
#endif
  pinMode(PWM_OUT, OUTPUT); // init pin for PWM - fast as possible, so the fan can't go wild (inverted output circuitry)
  analogWrite(PWM_OUT, 255); // set default power (inverted output, so 255 is 0%)

  pinMode(PIN_SW, INPUT_PULLUP); // init pin for switch interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_SW), switchInt, FALLING); // [CHANGE, RISING FALLING]switch interrupt

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    displayMissing = 1;
  }
  
  sensors.begin(); // Start up ds18b20 library
  sensLastRead = millis(); // resetting timer, because now delay is still included within requestTemperatures, so we need to pass
  sensors.setResolution(SENSOR_RESOLUTION); //before each measurement, set internal ADC resolution 9...12
  sensors.requestTemperatures();  // Send the command to get temperatures
  delay(sensReadDelay);
  tempIntC = readTemp() * 10; //read the ds18b20 sensor and store it in integer in decadegree format to fulfill array
  screenTempArrayInit();   // init screenTempArray
  screenPidArrayInit();
  sensors.setWaitForConversion(false); // exclude delay within requestTemperatures for smooth UI in loop
  
#if defined(BURN_NEW_EEPROM)
  settingsPut(); // save new EEProm for the first time
#endif
  settingsGet(); // load seeting from eeprom
  if (displayRotate) { // update display rotation
    display.setRotation(2);
  }
  screenLast = EEscreenModeGet();
  screenMode = screenLast;
  if (!displayMissing) { // setup display
    splashScreen();
  }

  //  To use other pins with Arduino UNO you can also use the ISR directly.
  // Here is some code for A2 and A3 using ATMega168 ff. specific registers.
  // Setup flags to activate the ISR PCINT1.
  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.
  lastActionTime = millis(); // reset screen timeout
}

void loop() {
  //if (encoderCheck) encoderRoutine(); // encCHECK: call to process new encoder data on flag rise before drawing UI
  if (!displayMissing) {  //   DISPLAY is OK
    //takePlotSample();     //   UI PLOT SCREEN SAMPLING
    if (!isConstPower) {  //   PIDmode ACTION: calc PID, apply result (TODO - apply min/max to result)
      calculatePID();//make all necessary math involved in PID
      setFanDim(255 - PID_output); //Now we can write the PWM signal to the FAN
    }
    else {                //   CONSTmode ACTION: apply constPower to output, set screenMode UICONST_SCREEN if not 0 (constpower settings)
      if (screenMode) screenMode = UICONST_SCREEN;
      if (!setMode) setMode = 1;
      //tempIntC = readTemp() * 10;
      calculatePID();
      setFanDim(255 - constPower); // set the const power value
    }
    if ((millis() - lastActionTime) > (screenTimeout * 1000L) && screenMode != 0) { // SLEEP TIMEOUT: goto SLEEP, store last screenmode
      if (!sensFail) screenLast = screenMode; // store screenMode for wake (only in normal mode)
      if (storeScreen) EEscreenModePut(); //save screen to eeprom only when it different (save eeprom life)
      screenMode = 0;
    }
    if (switchAction && !screenMode) { // WAKEUP by SWITCH: - wake up and setup actual screen
      if (!isConstPower) { // DISPLAY, SWITCH & SLEEP @PID: waking, going to start screen
        screenMode = screenLast;
      }
      else { // DISPLAY, SWITCH & SLEEP @CONST: switch at ConstPower rules to 9 screenMode with enabled scrolling
        screenMode = UICONST_SCREEN;
        setMode = 1;
      }
      switchAction = 0;
    }

    drawUi(); // MAIN UI CALL
  }
  else { // if the DISPLAY IS MISSING we have to get in alternative constant power mode with adjustment and automatic settings saving
    digitalWrite(13, 1);
    if (switchAction) {
      switchAction = 0;
      EEPROM.put(ADDR_constPower, constPower);
    }
    if (encoderAction) { // call to process new encoder data on flag rise
      encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
      constPower = scrollInt(constPower, encIncrement, 0, 255, 0); // increment constPower
      setFanDim(constPower); // set the const power value
    }
  }

#if defined(DEBUG_STATS)
  //Serial.print(F(" SCR-"));  Serial.print(int(screenMode));
  //Serial.print(F(" SET-"));  Serial.print(int(setMode));
  //Serial.print(F(" TMP-"));  Serial.print(int(tempIntC));
  //Serial.print(F(" GOA-"));  Serial.print(int(tempGoal));
  //Serial.print(F(" PID-"));  Serial.print(int(PID_value));
  //Serial.print(F(" CON-"));  Serial.print(int(isConstPower));
  //Serial.print(F(" SF-"));  Serial.print(int(sensFail));
  //Serial.print(F(" DR-"));  Serial.print(int(displayRotate));
  //Serial.print(F(" STO-")); Serial.print(screenTimeout * 1000L);
  Serial.print(F(" AIN-")); Serial.print(arrayIndex);
  Serial.print(F(" ROT-")); Serial.print(displayRotate);
  //Serial.print(F(" kpid-"));  Serial.print(int(kpid));
  //Serial.print(F(" TOL-")); Serial.print((screenTimeout * 1000L) - (millis() - lastActionTime));
  //Serial.print(F("   "));  Serial.print(millis() - lastActionTime);
  Serial.println();
#endif
}
