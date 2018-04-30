import serial
from N_Switch import NS
from time import sleep
"""
	runCommandPlayback(commandPlayback,savetoEEPROM=False)
	sendUSBReport(SWITCH=0,HAT = HAT_CENTER,LX = STICK_CENTER,LY = STICK_CENTER,RX = STICK_CENTER,RY = STICK_CENTER)
	sendUSBReportList(ReportPlayback):
		ReportPlaybackList["HatJump","Jump"]
		
"""
#[time scaled] == TimeInSec/.024
# 20 = 0.48s/.024

""" Farms coins from coinfrogs """
frogCoineFarming = [
	# Find Coin Froge
	[ NS.NOTHING,   10 ],
	[ NS.DOWN,      20 ], # Start Down the hall 0
	[ NS.NOTHING,   10 ],
	[ NS.R,          5 ],
	[ NS.NOTHING,   20 ],
	[ NS.UP,       110 ],
	[ NS.LEFT,      15 ], #5
	[ NS.NOTHING,   10 ],
	[ NS.UP,       170 ],
	[ NS.LEFT,      10 ],
	# Capture Coin Froge
	[ NS.Y,         20 ],
	[ NS.NOTHING,  180 ], #10
	# Return to WayPoint
	[ NS.MINUS,     5 ],
	[ NS.NOTHING,  40 ],
	[ NS.UP,        5 ],
	[ NS.NOTHING,  20 ],
	[ NS.A,         5 ], #15
	[ NS.NOTHING, 250 ],
	[ NS.NOTHING, 250 ],
	[ NS.NOTHING, 50 ] #18
]

""" Simple Jump Test """
farmTEST = [	
	[ NS.NOTHING,   20 ],
	[ NS.A,   	  20 ],
	[ NS.NOTHING,   50 ]
]

""" Used to Par the controler to the Switch """
ParController = [	
	[ NS.NOTHING,  100 ],
	[ NS.TRIGGERS,   5 ],
	[ NS.NOTHING,   50 ],
	[ NS.TRIGGERS,   5 ],
	[ NS.NOTHING,   50 ],
	[ NS.A,         10 ],
	[ NS.NOTHING,   50 ],
]

def main():	
	with serial.Serial('COM12', 19200) as ser:
		mySwitch = NS(ser)
		"""Jump Using the Prebuile USB Report to Jump Forword called by name"""
		#mySwitch.sendUSBReportList("HatJump")

		"""Jump Using the Prebuile USB Report to Jump Forword called by ref in ReportPlaybackList"""
		#mySwitch.sendUSBReportList(NS.ReportPlaybackList[1])

		""" Runs the Command Set "frogCoineFarming" and saves to EEPROM """
		mySwitch.runCommandPlayback(frogCoineFarming)

		""" Runs the Command Set "farmTEST" """
		#mySwitch.runCommandPlayback(farmTEST)

		""" This is and example of how to send raw USBReports"""
		#mySwitch.sendUSBReport(LY=mySwitch.STICK_MIN)
		#sleep(.2)
		#mySwitch.sendUSBReport(SWITCH=mySwitch.SWITCH_A,LY=mySwitch.STICK_MIN)
		#sleep(.5)
		#mySwitch.sendUSBReport()
		#pass
		#with serial.Serial('COM12', 19200,timeout=5) as ser:
		# while True:
		# 	line = ser.readline()
		# 	if len(line)==0:
		# 		break
		# 	print(line)

if __name__ == "__main__":
	main()