void settingsPut() { // store current settings to EEPROM
  EEPROM.put(ADDR_tempGoal, int(tempGoal));
  EEPROM.put(ADDR_kp, kp);
  EEPROM.put(ADDR_ki, ki);
  EEPROM.put(ADDR_kd, kd);
  EEPROM.put(ADDR_kpid, int(kpid)); //*
  EEPROM.put(ADDR_minPower, minPower);
  EEPROM.put(ADDR_maxPower, maxPower);
  EEPROM.put(ADDR_isConstPower, byte(isConstPower));
  EEPROM.put(ADDR_constPower, constPower);
  EEPROM.put(ADDR_tempAdj, int(tempAdj)); //*
  EEPROM.put(ADDR_timeOut, screenTimeout);
  EEPROM.put(ADDR_storeScreen, storeScreen);
  EEPROM.put(ADDR_displayRotate, displayRotate);
  EEPROM.put(ADDR_isAutoSave, isAutoSave);
  EEPROM.put(ADDR_keepSample, keepSample);
  EEPROM.put(ADDR_HYSTtime, HYSTtime);  
  EEPROM.put(ADDR_DESTtime, DESTtime);
  EEPROM.put(ADDR_DESTperiod, DESTperiod);
  //EEPROM.put(ADDR_screenMode, screenMode);
}

void settingsGet() { // restore settings from EEPROM
  EEPROM.get(ADDR_tempGoal, tempGoal);
  EEPROM.get(ADDR_kp, kp);
  EEPROM.get(ADDR_ki, ki);
  EEPROM.get(ADDR_kd, kd);
  EEPROM.get(ADDR_kpid, kpid);
  EEPROM.get(ADDR_minPower, minPower);
  EEPROM.get(ADDR_maxPower, maxPower);
  byte TEMPisConstPower=isConstPower;
  EEPROM.get(ADDR_isConstPower, TEMPisConstPower);
  EEPROM.get(ADDR_constPower, constPower);
  EEPROM.get(ADDR_tempAdj, tempAdj);
  EEPROM.get(ADDR_timeOut, screenTimeout);
  EEPROM.get(ADDR_storeScreen, storeScreen);
  EEPROM.get(ADDR_displayRotate, displayRotate);
  EEPROM.get(ADDR_isAutoSave, isAutoSave);
  EEPROM.get(ADDR_keepSample, keepSample);
  EEPROM.get(ADDR_screenMode, screenMode);
  EEPROM.get(ADDR_HYSTtime, HYSTtime);
  EEPROM.get(ADDR_DESTtime, DESTtime);
  EEPROM.get(ADDR_DESTperiod, DESTperiod);
}

byte EEstoreScreenGet() { //get setting, so we can show saved value first
  byte TEMPstoreScreen=0;
  EEPROM.get(ADDR_storeScreen, TEMPstoreScreen);
  return (TEMPstoreScreen);
}
byte EEisAutoSaveGet() { //get setting, so we can show saved value first
  byte TEMPisAutoSave=0;
  EEPROM.get(ADDR_isAutoSave, TEMPisAutoSave);
  return (TEMPisAutoSave);
}

void EEscreenModePut() { // update screenMode to EEprom only if different from actual
  if(!sensFail) EEPROM.put(ADDR_screenMode, screenMode); // store only if permit
}
byte EEscreenModeGet() {
  byte tScreenMode = 0;
  EEPROM.get(ADDR_screenMode, tScreenMode);
  return (tScreenMode);
}
