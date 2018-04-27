# Nintendo switch pro controller emulator

Allows action to be replayed on nintendo switch using ATmega32u4 

included Python Lib to allow for updating and live control see data folder.

Moved to Ardino to support easer install and more features.

[Arduino-Lufa](https://github.com/Palatis/Arduino-Lufa)

[ATmega32U4](https://www.amazon.com/OSOYOO-ATmega32U4-arduino-Leonardo-ATmega328/dp/B012FOV17O)


# Hardware

Pin 2 is used as a Clear pin for the USB reports 
  
  This will stop any running scripts sent over the serial port.
  
  Connecting this pin to ground will clear the reports.
  
Pin 3 is used as swtich for serial Mode / vs embeded USB reports.

  Connecting this pin Low (ground) will run the embeded USB reports.
  
  Default is High Not conected and will run in serial Mode. 
  
  If there is any USB reports stored in EEPROM thay will run.
  

# Examples

[Live PlayBack Example](https://github.com/odwdinc/nintendo_switch_pro_controller_emulator/blob/master/data/examplePSJoyCon.py)

[![Making progress](https://img.youtube.com/vi/9iv6o_2WkOk/0.jpg)](https://www.youtube.com/watch?v=9iv6o_2WkOk "Making progress")

[Coine Farming Example](https://github.com/odwdinc/nintendo_switch_pro_controller_emulator/blob/master/data/example.py)

[![Coine Farming Example](https://img.youtube.com/vi/wtSV8fbQbBM/0.jpg)](https://www.youtube.com/watch?v=wtSV8fbQbBM "Coine Farming Example")


# Thanks

Thanks to Shiny Quagsire for his Splatoon post printer and progmem for his [original discovery.](https://github.com/shinyquagsire23/Switch-Fightstick)

Thanks to Bertrand Fan for his [Automated snowball thrower](https://github.com/bertrandom/snowball-thrower)
