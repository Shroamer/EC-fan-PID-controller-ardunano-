void printTemp(int tempDecaDegree, bool bigSize) { // prints tempDecaDegree in "time format"
  int tempDegree = tempDecaDegree / 10;
  if (bigSize) display.setTextSize(2);
  else display.setTextSize(1);
  if (tempDecaDegree < 0 && tempDecaDegree > -10)display.print(F("-"));
  display.print(tempDegree);
  if (bigSize) display.setTextSize(1);
  display.print(F("."));
  display.print(abs(tempDecaDegree - (tempDegree * 10)));
  display.print((char)247);
}

void UIpidBar() { // PID info bar to the left of the screen. helps configure PID
  // display temperatures:
  display.setTextSize(1);
  display.setCursor(0, 0);
  //display.println(F("T ")); display.println(F("G ")); display.println(F("E ")); display.print(F("P "));
  display.println(char(ICONtempRead)); display.println(char(ICONtempGoal)); display.println(char(ICONPIDerror)); display.print(char(ICONoutput));
  display.setCursor(9, 0);
  printTemp(tempIntC, 0);
  display.setCursor(9, 7);
  printTemp(tempGoal, 0);
  display.setCursor(9, 15);
  printTemp(PID_err * -1, 0);
  display.setCursor(9, 23);
  display.print(map(PID_output, 0, 255, 0, 100)); display.println(F("%"));

  byte coordsList = screenMode - UIP_SCREEN;
  if (setMode) {
    /*switch (screenMode) { //locate marker
      case UIP_SCREEN:
        display.setCursor(44, 3);
        break;
      case UII_SCREEN:
        display.setCursor(44, 12);
        break;
      case UID_SCREEN:
        display.setCursor(44, 20);
        break;
      default:
        break;
      }
    */
    display.setCursor(coordsPIDarrow[coordsList][0], coordsPIDarrow[coordsList][0]);
    display.setTextSize(1);
    display.print((char)ICONselect);
    display.fillRect(0, 0, 44, SCREEN_HEIGHT, SSD1306_INVERSE);
  }
  // P display:
  if (screenMode == UIP_SCREEN) display.setTextSize(2); else display.setTextSize(1);
  display.setCursor(coordsP[coordsList][0][0], coordsP[coordsList][0][1]); display.print(kp); //x50
  if (screenMode == UIP_SCREEN) display.setTextSize(1);
  display.setCursor(coordsP[coordsList][1][0], coordsP[coordsList][1][1]); display.print(F("P")); //x90
  display.setCursor(coordsP[coordsList][2][0], coordsP[coordsList][2][1]); display.print(PID_p * -1);
  // I display
  if (screenMode == UII_SCREEN) display.setTextSize(2);
  display.setCursor(coordsI[coordsList][0][0], coordsI[coordsList][0][1]); display.print(ki); //x60
  if (screenMode == UII_SCREEN) display.setTextSize(1);
  display.setCursor(coordsI[coordsList][1][0], coordsI[coordsList][1][1]); display.print(F("I")); //x90
  display.setCursor(coordsI[coordsList][2][0], coordsI[coordsList][2][1]); display.print(PID_i * -1);
  // D display
  if (screenMode == UID_SCREEN) display.setTextSize(2);
  display.setCursor(coordsD[coordsList][0][0], coordsD[coordsList][0][1]); display.print(kd); //x60
  if (screenMode == UID_SCREEN) display.setTextSize(1);
  display.setCursor(coordsD[coordsList][1][0], coordsD[coordsList][1][1]); display.print(F("D")); //x90
  display.setCursor(coordsD[coordsList][2][0], coordsD[coordsList][2][1]); display.print(PID_d * -1);
  /*
    switch (screenMode) { // drawing 3 lines of PID data:
      case UIP_SCREEN:

        break;
      case UII_SCREEN:

        break;
      case UID_SCREEN:

        break;
      default:
        break;
    }*/
}

