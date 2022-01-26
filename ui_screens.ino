void UIsleepRoutine() { // routine for sleep mode
  sleepScreen();
  if (switchAction) { // main key is pressed when sleep - wake up into last screen
    screenMode = screenLast;
    switchAction = 0; //reset flag after action
  }
  if (encoderAction) { // encoder is turned when sleep - wake up into last screen
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    screenMode = screenLast;
  }
}

void UImain() { // main screen
  if (encoderAction && setMode) { // adjust goal temp
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    tempGoal = scrollInt(tempGoal, encIncrement * 5, -250, 1200, 1); // roll goal temp by 0.5*c
        if(!(tempGoal%5)){ //check if tempgoal is dividable by 5
          tempGoal=(tempGoal/10)*10;
        }
  }
  display.clearDisplay();
  //  if(!isConstPower) { // if PID is enabled instead of ConstantPower
  // display power
  display.setCursor(1, 1);
  display.setTextSize(1);
  //display.print("P");
  display.write(char(ICONoutput));
  display.setTextSize(2);
  display.print(map(PID_output, 0, 255, 0, 100));
  display.print(F("%"));
  //display online temperature
  display.setCursor(1, 17);
  display.setTextSize(1);
  //display.print("t");
  display.write(char(ICONtempRead));
  printTemp(tempIntC, 1);
  display.setCursor(65, 17);
  //display.print("g");
  display.write(char(ICONtempGoal));
  printTemp(tempGoal, 1);
  if (setMode) {
    /*//PID data
    display.setTextSize(0);
    display.setCursor(50, 5);
    //display.print(F("PID"));
    display.print(PID_p * -1);
    display.print(F(" "));
    display.print(PID_i * -1);
    display.print(F(" "));
    display.print(PID_d * -1);*/
    display.fillRect(SCREEN_WIDTH - map(truncInt(tempGoal, 0, 600), 0, 600, 0, 127), (SCREEN_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  } else // not setMode
  {
    display.fillRect(0, (SCREEN_HEIGHT / 2), map(truncInt(tempIntC, 0, 600), 0, 600, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.fillRect(0, 0, map(PID_output, 0, 255, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  //  } //if(!isConstPower)
  /*  else { // if Constant power mode
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print(F("PWR:  "));
      display.print(constPower);
      display.print(F(" %"));
      display.setCursor(0, 16);
      display.print(F("TMP: "));
      printTemp(tempIntC,1);
      //display.print(tempIntC / 10); display.setTextSize(1); display.print(F(".")); display.print(tempIntC - (int(tempIntC / 10) * 10));  display.print((char)247);
      //draw barlines
      display.fillRect(0, 0, map(constPower, 255, 0, 0, 127), display.height() / 2, SSD1306_INVERSE);
      display.fillRect(0, (display.height() / 2), map(tempIntC, 0, 600, 0, 127), display.height() / 2, SSD1306_INVERSE);
    }*/
  display.display(); // Update screen with each newly-drawn rectangle
}

void UIpid() { // PIDx-parameter set screen
  if (encoderAction && setMode) { // adjust kpid
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    if (kpid < 200) {
      kpid = scrollInt(kpid, encIncrement, 1, 10000, 0);
    }
    if (kpid >= 200 && kpid < 1000) {
      kpid = scrollInt(kpid, encIncrement * 10, 1, 10000, 0);
      kpid = int(kpid / 10) * 10;
    }
    if (kpid >= 1000) {
      kpid = scrollInt(kpid, encIncrement * 100, 1, 10000, 0);
      kpid = int(kpid / 100) * 100;
    }
  }
  int hundrKpid = kpid / 100;
  int centiKpid = kpid - int(hundrKpid * 100);
  display.clearDisplay();
  { // header math
    display.setCursor(0, 0);
    display.setTextSize(1);
    //display.print(F(""));
    display.print(PID_p * -1);
    if (PID_i <= 0)display.print(F("+"));
    display.print(PID_i * -1);
    if (PID_d <= 0)display.print(F("+"));
    display.print(PID_d * -1);
    display.print(F(" = ")); display.println((PID_d + PID_i + PID_d) * -1);
    display.print(F(" x ")); display.print(int(hundrKpid)); display.print(F("."));
    if (centiKpid < 10) display.print(F("0"));
    display.print(kpid - (hundrKpid * 100));
    display.print(F("= ")); display.print(int(PID_value)); display.print(F(" | ")); display.print(int(PID_output));
  }
  { // value to change
    byte xPos = 24;
    if (kpid > 999)xPos = 12;
    if (kpid > 9999)xPos = 0;
    display.setCursor(xPos, 17);
    display.setTextSize(2);
    display.print(F("x ")); display.print(hundrKpid); display.print(F("."));
    if (centiKpid < 10) display.print(F("0"));
    display.print(kpid - (hundrKpid * 100));
  }
  if (setMode) { // progressbar
    if (kpid == 100) {
      display.fillRect(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
    }
    if (kpid > 100 && kpid <= 1000)  {
      display.fillRect(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        map(kpid,
            100,
            1000,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
    }
    if (kpid > 1000)  {
      display.fillRect(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        map(kpid,
            1000,
            10000,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
    }
    if (kpid < 100 && kpid >= 10) {
      display.fillRect(
        (SCREEN_WIDTH / 2) - map(kpid,
                                 100,
                                 10,
                                 0,
                                 SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        map(kpid,
            100,
            10,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
    }
    if (kpid < 10) {
      display.fillRect(
        (SCREEN_WIDTH / 2) - map(kpid,
                                 10,
                                 1,
                                 0,
                                 SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        map(kpid,
            10,
            1,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
    }
  }
  /*  if (setMode) {
      if (kpid <= 100) display.fillRect(0, SCREEN_HEIGHT / 2 , map(kpid, 0, 100, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      if (kpid > 100) display.fillRect(SCREEN_WIDTH - map(kpid, 100, 10000, 0, 127), SCREEN_HEIGHT / 2 , map(kpid, 100, 10000, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
    }*/
  display.display();
}

void UIp() { // P-parameter set screen
  if (encoderAction && setMode) { // adjust kp
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    kp = scrollInt(kp, encIncrement, 0, 999, 1); // roll kp by 1 within 0...999
  }
  display.clearDisplay();
  UIpidBar(); // display pid bar to the left
  // P display:
  display.setTextSize(2);
  display.setCursor(58, 0); display.print(kp); //x50
  display.setTextSize(1);
  display.setCursor(50, 3); display.print(F("P")); //x90
  display.setCursor(98, 3); display.print(PID_p * -1);
  // I display
  display.setCursor(68, 16); display.print(ki); //x60
  display.setCursor(50, 16); display.print(F("I")); //x90
  display.setCursor(98, 16); display.print(PID_i * -1);
  // D display
  display.setCursor(68, 24); display.print(kd); //x60
  display.setCursor(50, 24); display.print(F("D")); //x90
  display.setCursor(98, 24); display.print(PID_d * -1);

  display.display();
}

void UIi() {
  if (encoderAction && setMode) { // adjust ki
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    ki = scrollInt(ki, encIncrement, 0, 999, 1); // roll ki by 1 within 0...999
  }
  display.clearDisplay();
  UIpidBar(); // display pid bar to the left
  // P display:
  display.setCursor(68, 0); display.print(kp);
  display.setCursor(50, 0); display.print(F("P"));
  display.setCursor(98, 0); display.print(PID_p * -1);
  // I display
  display.setTextSize(2);
  display.setCursor(58, 8); display.print(ki);
  display.setTextSize(1);
  display.setCursor(50, 12); display.print(F("I"));
  display.setCursor(98, 12); display.print(PID_i * -1);
  // D display
  display.setCursor(68, 24); display.print(kd);
  display.setCursor(50, 24); display.print(F("D"));
  display.setCursor(98, 24); display.print(PID_d * -1);
  display.display();
}

void UId() {
  if (encoderAction && setMode) { // adjust kd
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    kd = scrollInt(kd, encIncrement, 0, 999, 1); // roll kd by 1 within 0...999
  }
  display.clearDisplay();
  UIpidBar(); // display pid bar to the left
  // P display:
  display.setCursor(68, 0); display.print(kp);
  display.setCursor(50, 0); display.print(F("P"));
  display.setCursor(98, 0); display.print(PID_p * -1);
  // I display
  display.setCursor(68, 8); display.print(ki);
  display.setCursor(50, 8); display.print(F("I"));
  display.setCursor(98, 8);  display.print(PID_i * -1);
  // D display
  display.setTextSize(2);
  display.setCursor(58, 16); display.print(kd);
  display.setTextSize(1);
  display.setCursor(50, 20); display.print(F("D"));
  display.setCursor(98, 20); display.print(PID_d * -1);
  display.display();
}

void UIpowerMin() { // set min power
  if (encoderAction && setMode) { // adjust min power
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    minPower = scrollInt(minPower, encIncrement, 0, maxPower, 0); // roll minPower by 1 within 0...254
  }
  display.clearDisplay();
  UIpowerHead();
  display.setTextSize(2);
  display.setCursor(1, 17);
  display.print(minPower);
  display.setTextSize(1);
  display.setCursor(95, 21);
  display.print(maxPower);
  if (setMode) { // head barline for minPower
    display.fillRect(0, SCREEN_HEIGHT / 2, minPower / 2, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

void UIpowerMax() { // set max power
  if (encoderAction && setMode) { // adjust max power
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    maxPower = scrollInt(maxPower, encIncrement, minPower, 255, 0); // roll maxPower by 1 within 0...254
  }
  display.clearDisplay();
  UIpowerHead();
  display.setTextSize(1);
  display.setCursor(5, 21);
  display.print(minPower);
  display.setTextSize(2);
  display.setCursor(90, 17);
  display.print(maxPower);
  if (setMode) { // head barline for maxPower
    display.fillRect((maxPower / 2), SCREEN_HEIGHT / 2, SCREEN_WIDTH - (maxPower / 2), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

#define FOUR_CHAR_STRING_LEN 50
void UIsetLoad() { // load settings screen
  if (encoderAction && setMode) { // chose menu item
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    setMode = scrollInt(setMode, encIncrement, 1, 2, 1); // roll menu
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(17, 0);
  display.print(F("SETTINGS"));
  /*if (setMode) { // inverse the head
    display.fillRect(0, 0, display.width(), display.height() / 2, SSD1306_INVERSE);
    }*/
  display.setCursor(2, 17);
  display.print(F("BACK"));
  display.setCursor(80, 17);
  display.print(F("LOAD"));
  switch (setMode) { // highlight selected
    case 1: // BACK
      display.fillRect(0, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    case 2: // LOAD
      display.fillRect(SCREEN_WIDTH - FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    default: // HEAD
      display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
  }
  display.display();
}

void UIsetSave() { // save settings screen
  if (encoderAction && setMode) { // chose menu item
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    setMode = scrollInt(setMode, encIncrement, 1, 2, 1); // roll menu
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(17, 0);
  display.print(F("AUTOSAVE"));
  /*if (setMode) { // inverse the head
    display.fillRect(0, 0, display.width(), display.height() / 2, SSD1306_INVERSE);
    }*/
  display.setCursor(2, 17);
  display.print(F("NOPE"));
  display.setCursor(80, 17);
  display.print(F("SAVE"));
  switch (setMode) { // highlight selected
    case 1: // NOPE
      display.fillRect(0, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    case 2: // SAVE
      display.fillRect(SCREEN_WIDTH - FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    default: // HEAD
      display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
  }
  display.display();
}

void UIstoreScreen() { // store screen mode when goto sleep?
  if (encoderAction && setMode) { // chose menu item
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    setMode = scrollInt(setMode, encIncrement, 1, 2, 1); // roll menu
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 1);
  display.print(F("MEM SCREEN"));
  /*if (setMode) { // inverse the head
    display.fillRect(0, 0, display.width(), display.height() / 2, SSD1306_INVERSE);
    }*/
  display.setCursor(7, 17);
  display.print(F("OFF"));
  display.setCursor(91, 17);
  display.print(F("ON"));
  switch (setMode) { // highlight selected
    case 1: // BACK
      display.fillRect(0, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    case 2: // LOAD
      display.fillRect(SCREEN_WIDTH - FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, FOUR_CHAR_STRING_LEN, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
    default: // HEAD
      display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
      break;
  }
  display.display();
}

void UIconstMode() { // constant speed set
  if (encoderAction && setMode) { // increment constPower value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    constPower = scrollInt(constPower, encIncrement, 0, 255, 0); // increment constPower
  }
  if (setMode && !isConstPower) {
    isConstPower = 1;
  }
  if (!setMode && isConstPower) {
    isConstPower = 0;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(17, 1);
  //HEADER:
  if (sensFail) {
    display.print(F("SENSFAIL"));
  }
  else {
    display.print(F("CONSTANT"));
  }
  display.setCursor(80, 20);
  display.setTextSize(1);
  if (!sensFail) {
    display.write(char(ICONtempRead));
    printTemp(tempIntC, 0);
  }
  display.setCursor(1, 17);
  display.write(char(ICONoutput));
  display.setTextSize(2);
  display.print(constPower);
  //display.print(tempIntC / 10); display.setTextSize(1); display.print(F(".")); display.print(tempIntC - (int(tempIntC / 10) * 10));  display.print((char)247);
  if (setMode) { // head barline
    display.fillRect(0, SCREEN_HEIGHT / 2, constPower / 2, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  else {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

void UItimeout() { // set screen timeout
  if (encoderAction  && setMode) { // increment constPower value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    screenTimeout = scrollInt(screenTimeout, encIncrement, 5, 600, 0); // increment screenTimeout
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(22, 1);
  display.print(F("TIMEOUT"));
  display.setCursor(80, 17);
  display.print(screenTimeout);
  display.setTextSize(1);
  display.print(F("s"));
  if (setMode) { // head barline
    display.fillRect(0, SCREEN_HEIGHT / 2, map(truncInt(screenTimeout, 0, 120), 0, 120, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  else {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

void UItAdj() { // set tempAdj (shifts sensor value)
  if (encoderAction  && setMode) { // increment tempAdj value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    tempAdj = scrollInt(tempAdj, encIncrement, -90, +90, 0); // increment by 0.1degree
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(1, 1);
  display.print(F("TEMP"));
  display.setCursor(65, 1);
  display.print(F("ADJ."));
  display.setCursor(3, 20);
  display.setTextSize(1);
  display.write(char(ICONtempRead));
  printTemp(tempIntC, 0);
  display.setCursor(53, 17);
  display.setTextSize(2);
  if (tempAdj > 0)display.print(F("+"));
  //if (tempAdj < 0)display.print(F("-"));
  if (tempAdj == 0)display.setCursor(65, 17);
  printTemp(tempAdj, 1);
  if (setMode) { // progressbar
    if (tempAdj == 0) {
      display.fillRect(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
    }
    if (tempAdj > 0)  {
      display.fillRect(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        map(tempAdj,
            0,
            +90,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
    }
    if (tempAdj < 0) {
      display.fillRect(
        (SCREEN_WIDTH / 2) - map(tempAdj,
                                 0,
                                 -90,
                                 0,
                                 SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        map(tempAdj,
            0,
            -90,
            0,
            SCREEN_WIDTH / 2),
        SCREEN_HEIGHT / 2,
        SSD1306_INVERSE);
      //display.fillRect(SCREEN_WIDTH - map(tempAdj,0,-90, 0,128),SCREEN_HEIGHT / 2,map(tempAdj,0,-90,0,128), SCREEN_HEIGHT / 2,SSD1306_INVERSE);
    }
  }
  else { // head bar
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

void UIscreenRotate() {
  if (encoderAction  && setMode) { // increment tempAdj value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    setMode = scrollInt(setMode, encIncrement, 1, 2, 1); // increment displayRotate
    if (setMode == 1) {
      //displayRotate
      display.setRotation(0);
    }
    if (setMode == 2)
    {
      display.setRotation(2);
    }
  }
  display.clearDisplay();
  if (!setMode) {
    display.setTextSize(2);
    display.setCursor(13, 1);
    display.print(F("ROTATE UI"));
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  else {
    display.setTextSize(0);
    display.setCursor(38, 0);
    display.print(F("ROTATE UI"));
    display.setTextSize(3);
    if (setMode == 1) {
      display.setCursor(12, 8);
      display.print(F("<LEFT"));
    }
    if (setMode == 2) {
      display.setCursor(8, 8);
      display.print(F("RIGHT>"));
    }
  }
  display.display();
}

#define DEGREE_PER_PIXEL 1   // 

void UIplotTemp() {
  display.clearDisplay();
  if (encoderAction  && setMode) { // increment tempAdj value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    keepSample = scrollInt(keepSample, encIncrement, 1, 600, 0); // increment plotSampleDensity
    //display.fillRect(0, 0, 21, 8, SSD1306_BLACK);
  }
  int rangeMin = screenTempArray[arrayIndex]; // setting range to actual data point
  int rangeMax = rangeMin;
  if (setMode) { // for setMode presetup range to include tempGoal
    if (tempGoal < rangeMin) rangeMin = tempGoal;
    if (tempGoal > rangeMax) rangeMax = tempGoal;
  }
  for (int i = 0; i < 128; i++) { // expanding array range to include actual array range
    if (screenTempArray[i] < rangeMin) rangeMin = screenTempArray[i];
    if (screenTempArray[i] > rangeMax) rangeMax = screenTempArray[i];
  }
  if (rangeMax - rangeMin < (SCREEN_HEIGHT * DEGREE_PER_PIXEL)) { // if it is still overzoomed   // SCREEN_HEIGHT*10 - 1 degree scale; SCREEN_HEIGHT*5 - 0.5 degree scale
    if (!setMode) { // for regular mode center window with actual range center with maximum zoom
      int rangeMiddle = (rangeMax + rangeMin) / 2;
      //int rangeHeight = (rangeMax - rangeMin);
      rangeMin = rangeMiddle - (SCREEN_HEIGHT / 2) * DEGREE_PER_PIXEL;
      rangeMax = rangeMin + SCREEN_HEIGHT * DEGREE_PER_PIXEL;
    }
    else {
      rangeMax = rangeMin + (SCREEN_HEIGHT * DEGREE_PER_PIXEL); // add extra range to top (only thing to do in setMode)
    }
  }
  // OSD:
  // display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.write(char(ICONminRange)); //0x18 minRange
  printTemp(rangeMax, 0);
  display.setCursor(1, 24);
  display.write(char(ICONmaxRange));
  printTemp(rangeMin, 0);
  if (setMode) {
    display.setCursor(1, 9);
    display.print(F("s:"));
    display.print(int(keepSample));
    display.setCursor(1, 16);
    display.write(char(ICONtempRead));
    printTemp(tempIntC, 0);
  }
  else {
    display.setCursor(1, 9);
    display.write(char(ICONtempRead));
    display.setTextSize(2);
    printTemp(tempIntC, 1);
  }
  // PLOT GRAPH:
  byte plotXpos = 0;
  byte plotArrayPos = arrayIndex + 1;
  if (plotArrayPos > 127)plotArrayPos = 0;
  for (plotXpos = 0; plotXpos < 128; plotXpos++) { // plot graph
    if (plotArrayPos > 127)plotArrayPos = 0;
    display.fillRect(plotXpos, map(screenTempArray[plotArrayPos], rangeMin, rangeMax, SCREEN_HEIGHT - 1, 1), 1, SCREEN_HEIGHT, SSD1306_INVERSE);
    plotArrayPos++;
  }
  if (tempGoal >= rangeMin && tempGoal <= rangeMax) { // plot tempGoal line
    byte setPlane = map(tempGoal, rangeMin, rangeMax, SCREEN_HEIGHT - 1, 0);
    for (byte i = 1; i < 127; i += 5) {
      display.drawPixel(i, setPlane, SSD1306_INVERSE);
    }
  }
  display.display();
}

void UIplotPid() {
  if (encoderAction  && setMode) { // increment tempAdj value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    keepSample = scrollInt(keepSample, encIncrement, 1, 600, 0); // increment plotSampleDensity
    //display.fillRect(0, 0, 21, 8, SSD1306_BLACK);
  }
  int rangeMin = 0;
  int rangeMax = 0;
  { // AUTO RANGE ARRAY if not PID_output, which is 0...255 always
    rangeMin = screenPidArray[arrayIndex];
    rangeMax = rangeMin;
    for (int i = 0; i < 128; i++) { // define margins of array
      if (screenPidArray[i] < rangeMin) rangeMin = screenPidArray[i];
      if (screenPidArray[i] > rangeMax) rangeMax = screenPidArray[i];
    }
    if (setMode) { // set array range to min-max power range
      if (rangeMin > minPower) rangeMin = minPower;
      if (rangeMax < maxPower) rangeMax = maxPower;
    }
    if (rangeMax < rangeMin + SCREEN_HEIGHT) { // maximum vertical detail
      if (!setMode) {
        int rangeMiddle = (rangeMax + rangeMin) / 2;
        //int rangeHeight = (rangeMax - rangeMin);
        rangeMin = rangeMiddle - SCREEN_HEIGHT / 2;
        if (rangeMin < 0) rangeMin = 0;
      }
      rangeMax = rangeMin + (SCREEN_HEIGHT);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.write(char(ICONminRange));
  display.print(rangeMax);
  display.setCursor(1, 24);
  display.write(char(ICONmaxRange));
  display.print(rangeMin);
  if (setMode) {
    display.setCursor(1, 8);
    display.print(F("s:"));
    display.print(int(keepSample));
    display.setCursor(1, 16);
    display.write(char(ICONoutput));
    display.print(PID_output);
  }
  else {
    display.setCursor(1, 9);
    display.write(char(ICONoutput));
    display.setTextSize(2);
    display.print(PID_output);
    //display.print(F("%"));
  }
  byte minValY = map(minPower, rangeMin, rangeMax, SCREEN_HEIGHT - 1, 0);
  byte maxValY = map(maxPower, rangeMin, rangeMax, SCREEN_HEIGHT - 1, 0);
  byte plotXpos = 0;
  byte plotArrayPos = arrayIndex + 1;
  if (plotArrayPos > 127)plotArrayPos = 0;
  for (plotXpos; plotXpos < 128; plotXpos++) { // plot graph
    if (plotArrayPos > 127)plotArrayPos = 0;
    display.fillRect(plotXpos, map(screenPidArray[plotArrayPos], rangeMin, rangeMax, SCREEN_HEIGHT - 1, 0), 1, SCREEN_HEIGHT, SSD1306_INVERSE);
    plotArrayPos++;
  }
  for (byte i = 1; i < 127; i += 5) { // draw horizontal lines of minPower and maxPower (start from x=1)
    if (minValY >= 0 && minValY < SCREEN_HEIGHT - 1)display.drawPixel(i, minValY, SSD1306_INVERSE);
    if (maxValY >= 0 && maxValY < SCREEN_HEIGHT - 1)display.drawPixel(i, maxValY, SSD1306_INVERSE);
  }
  display.display();
}

void UIhysteresisTime() {
  if (encoderAction  && setMode) { // increment value
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    hysteresisTimeS = scrollInt(hysteresisTimeS, encIncrement, 0, 3600, 1); // increment by 1sec
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(4, 1);
  display.print(F("HYSTERESIS"));
  display.setCursor(70, 17);
  display.print(hysteresisTimeS);
  display.setTextSize(1);
  display.print(F("s"));
  if (setMode) { // head barline
    display.fillRect(0, SCREEN_HEIGHT / 2, truncInt(hysteresisTimeS, 0, 127), SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  else {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_INVERSE);
  }
  display.display();
}

void splashScreen() {
  display.clearDisplay(); // Clear the buffer
  display.setTextWrap(0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(62, 0);            // Start at top-left corner
  display.println(F("0...10v PWM"));
  display.setTextSize(2);
  display.setCursor(5, 8);
  display.print(F("EC FAN PID"));
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println(REV_NO);
  display.display(); // Show display buffer contents on the screen
  delay(300);
  display.fillRect(0, 7, 128, 17, SSD1306_INVERSE);
  delay(1000);
}

void MSGwrongMode() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(1, 1);
  display.write(char(ICONerror));
  display.setTextSize(2);
  display.setCursor(43, 1);
  display.println(F("WRONG"));
  display.setCursor(50, 16);
  display.println(F("MODE"));
  display.display();
}
void MSGsensorFail() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(1, 1);
  display.write(char(ICONerror));
  display.setTextSize(2);
  display.setCursor(38, 1);
  display.println(F("SENSOR"));
  display.setCursor(50, 16);
  display.println(F("FAIL"));
  display.display();
}
