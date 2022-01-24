void sleepScreen() { // routine to sleep screen if awake
  if (!displaySleep) {
    displaySleep = 1;
    display.ssd1306_command(SSD1306_DISPLAYOFF); // send display to sleep
  }
}

void wakeScreen() { // routine to wake screen if sleeping
  if (displaySleep) {
    displaySleep = 0;
    lastActionTime = millis(); // reset screen timeout
    display.ssd1306_command(SSD1306_DISPLAYON); // wake up display
  }
}