void UIpowerHead() { // header for power settings screens
  display.setTextSize(2);
  display.setCursor(5, 1);
  display.print(F("MIN .. MAX"));
  display.setTextSize(1);
  display.setCursor(56, 21);
  display.write(char(ICONoutput));
  display.print(int(outVal));
  if (!setMode) display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);

  byte coordsList = screenMode - UIPOWERMIN_SCREEN;
  if (screenMode == UIPOWERMIN_SCREEN) display.setTextSize(2); else display.setTextSize(1);
  display.setCursor(coordsMin[coordsList][0], coordsMin[coordsList][1]);
  display.print(minPower);
  if (screenMode == UIPOWERMAX_SCREEN) display.setTextSize(2); else display.setTextSize(1);
  display.setCursor(coordsMax[coordsList][0], coordsMax[coordsList][1]);
  display.print(maxPower);
}

void UIdestagHead() {
  display.setTextSize(2);
  display.setCursor(1, 1);
  display.print(F("DEST "));
  display.setTextSize(1);
  if (screenMode == UIDESTAGPERIOD_SCREEN) display.print(F("period"));
  else display.print(F("time"));
  
  if (!setMode) display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);

  display.setTextSize(2);
  display.setCursor(1, 17);
  if (screenMode == UIDESTAGPERIOD_SCREEN) {
    display.print(DESTperiod);
    display.print(F("m"));
  }
  else { //UIDESTAGTIME_SCREEN
    display.print(DESTtime);
    display.print(F("s"));
  }
  if (DESTperiod && DESTtime) {
    display.setTextSize(1);
    display.setCursor(72, 21);
    unsigned long timeNow = millis();
    if (screenMode == UIDESTAGPERIOD_SCREEN) display.print(long(timeNow - (DESTcycleStart + (DESTperiod * DEST_PERIOD_MULT)) ) / 1000);
    else display.print(long(timeNow - (DESTcycleStart + (DESTperiod * DEST_PERIOD_MULT) + (DESTtime * DEST_TIME_MULT)) ) / 1000);
  }
}

void screenTempArrayInit() { // init screenTempArray
  for (arrayIndex = 0; arrayIndex < 128; arrayIndex++) {
    screenTempArray[arrayIndex] = tempIntC;
  }
  arrayIndex = 0;
}

void screenPidArrayInit() { // init screenTempArray
  for (arrayIndex = 0; arrayIndex < 128; arrayIndex++) {
    screenPidArray[arrayIndex] = 0; // value shifted by 100, so 0 degree is 100.
  }
  arrayIndex = 0;
}

void takePlotSample() { //   UI PLOT SCREEN SAMPLING:
  keepSampleIndex++; //increment keep smaple index
  if (keepSampleIndex >= keepSample) { //take a sample if should
    if (arrayIndex < 127) arrayIndex++; //increment array index
    else arrayIndex = 0; //reset array index
    int passTemp; // temporary storing value to pass tp plotter
    int passPid; // temporary storing value to pass tp plotter
    //if(!sensFail)
    tempApproxSum += tempIntC; // adding last sens data
    //else tempApproxSum+=1250;
    passTemp = tempApproxSum / keepSampleIndex; // dividing sum of sample temperatures by sample count to pass an average
    tempApproxSum = 0; // resetting sum vriable for the next sample collection
    if (isConstPower) powerApproxSum += constPower; // last addition and divide to get approx power
    else powerApproxSum += outVal;
    passPid = powerApproxSum / keepSampleIndex;
    powerApproxSum = 0;
    screenTempArray[arrayIndex] = passTemp; //ds18b20 can read -55...+125^C
    screenPidArray[arrayIndex] = passPid; //PID_value or PID_output
    keepSampleIndex = 0;
  }
  else { // just summing data for approximization
    tempApproxSum += tempIntC;
    if (isConstPower) powerApproxSum += constPower;
    else powerApproxSum += outVal;
  }
}
