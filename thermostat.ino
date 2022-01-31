byte calculatePID() { // PID calculation routine
  tempIntC = readTemp() * 10.0; //read the ds18b20 sensor and store it in integer in decadegree format
  if (sensNewData) {
    //PID_output_prev = PID_output;
    PID_err = tempGoal - tempIntC; // +5 calculate error between set value and real temperature
    PID_p = float(0.01 * kp * PID_err); //Calculate the P value
    PID_i = float(0.01 * PID_i + (ki * (PID_err))); //Calculate the I value in a range on +-5
    //For derivative we need real time to calculate speed change rate
    timePrev = Time; // the previous time is stored before the actual time read
    Time = millis(); // actual time read
    elapsedTime = (Time - timePrev) / 1000;
    //Now we can calculate the D value
    PID_d = float(0.01 * kd * ((PID_err - PID_err_prev) / elapsedTime));
    //Final total PID value is the sum of P + I + D
    PID_value = (float(float(kpid) / 100.00) * float(PID_p + PID_i + PID_d)) * -1L; // kpid/100 - apply afterall coefitient
    // map PID value: shift bottom to minPower, truncate top to maxPower to fit within range minPower...maxPower
    //PID_value = map(PID_value, 0, 255, minPower, maxPower);
    //We define PWM range between 0 and 255
    //PID_value = truncInt(PID_value, 0, 255);
    // truncInt(PID_value, minPower, maxPower);
    PID_output = PID_value;
    if (PID_output > maxPower) {
      PID_output = maxPower;
    }
    if (PID_output < minPower) {
      PID_output = 0;
    }
    //Now we can write the PWM signal to the FAN
    //setFanDim(PID_value);
    PID_err_prev = PID_err;     //Remember to store the previous error for next loop.
    return (PID_output);
  }
}

void PID_DEST_HYST() {
  calculatePID();//make all necessary math involved in PID
  getHYST(); // >HYSTwithin(0,1)
  getDEST(); // >DESTaction(0,1)
  outVal = PID_output;
  ICONoutput = 0xe3;
  if (HYSTwithin) {
    outVal = 0;
    ICONoutput = 0x19;
  }
  if (DESTaction && !PID_output) {
    outVal = minPower;
    ICONoutput = 0x18;
  }
  //takePlotSample();     //   UI PLOT SCREEN SAMPLING
  // now we'll aaply output value and take plot sample
  if(sensNewData){
    sensNewData=0;
    takePlotSample();     //   UI PLOT SCREEN SAMPLING
  }
  setFanDim(255 - outVal); //Now we can write the PWM signal to the FAN
}

void getHYST() {
  if (HYSTtime) {
    unsigned long timeNow = millis();
    if (timeNow > HYSTcycleStart && timeNow < HYSTcycleStart + (HYSTtime * HYST_TIME_MULT))  HYSTwithin = 1;
    else HYSTwithin = 0;
  }
  else HYSTwithin = 0;
}

void getDEST() {
  if (DESTperiod && DESTtime) {
    unsigned long timeNow = millis();
    unsigned long DESTcycleEnd = DESTcycleStart + (DESTperiod * DEST_PERIOD_MULT) + (DESTtime * DEST_TIME_MULT);
    if (DESTcycleStart < timeNow) { // check if fan was turned off in the past
      if (timeNow < (DESTcycleStart + (DESTperiod * DEST_PERIOD_MULT))) DESTaction = 0;
      else {
        if (timeNow > DESTcycleEnd) { // overflow,
#if defined(DEBUG_MODE)
          Serial.println(F("DEST!"));
#endif
          DESTcycleStart = millis();
          DESToverflow = 1;
          DESTaction = 0;
        }
        else DESTaction = 1; // within TIME range
      }
    }
  }
  else DESTaction = 0;
}

float readTemp() { // used to get current temperature from a single ds18b20 sensor
  unsigned long timeSinceLastRead = millis() - sensLastRead;
  static float tempC;
  //Serial.print(timeSinceLastRead);
  //Serial.print(" ");
  if (timeSinceLastRead >= sensReadDelay) {
    tempC = sensors.getTempCByIndex(0); // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    if (tempC != DEVICE_DISCONNECTED_C) {  // Check if reading was successful
      //Serial.println(tempC);
      if (sensFail == 1) { // SENSOR JUST RECOVERED FROM FAIL: recover last screen mode if sensoor ok, clear flag
        digitalWrite(13, 0);
        wakeScreen();
        if (!returnToManual) {
          screenMode = screenLast;
          setMode = 0;
          isConstPower = 0;
        }
        else {
          screenMode = UICONST_SCREEN;
          setMode = 1;
          isConstPower = 1;
        }
        sensFail = 0;
        lastActionTime = millis();
        if (screenMode != UICONST_SCREEN) { // back from constPower if was not in UICONST_SCREEN
        }
      }
      if (tempAdj) tempC += float(tempAdj) / 10.0; // implement temperature adjust shift
      sensNewData = 1;
      sensors.setResolution(SENSOR_RESOLUTION); //before each measurement, set internal ADC resolution 9...12
      sensors.requestTemperatures();  // Send the command to get temperatures
      sensLastRead = millis();
      return (tempC);
    }
    else { // TEMPERATURE SENSOR FAILED!!! (save to constant mode)
      if (!sensFail) { // SENSOR JUST FALLED INTO FAIL MODE
        digitalWrite(13, 1);
        wakeScreen();
        MSGsensorFail();
        delay(1500);
        if (isConstPower)returnToManual = 1;
        else returnToManual = 0;
        if (screenMode) screenLast = screenMode; // remember last screen
        sensFail = 1;
        lastActionTime = millis();
      }
      isConstPower = 1;
      screenMode = UICONST_SCREEN; // go to manual mode
      setMode = 1;
      sensLastRead = millis();
      return (65535);
    }
  }
  else { //it is not time to get new readings
    sensNewData = 0;
    //Serial.print("wait ");
    //Serial.println(sensReadDelay);
    return (tempC);
  }
}

void setFanDim(int dimVal) { // call to set correct PWM at pin, so FAN works correct
  lastFANvalue = actualFANvalue;
  actualFANvalue = 255 - dimVal;
  analogWrite(PWM_OUT, dimVal);
  //store time of last turning off if PID just went down
  if (!actualFANvalue && lastFANvalue) FANwentOff();
  //store time of last turning on if PID just went up
  if (actualFANvalue && !lastFANvalue) FANwentOn();

}

void FANwentOff() {
  unsigned long timeNow = millis();
  //  fanTurnedOffTimer = timeNow;
  DESTcycleStart = timeNow;
  if (!DESToverflow) HYSTcycleStart = timeNow; // don't reset hyst timer if overflowed DEST
  else DESToverflow = 0;
#if defined(DEBUG_MODE)
  Serial.println(F("TURNED OFF"));
#endif
}

void FANwentOn() {
  //  fanTurnedOnTimer = millis();
#if defined(DEBUG_MODE)
  Serial.println(F("TURNED ON"));
#endif
}
