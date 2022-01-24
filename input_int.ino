void switchInt() { // ISR called each time state FALL on pin3
  unsigned long interrupt_time = millis();
  if ((millis() - lastActionTime) > 50UL) { // ignores interupts for 50milliseconds
    switchAction = 1;
  }
  lastActionTime = millis(); // reset screen timeout
}

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3.
ISR(PCINT1_vect) {
  static unsigned long encLastTime = 0;
  unsigned char result = rotary.process();
  if ((result == DIR_CW) || (result == DIR_CCW)) {
    unsigned long thisMoment = millis();
    unsigned long encTime = thisMoment - encLastTime;
    byte encFast = (ENC_MULT / encTime) + 1;
    int displayRotateMult = 1;
    if (!displayRotate) displayRotateMult = -1;
    //if (displayRotate) displayRotateMult = 1;
    if (result == DIR_CW) encoderIncrement += (encFast * displayRotateMult);
    if (result == DIR_CCW) encoderIncrement -= (encFast * displayRotateMult);
    encLastTime = thisMoment;
    //encoderCheck = 1; // flag there's an action from encoder to call encoderRoutine()
    encoderAction = 1;
    lastActionTime = thisMoment; // reset screen timeout
  }
}

int nullEncoder() { // reset encoder state/flag
  int tempIncrement=encoderIncrement;
  encoderIncrement = 0;
  encoderAction = 0;
  return(tempIncrement);
}
