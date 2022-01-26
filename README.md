# EC-fan-PID-controller-ardunano-
2.2.4 going on
last stable is r2.2.3

PID controller to govern EC fan (0...10v PWM control line) as a PID temperature controller. 
Termperature sensor is ds18b20 on 1-wire
UI oled screen 128x32 ssd1306 on i2c
UI input encoder with switch on interrupts
### The device is used to govern EC fan thru 0...10v control line using temperature reading from ds18b20 digital sensor.
- It utilize **ssd1306 128x32 oled display** to show various data and plot graphs.
- Use **encoder with button** to roll thru displays and enter settings. _(with acceleration)_
- ds18b20 **1-wire digital temperature sensor** connected via 3 wire with GND and +5v, _supposedly to make reading routine faster. but i did not find any mentions of id it is faster than on parasitic power_
- Arduino is **powered by** GND and +10v from fan's control line. (my sample use **<35ma** with most pixels white) _I was about to use arduino pro mini, but voltage regulator is too weak to drive arduino and screen, so i turned to nano as it has more powerful +5v voltage regulator_
- PWM signal is raised to 10v and **inverted** with two transistors. _Inversion is used to prevent fan from stopping in case of device failure. Discussible._

### Various variables and data can be set and read:
- Goal temperature.
- PID variables: **kP**, **kI**, **kD** and overall **multiplier** (0.01...100).
- There is a **P+I+D math display** on multiplier screen for you to have deeper understanding of how your config works.
- MinPower (some fans can't run at less than 5 or 10%)
- MaxPower (sometimes you don't want fan to go full speed)
- You can set **hysteresys delay** in seconds. Think of it as a _cool-down time you need to wait after turning fan off before turning it on again_. It prevents fan from going fast on-off at a margin values.
- **Temperature offset** can be entered -9.0...9.0^ (0.1^ increment) if your sensor lies linearly.
- **Display timeout** 0...600sec so oled display will last longer.
- You can **turn image 180^** and encoder reading will be inverted.
- Manual and automatic **store setting** to EEPROM on case of power loss.
- You can avoid EEPROM degradation restricting to store current screen. It is convenient while configuring, but isn't needed after.
- You can play with PId parameters with AUSTOISAVE OFF, and then **restore settings from EEPROM**, or save new settings if new config is better.
- **Temperature graph plot** show last 128 readings for diagnostics.
- **PIDoutput graph plot** show last 128 output values, so you can diagnose and configure PID.
- **Plot sampling rate** can be adjusted averaging up to 250 reading in a single data line in plot, so you can log longer periods. _(sample rate is approx. 3rps)_
- **Automatic vertical range** for graph plots. _Set-mode will change it to full range._
- You can **manually set output rate**. This mode is stored into EEPROM to **return after power loss**. 
- Device enters manual mode with last stored value **if temp. sensor is lost**.
- Device enters manual mode **if screen has failed**.

![EC-FAN-pid controller r2 2_bb](https://user-images.githubusercontent.com/98293163/150709377-43f2c787-2473-4872-998f-7ee267ae28f0.png)
![EC-FAN-pid controller r2 2_schem](https://user-images.githubusercontent.com/98293163/150709451-bd5a9352-163f-49d2-b703-9abf2e3a6ef2.png)