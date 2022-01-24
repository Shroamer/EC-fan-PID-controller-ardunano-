void drawUi() { // top-level UI routine
  if (screenMode) { // wake screen if not should be turned off (screenMode!=0)
    wakeScreen();
  }
  if (switchAction && !setMode) { // ***** PUT PRELOAD SET DATA HERE ***** SWITCH is pressed in select mode - enter setMode
    setMode = 1;
    switch (screenMode) {
      case UISCREENROTATE_SCREEN: {
          if (displayRotate) {
            setMode = 2;
          }
          break;
        }
      case UISETSAVE_SCREEN: {
          if (EEisAutoSaveGet()) {
            setMode = 2;
          }
          break;
        }
      case UISTORESCREEN_SCREEN: {
          if (EEstoreScreenGet()) {
            setMode = 2;
          }
          break;
        }
      default: {
          break;
        }
    }
    switchAction = 0;
  }
  if (switchAction && setMode) { // APPLY SET VALUE (press SWITCH in SET mode)
    switch (screenMode) { // ***** DEFINE WHAT TO APPLY AFTER EXITING SETMODE HERE *****
      case UIMAIN_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_tempGoal, tempGoal);
          break;
        }
      case UIPID_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_kpid, int(kpid));
          break;
        }
      case UIP_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_kp, kp);
          break;
        }
      case UII_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_ki, ki);
          break;
        }
      case UID_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_kd, kd);
          break;
        }
      case UIPOWERMIN_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_minPower, minPower);
          break;
        }
      case UIPOWERMAX_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_maxPower, maxPower);
          break;
        }
      case UITADJ_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_tempAdj, int(tempAdj));
          break;
        }
      case UITIMEOUT_SCREEN: {
          if (isAutoSave) EEPROM.put(ADDR_timeOut, screenTimeout);
          break;
        }
      case UISTORESCREEN_SCREEN: {     // save screenMode
          if (setMode == 1) { // save  screen to eeprom
            storeScreen = 0; // let's save screen each sleep
            EEPROM.put(ADDR_storeScreen, storeScreen); // remember it
          }
          if (setMode == 2) { // save  screen to eeprom
            storeScreen = 1; // let's skip screen save at sleep
            EEPROM.put(ADDR_storeScreen, storeScreen); // remember it
          }
          break;
        }
      case UISCREENROTATE_SCREEN: {     // rotate screen
          if (setMode == 1) { // save  screen to eeprom
            displayRotate = 0; // let's save screen each sleep
            EEPROM.put(ADDR_displayRotate, displayRotate);
          }
          if (setMode == 2) { // save  screen to eeprom
            displayRotate = 1; // let's skip screen save at sleep
            EEPROM.put(ADDR_displayRotate, displayRotate);
          }
          break;
        }
      case UISETSAVE_SCREEN: {     // eeprom save settings
          if (setMode == 2) { // store settings to EEPROM
            isAutoSave = 1;
            settingsPut();
            //screenMode = screenLast; // go home
          }
          if (setMode == 1) { // don't store settings
            isAutoSave = 0;
            EEPROM.put(ADDR_isAutoSave, isAutoSave);
          }
          break;
        }
      case UISETLOAD_SCREEN: {        // eeprom load settings (1-none, 2-save)
          if (setMode == 2) { // restore settings from EEPROM
            settingsGet();
            //screenMode = screenLast; // go home
          }
          break;
        }
      case UIPLOTTEMP_SCREEN: {
          //if (isAutoSave) EEPROM.put(ADDR_keepSample, keepSample);
          break;
        }
      case UIPLOTPID_SCREEN: {
          //if (isAutoSave) EEPROM.put(ADDR_keepSample, keepSample);
          break;
        }
      case UICONST_SCREEN: {
          if (isAutoSave || displayMissing) {
            EEPROM.put(ADDR_isConstPower, byte(isConstPower));
            EEPROM.put(ADDR_constPower, constPower);
          }
          break;
        }
      default: //screen has no specific action to pass by exit, fault flag, just reset it
        break;
    }
    //setMode = 0; //clear the flag
    setMode = 0;
    switchAction = 0;
  }
  if (encoderAction && !setMode) { // ENCODER turned in select mode - roll screens
    encIncrement = nullEncoder(); // retrieve increment data and reset encoder state/flag
    if (screenMode) {
      screenMode = scrollInt(screenMode, -encIncrement, 1, UICONST_SCREEN, 1);
    }
    else {
      screenMode = screenLast;
      Serial.print("ENC WAKE DUI");
    }
  }
  switch (screenMode) { // ***** PUT NEW SCREEN HERE *****
    case 0: // sleep mode
      UIsleepRoutine();
      break;
    case UIMAIN_SCREEN: // main screen (set to go goal adjust)
      UImain();
      break;
    case UIPID_SCREEN: // adjust P
      UIpid();
      break;
    case UIP_SCREEN: // adjust P
      UIp();
      break;
    case UII_SCREEN: // adjust I
      UIi();
      break;
    case UID_SCREEN: // adjust D
      UId();
      break;
    case UIPOWERMIN_SCREEN: // adjust min power
      UIpowerMin();
      break;
    case UIPOWERMAX_SCREEN: // adjust max power
      UIpowerMax();
      break;
    case UITADJ_SCREEN: // adjust Tajd
      UItAdj();
      break;
    case UITIMEOUT_SCREEN: // timeout change
      UItimeout();
      break;
    case UISTORESCREEN_SCREEN: // store screenMode
      UIstoreScreen();
      break;
    case UISCREENROTATE_SCREEN: // rotate screen
      UIscreenRotate();
      break;
    case UISETSAVE_SCREEN: // EEPROM save
      UIsetSave();
      break;
    case UISETLOAD_SCREEN: // EEPROM load
      UIsetLoad();
      break;
    case UIPLOTTEMP_SCREEN: // EEPROM load
      UIplotTemp();
      break;
    case UIPLOTPID_SCREEN: // EEPROM load
      UIplotPid();
      break;
    case UICONST_SCREEN: // manual speed (8)
      UIconstMode();
      break;
    default:
      MSGwrongMode();
      delay(2500);
      screenMode = 1;
      break;
  }
  //switchAction = 0;
  //nullEncoder(); // reset encoder state/flag
}
