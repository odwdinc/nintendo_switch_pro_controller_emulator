import io
from time import sleep
import struct
"""Simple example showing how to get keyboard events.""""""Simple example showing how to get keyboard events."""

from inputs import get_key
class NS:

	SWITCH_Y       = 0x01
	SWITCH_B       = 0x02
	SWITCH_A       = 0x04
	SWITCH_X       = 0x08
	SWITCH_L       = 0x10
	SWITCH_R       = 0x20
	SWITCH_ZL      = 0x40
	SWITCH_ZR      = 0x80
	SWITCH_MINUS   = 0x100
	SWITCH_PLUS    = 0x200
	SWITCH_LCLICK  = 0x400
	SWITCH_RCLICK  = 0x800
	SWITCH_HOME    = 0x1000
	SWITCH_CAPTURE = 0x2000

	HAT_TOP        = 0x00
	HAT_TOP_RIGHT  = 0x01
	HAT_RIGHT      = 0x02
	HAT_BOTTOM_RIGHT = 0x03
	HAT_BOTTOM      = 0x04
	HAT_BOTTOM_LEFT = 0x05
	HAT_LEFT         = 0x06
	HAT_TOP_LEFT     = 0x07
	HAT_CENTER       = 0x08

	STICK_MIN     =  0
	STICK_CENTER = 128
	STICK_MAX    = 255

	X = b'a'
	Y = b'b'
	A = b'c'
	B = b'd'
	L = b'e'
	R = b'f'
	MINUS = b'g'
	PLUS = b'h'
	UP = b'i'
	DOWN = b'j'
	LEFT = b'k'
	RIGHT = b'l'
	UP_RIGHT = b'm'
	UP_LEFT = b'n'
	DOWN_RIGHT = b'o'
	DOWN_LEFT = b'p'
	THROW = b'q'
	TRIGGERS = b'r'
	NOTHING = b's'
	ReportPlaybackList=["HatJump","Jump"]
	ser = None

	options = {"KEY_W" : UP, #up
	           "KEY_A": LEFT,	#left
	           "KEY_S" : DOWN, #down
	           "KEY_D" : RIGHT,	#right
	           "KEY_SPACE": B, #B
	           "KEY_LEFTCTRL": A, #A
	           "KEY_LEFTSHIFT": Y, #Y
	}

	sendcommand = {}
	def __init__(self,ser):
		self.ser = ser

	def keyControl(self):
		while 1:
				events = get_key()
				if events:
					for event in events:
						#print(event.ev_type, event.code, event.state)
						if event.code in options:
							if event.state ==1 :
								sendcommand[event.code] =True
							else:
								sendcommand[event.code] =False
				commandstring =  b''
				for command in sendcommand:
					if sendcommand[command] == True:
						commandstring += options[command]
				if len(commandstring) == 0:
					commandstring = b'$s'
				self.ser.write(commandstring)
				print(commandstring)

	def runCommandPlayback(self,commandPlayback,savetoEEPROM=False):
		cmdStr = b''
		for command in commandPlayback:
				cmd = command[0]
				time = bytes([command[1]])
				cmdStr += b'$'
				cmdStr += cmd
				cmdStr += time
		if(savetoEEPROM):
			cmdStr += b'!'
		cmdStr += b'#'
		

		self.ser.write(b'#')
		self.ser.write(cmdStr)
		print(cmdStr,len(cmdStr))


	def sendUSBReport(self,SWITCH=0,HAT = HAT_CENTER,LX = STICK_CENTER,LY = STICK_CENTER,RX = STICK_CENTER,RY = STICK_CENTER):
		# report info
		# @[SWITCH0][SWITCH1][HAT][LX][LY][RX][RY]
		SWITCH0, SWITCH1 = struct.pack('<H', SWITCH)
		rpstring = b''+bytes([SWITCH0])+bytes([SWITCH1])+bytes([HAT])+bytes([LX])+bytes([LY])+bytes([RX])+bytes([RY])
		print (rpstring)
		self.ser.write(b'@')
		self.ser.write(rpstring)

	def sendUSBReportList(self, ReportPlayback):
		if ReportPlayback is "HatJump":
			self.sendUSBReport(SWITCH=self.SWITCH_Y,LY=self.STICK_MIN)
			sleep(.9)
			self.sendUSBReport(SWITCH=self.SWITCH_A|self.SWITCH_Y,LY=self.STICK_MIN)
			sleep(.1)
			self.sendUSBReport(SWITCH=self.SWITCH_Y,LY=self.STICK_MIN)
			sleep(.5)
			self.sendUSBReport()
		elif ReportPlayback is "Jump":
			self.sendUSBReport(LY=self.STICK_MIN)
			sleep(.2)
			self.sendUSBReport(SWITCH=self.SWITCH_A,LY=self.STICK_MIN)
			sleep(.5)
			self.sendUSBReport()
		else:
			self.sendUSBReport()