/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for Joystick.c.
 */

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "program.h"
#include <Wire.h>
#include <SPI.h>
  /* Includes: */
  /* Includes: */
    #include <avr/io.h>
    #include <avr/wdt.h>
    #include <avr/power.h>
    #include <avr/interrupt.h>
    #include <string.h>

    #include "Descriptors.h"

    #include <LUFA/LUFA/Drivers/Board/LEDs.h>
    #include <LUFA/LUFA/Drivers/Board/Joystick.h>
    #include <LUFA/LUFA/Drivers/USB/USB.h>
    #include <LUFA/LUFA/Platform/Platform.h>

// Type Defines
// Enumeration for joystick buttons.
typedef enum {
	SWITCH_Y       = 0x01,
	SWITCH_B       = 0x02,
	SWITCH_A       = 0x04,
	SWITCH_X       = 0x08,
	SWITCH_L       = 0x10,
	SWITCH_R       = 0x20,
	SWITCH_ZL      = 0x40,
	SWITCH_ZR      = 0x80,
	SWITCH_MINUS   = 0x100,
	SWITCH_PLUS    = 0x200,
	SWITCH_LCLICK  = 0x400,
	SWITCH_RCLICK  = 0x800,
	SWITCH_HOME    = 0x1000,
	SWITCH_CAPTURE = 0x2000,
} JoystickButtons_t;

#define HAT_TOP          0x00
#define HAT_TOP_RIGHT    0x01
#define HAT_RIGHT        0x02
#define HAT_BOTTOM_RIGHT 0x03
#define HAT_BOTTOM       0x04
#define HAT_BOTTOM_LEFT  0x05
#define HAT_LEFT         0x06
#define HAT_TOP_LEFT     0x07
#define HAT_CENTER       0x08

#define STICK_MIN      0
#define STICK_CENTER 128
#define STICK_MAX    255


#define  EEPROMCommandSize 50
#define ECHOES 2
int echoes = 0;
int bufindex = 0;
int portsval = 0;
int IsWriteing = 0;
bool report = false;
uint16_t loopCount = 0;
int Serialstepcount = 0;
bool SaveToEprom = false;
bool boot = false;
bool playingBack = false;

// Joystick HID report structure. We have an input and an output.
typedef struct  __attribute__ ((packed)){
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
	uint8_t  VendorSpec;
} USB_JoystickReport_Input_t;


bool operator==(const USB_JoystickReport_Input_t& lhs, const USB_JoystickReport_Input_t& rhs)
{
   return (lhs.Button == rhs.Button)&&(lhs.HAT == rhs.HAT)&&(lhs.LX == rhs.LX)&&(lhs.LY == rhs.LY)&&(lhs.RX == rhs.RX)&&(lhs.RY == rhs.RY)&&(lhs.VendorSpec == rhs.VendorSpec);
}

typedef struct  __attribute__ ((packed)){
  USB_JoystickReport_Input_t report;
  uint16_t reportDelay;
}USB_JoystickReport_SDRec;


int duration_count = 0;


// The output is structured as a mirror of the input.
// This is based on initial observations of the Pokken Controller.
typedef struct  __attribute__ ((packed)) {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
} USB_JoystickReport_Output_t;

// Function Prototypes
// Setup all necessary hardware, including USB initialization.
void SetupHardware(void);
// Process and deliver data from IN and OUT endpoints.
void HID_Task(void);
// USB device event handlers.
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData);
void LoadEEPROM();
void HandalFileOpening(String name, bool Writeing);
bool slideSwitch();
bool clearButton();
void prossesCommandSet(byte Buffer[]);
USB_JoystickReport_Input_t runScript(USB_JoystickReport_Input_t* const ReportData, command CommandStep[], int CommandStepSize);

typedef enum {
  SYNC_CONTROLLER,
  SYNC_POSITION,
  BREATHE,
  PROCESS
} State_t;
State_t state = SYNC_CONTROLLER;


USB_JoystickReport_Input_t last_report;
USB_JoystickReport_Input_t TempReport;

USB_JoystickReport_SDRec curentReport;

command prossing;
command Serialstep[EEPROMCommandSize];

static USB_JoystickReport_Input_t const CommandPROCESS(USB_JoystickReport_Input_t* const ReportData , command ThisCommand){
  switch (ThisCommand.button)
      {

        case UP:
          ReportData->LY = STICK_MIN;
          Serial1.println("UP");       
          break;

        case LEFT:
          ReportData->LX = STICK_MIN;       
          break;

        case DOWN:
          ReportData->LY = STICK_MAX;
          Serial1.println("DOWN");       
          break;

        case RIGHT:
          ReportData->LX = STICK_MAX;
          Serial1.println("RIGHT");       
          break;

        case UP_RIGHT:
          ReportData->LY = STICK_MIN;
          ReportData->LX = STICK_MAX; 
          Serial1.println("UP_RIGHT");
          break;
        case UP_LEFT:
          ReportData->LY = STICK_MIN;
          ReportData->LX = STICK_MIN;
          Serial1.println("UP_LEFT");
          break;
        case DOWN_RIGHT:
          ReportData->LY = STICK_MAX;
          ReportData->LX = STICK_MAX;
          Serial1.println("DOWN_RIGHT");
          break;
        case DOWN_LEFT:
          ReportData->LY = STICK_MAX;
          ReportData->LX = STICK_MIN;
          Serial1.println("DOWN_LEFT");
          break;

        case A:
          ReportData->Button |= SWITCH_A;
          Serial1.println("A");
          break;

        case B:
          ReportData->Button |= SWITCH_B;
          Serial1.println("B");
          break;

        case R:
          ReportData->Button |= SWITCH_R;
          Serial1.println("R");
          break;
          
        case L:
          ReportData->Button |= SWITCH_L;
          Serial1.println("L");
          break;  
          
        case X:
          ReportData->Button |= SWITCH_X;
          Serial1.println("X");
          break;
          
        case Y:
          ReportData->Button |= SWITCH_Y;
          Serial1.println("Y");
          break;

       case MINUS:
          ReportData->Button |= SWITCH_MINUS;
          Serial1.println("MINUS");
          break;
          
        case PLUS:
          ReportData->Button |= SWITCH_PLUS;
          Serial1.println("PLUS");
          break;
          
        case THROW:
          ReportData->LY = STICK_MIN;       
          ReportData->Button |= SWITCH_R;
          Serial1.println("THROWWN");
          break;

        case TRIGGERS:
          ReportData->Button |= SWITCH_L | SWITCH_R;
          Serial1.println("TRIGGERS");
          break;

        default:
          ReportData->LX = STICK_CENTER;
          ReportData->LY = STICK_CENTER;
          ReportData->RX = STICK_CENTER;
          ReportData->RY = STICK_CENTER;
          ReportData->HAT = HAT_CENTER;
          break;
      }
      return *ReportData;

};
#endif

