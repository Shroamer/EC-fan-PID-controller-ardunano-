void printTemp(int tempDecaDegree, bool bigSize) { // prints tempDecaDegree in "time format"
  int tempDegree = tempDecaDegree / 10;
  if (bigSize) display.setTextSize(2);
  else display.setTextSize(1);
  if(tempDecaDegree<0&&tempDecaDegree>-10)display.print(F("-"));
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
  if (setMode) {
    switch (screenMode) { //locate marker
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
    display.setTextSize(1);
    //display.print(F(">"));
    display.print((char)ICONselect);
    display.fillRect(0, 0, 44, SCREEN_HEIGHT, SSD1306_INVERSE);
  }
}

void UIpowerHead() { // header for power settings screens
  display.setTextSize(2);
  display.setCursor(5, 1);
  display.print(F("MIN .. MAX"));
  display.setTextSize(1);
  display.setCursor(56, 21);
  display.write(char(ICONoutput));
  display.print(int(PID_output));
  if (!setMode) display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
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
    if (arrayIndex < 127) arrayIndex++; //increment index
    else arrayIndex = 0;
    int passTemp; // temporary storing value to pass tp plotter
    int passPid; // temporary storing value to pass tp plotter
    //if (keepSampleIndex > 1) { // we have more than 1 sample to handle
      // last addition and divide to get approx temp
      //if(!sensFail) 
      tempApproxSum += tempIntC;
      //else tempApproxSum+=1250;
      passTemp = tempApproxSum / keepSampleIndex;
      tempApproxSum=0;
      // last addition and divide to get approx power
      if (isConstPower) powerApproxSum += constPower;
      else powerApproxSum += PID_output;
      passPid = powerApproxSum / keepSampleIndex;
      powerApproxSum=0;
    /*}
    else { // there is only one sample we have to store
      //if(!sensFail) 
      passTemp = tempIntC; // temp sample addition
      //else passTemp=1250;
      if (isConstPower) passPid = constPower;
      else passPid = PID_output;
    }*/    
    screenTempArray[arrayIndex] = passTemp; //ds18b20 can read -55...+125^C
    //if (isConstPower) screenPidArray[arrayIndex] = constPower; //PID_value or PID_output
    //else 
    screenPidArray[arrayIndex] = passPid; //PID_value or PID_output
    keepSampleIndex = 0;
  }
  else {
    //if (keepSample > 1) {
      tempApproxSum += tempIntC;
      if (isConstPower) powerApproxSum += constPower;
      else powerApproxSum += PID_output;
    //}
  }
}
