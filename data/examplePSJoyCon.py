from inputs import get_gamepad
from N_Switch import NS
from numpy import interp
import serial
import struct

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
"HAT": 		NS.STICK_CENTER,
"LX":  		NS.STICK_CENTER,
"LY":		NS.STICK_CENTER,
"RX":		NS.STICK_CENTER,
"RY":		NS.STICK_CENTER,
}

JoyString = {
"SWITCH": 	0,
"HAT": 		NS.STICK_CENTER,
"LX":  		NS.STICK_CENTER,
"LY":		NS.STICK_CENTER,
"RX":		NS.STICK_CENTER,
"RY":		NS.STICK_CENTER,
}


def reportCheck(jb,jbo):
	NeedsUpdate = False
	for byew in jb:
		if jbo[byew] != jb[byew]:
			temp = jb[byew]
			jbo[byew] = temp
			NeedsUpdate = True
	return NeedsUpdate

JoyoOptions={
	"BTN_WEST": [NS.SWITCH_Y,False],
	"BTN_SOUTH": [NS.SWITCH_B,False],
	"BTN_EAST": [NS.SWITCH_A,False],
	"BTN_NORTH": [NS.SWITCH_X,False],
	"BTN_TL": [NS.SWITCH_L,False],
	"BTN_TR": [NS.SWITCH_R,False],
	"BTN_START": [NS.SWITCH_MINUS,False],
	"BTN_SELECT": [NS.SWITCH_PLUS,False],
	"BTN_THUMBL": [NS.SWITCH_LCLICK,False],
	"BTN_THUMBR": [NS.SWITCH_RCLICK,False],
	"ABS_HAT0X": [NS.SWITCH_HOME,False],
	"ABS_HAT0Y": [NS.SWITCH_CAPTURE,False],
}

def main():	
	global JoyStringOut
	global JoyString
	min_Stick_Map = 4000
	max_Stick_Map = 32767
	dirtty =  True
	with serial.Serial('COM12', 19200) as ser:
		mySwitch = NS(ser)
		while 1:
			events = get_gamepad()
			for event in events[0:1]:
				if event.code not in JoyoOptionsRemove:
					if event.code in JoyoOptions:
						if event.state is 0:
							JoyoOptions[event.code][1] = False
						else:
							JoyoOptions[event.code][1] = True
						JoyoOptionsbuttons = 0
						for button in JoyoOptions:
							if JoyoOptions[button][1]:
								JoyoOptionsbuttons = JoyoOptionsbuttons | JoyoOptions[button][0]
						JoyString["SWITCH"] = JoyoOptionsbuttons
					elif event.code in JoyoOptionsAnalog:
						if event.code is "ABS_X" or event.code is "ABS_RX":
							event.state = event.state*-1
						if (event.state > min_Stick_Map or event.state < -1*min_Stick_Map):
							if event.state > 0:
								#event.state = int(interp(event.state,[min_Stick_Map,max_Stick_Map],[NS.STICK_CENTER,NS.STICK_MIN]))
								#if event.state < NS.STICK_MIN:
								event.state = NS.STICK_MIN;
							else:
								#event.state = int(interp(event.state,[-1*max_Stick_Map,-1*min_Stick_Map],[NS.STICK_MAX,NS.STICK_CENTER]))
								#if event.state > NS.STICK_MAX:
								event.state = NS.STICK_MAX;
							
						else:
							event.state = NS.STICK_CENTER
						if JoyoOptionsAnalog[event.code] in JoyString:
							if JoyString[JoyoOptionsAnalog[event.code]] == event.state:
								pass
							else:
								JoyString[JoyoOptionsAnalog[event.code]] = event.state
					else:
						print(event.ev_type, event.code, event.state)
				if reportCheck(JoyString,JoyStringOut):	
					#print(JoyStringOut)
					mySwitch.sendUSBReport(JoyStringOut["SWITCH"],JoyStringOut["HAT"],JoyStringOut["LX"],JoyStringOut["LY"],JoyStringOut["RX"],JoyStringOut["RY"])
					#dirtty =  True;
				elif dirtty:
					dirtty =  False;
					mySwitch.sendUSBReport()
if __name__ == "__main__":
	main()