import io
from time import sleep,time
import struct
from numpy import interp
from inputs import get_key
from inputs import get_gamepad
import copy
current_milli_time = lambda: int(round(time() * 1000))
def milli_time_to_report_time(x): return  int(round(x/ 1000/.024))

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
	JoyoOptionsRemove = {
		"SYN_REPORT" : 1,
	}
	JoyoOptionsAnalog = {
	           "ABS_Y": "LY",	
	           "ABS_X" : "LX", 
	           "ABS_RY" : "RY",	
	           "ABS_RX": "RX", 
	           "ABS_Z": 6, 
	           "ABS_RZ": 7,
	}

	JoyoOptionsHat = {
	           "ABS_Y": 2,	
	           "ABS_X" : 3, 
	           "ABS_RY" : 4,	
	           "ABS_RX": 5, 
	           "ABS_Z": 6, 
	           "ABS_RZ": 7,
	}

	JoyStringOut = {
	"SWITCH": 	0,
	"HAT": 		STICK_CENTER,
	"LX":  		STICK_CENTER,
	"LY":		STICK_CENTER,
	"RX":		STICK_CENTER,
	"RY":		STICK_CENTER,
	"TIME":		0
	}

	JoyString = {
	"SWITCH": 	0,
	"HAT": 		STICK_CENTER,
	"LX":  		STICK_CENTER,
	"LY":		STICK_CENTER,
	"RX":		STICK_CENTER,
	"RY":		STICK_CENTER,
	"TIME":		0
	}


	CommandlistCheck = {
		36: "$",
		35: "#",
		33: "!",
		64: "@"
	}

	JoyoOptions={
		"BTN_WEST": [SWITCH_Y,False],
		"BTN_SOUTH": [SWITCH_B,False],
		"BTN_EAST": [SWITCH_A,False],
		"BTN_NORTH": [SWITCH_X,False],
		"BTN_TL": [SWITCH_L,False],
		"BTN_TR": [SWITCH_R,False],
		"BTN_START": [SWITCH_MINUS,False],
		"BTN_SELECT": [SWITCH_PLUS,False],
		"BTN_THUMBL": [SWITCH_LCLICK,False],
		"BTN_THUMBR": [SWITCH_RCLICK,False],
		"ABS_HAT0X": [SWITCH_HOME,False],
		"ABS_HAT0Y": [SWITCH_CAPTURE,False],
	}
	joyLoopTime = 0
	

	sendcommand = {}

	recoredJoystickCommandSet = []
	recoredJoystickCommandCount = 0
	def __init__(self,ser):
		self.ser = ser

	def reportCheck(self, jb,jbo):
		NeedsUpdate = False
		for byew in jb:
			if jbo[byew] != jb[byew]:
				temp = jb[byew]
				jbo[byew] = temp
				NeedsUpdate = True
		return NeedsUpdate

	def joystickControl(self,testing=False,timeout = 0):
		min_Stick_Map = 5000
		max_Stick_Map = 33000
		while 1:
			if self.recoredJoystickCommandCount == 0:
				self.RunStart = current_milli_time()
			elif  self.recoredJoystickCommandCount == 1:
				print("Run Started")

			if timeout > 0:
				if (current_milli_time() - self.RunStart)/1000 > timeout :
					print("Run Finished\n")
					for reportCMD in self.recoredJoystickCommandSet:
						print (reportCMD)
					break

			events = get_gamepad()
			for event in events:
				if event.code not in self.JoyoOptionsRemove:
					if event.code in self.JoyoOptions:
						if event.state is 0:
							self.JoyoOptions[event.code][1] = False
						else:
							self.JoyoOptions[event.code][1] = True
						JoyoOptionsbuttons = 0
						for button in self.JoyoOptions:
							if self.JoyoOptions[button][1]:
								JoyoOptionsbuttons = JoyoOptionsbuttons | self.JoyoOptions[button][0]
						self.JoyString["SWITCH"] = JoyoOptionsbuttons
					elif event.code in self.JoyoOptionsAnalog:
						if event.code is "ABS_X" or event.code is "ABS_RX":
							event.state = event.state*-1
						if (event.state > min_Stick_Map or event.state < -1*min_Stick_Map):
							if event.state > 0:
								event.state = int(interp(event.state,[min_Stick_Map,max_Stick_Map],[self.STICK_CENTER,self.STICK_MIN]))
								if event.state < self.STICK_MIN:
									event.state = self.STICK_MIN;
							else:
								event.state = int(interp(event.state,[-1*max_Stick_Map,-1*min_Stick_Map],[self.STICK_MAX,self.STICK_CENTER]))
								if event.state > self.STICK_MAX:
									event.state = self.STICK_MAX;

							
						else:
							event.state = self.STICK_CENTER

						if self.JoyoOptionsAnalog[event.code] in self.JoyString:
							if self.JoyString[self.JoyoOptionsAnalog[event.code]] == event.state:
								pass
							else:
								self.JoyString[self.JoyoOptionsAnalog[event.code]] = event.state
					else:
						print(event.ev_type, event.code, event.state)
				if self.reportCheck(self.JoyString,self.JoyStringOut):
					if testing:
						self.sendUSBReportCheck(self.JoyStringOut["LX"]);
						self.sendUSBReportCheck(self.JoyStringOut["LY"]);
						self.sendUSBReportCheck(self.JoyStringOut["RX"]);
						self.sendUSBReportCheck(self.JoyStringOut["RY"]);
						print(self.JoyStringOut)
					else:
						mills = current_milli_time()
						if (self.joyLoopTime == 0):
							self.joyLoopTime = mills
						
						if self.recoredJoystickCommandCount > 0:
							delayTime = (mills - self.joyLoopTime)/1000
							self.recoredJoystickCommandSet[self.recoredJoystickCommandCount-1]["TIME"] = delayTime
							self.recoredJoystickCommandSet.append(copy.deepcopy(self.JoyStringOut))
							self.recoredJoystickCommandCount += 1
						else:
							self.recoredJoystickCommandSet.append(copy.deepcopy(self.JoyStringOut))
							self.recoredJoystickCommandSet[self.recoredJoystickCommandCount]["TIME"] = 0
							self.recoredJoystickCommandCount +=1

						if(self.recoredJoystickCommandCount > 1):
							#print(self.recoredJoystickCommandSet[self.recoredJoystickCommandCount-2],self.recoredJoystickCommandCount)
							pass

						self.JoyString["TIME"]=0
						self.JoyStringOut["TIME"]=0

						self.joyLoopTime = mills
						self.sendUSBReport(self.JoyStringOut["SWITCH"],self.JoyStringOut["HAT"],self.JoyStringOut["LX"],self.JoyStringOut["LY"],self.JoyStringOut["RX"],self.JoyStringOut["RY"])
						#print("\n")
						sleep(.025)
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
		#print(cmdStr,len(cmdStr))

	def sendUSBReportCheck(self, report):
		while report in self.CommandlistCheck:
			#print("Yah found "+self.CommandlistCheck[report])
			report+=1
		if report > 255:
			report = 255
		if report < 0:
			report = 0
		return report


	def sendUSBReport(self,SWITCH=0,HAT = HAT_CENTER,LX = STICK_CENTER,LY = STICK_CENTER,RX = STICK_CENTER,RY = STICK_CENTER):
		# report info
		# @[SWITCH0][SWITCH1][HAT][LX][LY][RX][RY]
		LX = self.sendUSBReportCheck(LX);
		LY = self.sendUSBReportCheck(LY);
		RX = self.sendUSBReportCheck(RX);
		RY = self.sendUSBReportCheck(RY);

		SWITCH0, SWITCH1 = struct.pack('<H', SWITCH)
		rpstring = b'@'+bytes([SWITCH0])+bytes([SWITCH1])+bytes([HAT])+bytes([LX])+bytes([LY])+bytes([RX])+bytes([RY])
		#print(rpstring,len(rpstring))
		#self.ser.write(b'@')
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