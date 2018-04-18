from N_Switch import NS
import serial
from time import sleep
def main():	

	with serial.Serial('COM12', 19200) as ser:
		mySwitch = NS(ser)
		mySwitch.joystickControl(timeout = 10)
if __name__ == "__main__":
	main()