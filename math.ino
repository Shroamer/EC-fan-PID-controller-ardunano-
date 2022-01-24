int truncInt(int truncVal, int minVal, int maxVal) { // truncate value within
  if (truncVal < minVal)truncVal = minVal;
  if (truncVal > maxVal)truncVal = maxVal;
  return (truncVal);
}

int scrollInt(int incVar, int incVal, int minVal, int maxVal, bool cycle) {
  int incTemp = incVar + incVal;
  if (cycle) {
    while (incTemp < minVal || incTemp > maxVal) {
      if (incTemp > maxVal) { //turn around right
        incTemp -= maxVal;
      }
      if (incTemp < minVal) {
        incTemp += maxVal;
      }
    }
  }
  if (!cycle) {
    if (incTemp > maxVal) { // trunc min
      incTemp = maxVal;
    }
    if (incTemp < minVal) { // trunc max
      incTemp = minVal;
    }
  }
  return (incTemp);
  incTemp = 0;
}
