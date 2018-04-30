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

    Header file for Joystick.c.
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

#define EEPROMCommandStartPos  20
#define  EEPROMCommandSize 35
#define EEPROMCommandlog 19
#define ECHOES 2

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

// Joystick HID report structure. We have an input and an output.
typedef struct  __attribute__ ((packed)) {
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
  return (lhs.Button == rhs.Button) && (lhs.HAT == rhs.HAT) && (lhs.LX == rhs.LX) && (lhs.LY == rhs.LY) && (lhs.RX == rhs.RX) && (lhs.RY == rhs.RY) && (lhs.VendorSpec == rhs.VendorSpec);
}

typedef struct  __attribute__ ((packed)) {
  USB_JoystickReport_Input_t report;
  uint16_t reportDelay;
} USB_JoystickReport_SDRec;





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

typedef enum {
  SYNC_CONTROLLER,
  BREATHE,
  PROCESS
} State_t;



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
int HandalFileOpening(String name, bool Writeing);
bool slideSwitch();
bool clearButton();
void readPref();
void prossesCommandSet(byte Buffer[]);
void runScript(command CommandStep[], int CommandStepSize);
void startNewRecording();
void printReport(USB_JoystickReport_Input_t* TReport, String Pre);
void printUSBCommands();
 void flashErrot();
 
int echoes = 0;
int bufindex = 0;
int portsval = 0;
int IsWriteing = 0;
uint16_t loopCount = 0;
int Serialstepcount = -1;
bool SaveToEprom = false;
bool boot = false;
bool playingBack = false;
int duration_count = 0;
int reportCount = 0;

State_t state = SYNC_CONTROLLER;

USB_JoystickReport_Input_t last_report;
USB_JoystickReport_SDRec curentReport;

command prossing;
command Serialstep[EEPROMCommandSize];
command LastProssing;

static void voidDebugPrint(String str) {

#ifdef Serialoutput
  Serial1.print(str);
#endif
}
static void voidDebugPrint(unsigned char str, unsigned char format) {
#ifdef Serialoutput
  Serial1.print(str, format);
#endif
}

static void voidDebugPrintln(String str) {
#ifdef Serialoutput
  Serial1.println(str);
#endif
}



static void getDefaultReport(USB_JoystickReport_Input_t* reportt) {
  reportt->HAT = HAT_CENTER;
  reportt->RY = STICK_CENTER;
  reportt->RX = STICK_CENTER;
  reportt->LX = STICK_CENTER;
  reportt->LY = STICK_CENTER;
  reportt->Button = 0;
}

void printReport(USB_JoystickReport_Input_t* TReport, String Pre) {
  /*
    voidDebugPrint(Pre);
    voidDebugPrint("  reportButtons: ");
    voidDebugPrint(TReport->Button, DEC);
    voidDebugPrint("  LX,LY: [");
    voidDebugPrint(TReport->LX, DEC);
    voidDebugPrint(",");
    voidDebugPrint(TReport->LY, DEC);
    voidDebugPrint("]  RX,RY: [");
    voidDebugPrint(TReport->RX, DEC);
    voidDebugPrint(",");
    voidDebugPrint(TReport->RY, DEC);
    voidDebugPrintln( "]");
  */

}

static void CommandPROCESS(USB_JoystickReport_Input_t*  ReportData , command ThisCommand) {
  bool printCommand = false;//(LastProssing.button != ThisCommand.button);
  if (printCommand) printReport(ReportData, "Command Prossing: ");
  LastProssing = ThisCommand;
  switch (ThisCommand.button)
  {

    case UP:
      ReportData->LY = STICK_MIN;
      if (printCommand) voidDebugPrintln("UP");
      break;

    case LEFT:
      ReportData->LX = STICK_MIN;
      break;

    case DOWN:
      ReportData->LY = STICK_MAX;
      if (printCommand) voidDebugPrintln("DOWN");
      break;

    case RIGHT:
      ReportData->LX = STICK_MAX;
      if (printCommand) voidDebugPrintln("RIGHT");
      break;

    case UP_RIGHT:
      ReportData->LY = STICK_MIN;
      ReportData->LX = STICK_MAX;
      if (printCommand) voidDebugPrintln("UP_RIGHT");
      break;
    case UP_LEFT:
      ReportData->LY = STICK_MIN;
      ReportData->LX = STICK_MIN;
      if (printCommand)  voidDebugPrintln("UP_LEFT");
      break;
    case DOWN_RIGHT:
      ReportData->LY = STICK_MAX;
      ReportData->LX = STICK_MAX;
      if (printCommand)  voidDebugPrintln("DOWN_RIGHT");
      break;
    case DOWN_LEFT:
      ReportData->LY = STICK_MAX;
      ReportData->LX = STICK_MIN;
      if (printCommand) voidDebugPrintln("DOWN_LEFT");
      break;

    case A:
      ReportData->Button |= SWITCH_A;
      if (printCommand) voidDebugPrintln("A");
      break;

    case B:
      ReportData->Button |= SWITCH_B;
      voidDebugPrintln("B");
      break;

    case R:
      ReportData->Button |= SWITCH_R;
      if (printCommand) voidDebugPrintln("R");
      break;

    case L:
      ReportData->Button |= SWITCH_L;
      voidDebugPrintln("L");
      break;

    case X:
      ReportData->Button |= SWITCH_X;
      if (printCommand) voidDebugPrintln("X");
      break;

    case Y:
      ReportData->Button |= SWITCH_Y;
      if (printCommand) voidDebugPrintln("Y");
      break;

    case MINUS:
      ReportData->Button |= SWITCH_MINUS;
      if (printCommand) voidDebugPrintln("MINUS");
      break;

    case PLUS:
      ReportData->Button |= SWITCH_PLUS;
      if (printCommand) voidDebugPrintln("PLUS");
      break;

    case THROW:
      ReportData->LY = STICK_MIN;
      ReportData->Button |= SWITCH_R;
      if (printCommand) voidDebugPrintln("THROWWN");
      break;

    case TRIGGERS:
      ReportData->Button |= SWITCH_L | SWITCH_R;
      if (printCommand) voidDebugPrintln("TRIGGERS");
      break;

    default:
      getDefaultReport(ReportData);
      if (printCommand) voidDebugPrintln("CENTERED");
      break;
  }
};
#endif

