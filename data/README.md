# Dependincy
```
pip install numpy inputs pyserial
```
# Usage
	runCommandPlayback(commandPlayback,savetoEEPROM=False)
	sendUSBReport(SWITCH=0,HAT = HAT_CENTER,LX = STICK_CENTER,LY = STICK_CENTER,RX = STICK_CENTER,RY = STICK_CENTER)
	sendUSBReportList(ReportPlayback):
		ReportPlaybackList["HatJump","Jump"]
