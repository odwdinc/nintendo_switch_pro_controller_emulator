from N_Switch import NS
import serial
from time import sleep

FindYoshiDemo = [
{'SWITCH': 4, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.091} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 1.305} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.04} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 1.125} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.204} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.252} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.206} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 194, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.385} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.499} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 1.156} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 194, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.674} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.214} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.371} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 1.018} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 1.53} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.005} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 1.013} ,
{'SWITCH': 1, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.079} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 1.852} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.007} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 151, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 151, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 194, 'RX': 128, 'RY': 128, 'TIME': 1.148} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 255, 'RX': 128, 'RY': 128, 'TIME': 0.169} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 255, 'RX': 128, 'RY': 128, 'TIME': 0.357} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 255, 'RX': 128, 'RY': 128, 'TIME': 0.071} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 194, 'RX': 128, 'RY': 128, 'TIME': 0.08} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 151, 'RX': 128, 'RY': 128, 'TIME': 0.127} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.006} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.003} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.009} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.099} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.047} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.064} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.072} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.523} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.096} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.014} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.439} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.091} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.33} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.002} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.082} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.032} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.044} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.04} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.098} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.525} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.119} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.097} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.062} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.017} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.069} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.749} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.047} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.199} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.027} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.31} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.123} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.015} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.062} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.136} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.334} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 0, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.048} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 0, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.037} ,
{'SWITCH': 4, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.004} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.015} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.103} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.199} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 1.648} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.0} ,
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0} ,
]

coinefarmTest = [
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': .24} , # 54
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 151, 'RX': 128, 'RY': 128, 'TIME': 0.08} , # 2
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 194, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 3
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 194, 'RX': 128, 'RY': 128, 'TIME': 0.035} , # 4
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 255, 'RX': 128, 'RY': 128, 'TIME': 0.66} , # 5
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 255, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 6
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.026} , # 7
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.139} , # 8
{'SWITCH': 32, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.14} , # 9
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.55} , # 10
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.03} , # 11
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.03} , # 12
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.71} , # 13
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.55} , # 14
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 15
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.285} , # 16
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 17
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.055} , # 18
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.09} , # 19
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.47} , # 20
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.73} , # 21
{'SWITCH': 4, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.141} , # 22
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.51} , # 23
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.026} , # 24
{'SWITCH': 4, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.114} , # 25
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.11} , # 26
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.8} , # 27
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.87} , # 28
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.68} , # 29
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.026} , # 30
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 31
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.659} , # 32
{'SWITCH': 0, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.86} , # 33
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.026} , # 34
{'SWITCH': 1, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 35
{'SWITCH': 1, 'HAT': 128, 'LX': 65, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 36
{'SWITCH': 1, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.044} , # 37
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.09} , # 38
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 39
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.026} , # 40
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 4.629} , # 41
{'SWITCH': 256, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.12} , # 42
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.59} , # 43
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 108, 'RX': 128, 'RY': 128, 'TIME': 0.04} , # 44
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 45
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 46
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 0, 'RX': 128, 'RY': 128, 'TIME': 0.09} , # 47
{'SWITCH': 0, 'HAT': 128, 'LX': 151, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.03} , # 48
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 65, 'RX': 128, 'RY': 128, 'TIME': 0.025} , # 49
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.315} , # 50
{'SWITCH': 0, 'HAT': 128, 'LX': 108, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.07} , # 51
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.27} , # 52
{'SWITCH': 4, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 0.11} , # 53
{'SWITCH': 0, 'HAT': 128, 'LX': 128, 'LY': 128, 'RX': 128, 'RY': 128, 'TIME': 12} , # 54
]

def main():	

	with serial.Serial('COM12', 19200) as ser:
		mySwitch = NS(ser)
		#mySwitch.joystickControl()
		for i in range(0,int(1000/35)):
			mySwitch.RunJoystickReport(coinefarmTest)
if __name__ == "__main__":
	main()