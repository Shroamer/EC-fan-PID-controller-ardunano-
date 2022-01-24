byte calculatePID() { // PID calculation routine
  tempIntC = readTemp() * 10.0; //read the ds18b20 sensor and store it in integer in decadegree format
  if (sensNewData) {
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
    takePlotSample();     //   UI PLOT SCREEN SAMPLING
    return (PID_output);
  }
}

float readTemp() { // used to get current temperature from a single ds18b20 sensor
  unsigned long timeSinceLastRead = millis() - sensLastRead;
  static float tempC;
  //Serial.print(timeSinceLastRead);
  //Serial.print(" ");
  if (timeSinceLastRead >= sensReadDelay) {
    tempC = sensors.getTempCByIndex(0); // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    if (tempC != DEVICE_DISCONNECTED_C) {  // Check if reading was successful
      //Serial.print("Temperature for the device 1 (index 0) is: ");
      //Serial.println(tempC);
      if (sensFail == 1) { // SENSOR JUST RECOVERED FROM FAIL: recover last screen mode if sensoor ok, clear flag
        digitalWrite(13, 0);
        wakeScreen();
        if(!returnToManual){
        screenMode = screenLast;
        setMode = 0;
          isConstPower = 0;
        }
        else{
          screenMode=UICONST_SCREEN;
          setMode=1;
          isConstPower=1;
        }
        sensFail = 0;
        lastActionTime = millis();
        if (screenMode != UICONST_SCREEN) { // back from constPower if was not in UICONST_SCREEN
        }
      }
      if (tempAdj) tempC += float(tempAdj) / 10.0;
      //Serial.println(tempC);
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
        if(isConstPower)returnToManual=1;
        else returnToManual=0;
        if(screenMode) screenLast = screenMode; // remember last screen
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
  analogWrite(PWM_OUT, dimVal);
}
