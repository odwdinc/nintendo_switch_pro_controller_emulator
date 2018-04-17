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
#include <Adafruit_CircuitPlayground.h>
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

// Joystick HID report structure. We have an input and an output.
typedef struct {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
	uint8_t  VendorSpec;
} USB_JoystickReport_Input_t;

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


typedef enum {
  SYNC_CONTROLLER,
  SYNC_POSITION,
  BREATHE,
  PROCESS
} State_t;
State_t state = SYNC_CONTROLLER;

static int speeds[] = { 5, 10, 50, 100 };
int currentSpeed = 0;


static void setPixel(int pixal,int pos){
  uint32_t offset = millis() / speeds[currentSpeed];
  if(pixal > 9){
    pixal-='a';
  }
  if(pos > 9){
    pos-='a';
  }
  if(pos >= 0){
    CircuitPlayground.strip.setPixelColor(pixal, CircuitPlayground.colorWheel(((pos * 256 / 10) + offset) & 255));
  }else{
    CircuitPlayground.strip.setPixelColor(pixal, 0,0,0);
  }
}

static void flash(uint32_t color){
  for(int i=0; i<10; i++){
     CircuitPlayground.strip.setPixelColor(i, color);
  }
  CircuitPlayground.strip.show();
}

static void flashRed(){
  flash(0xFF0000);
}

static void flashGreen(){
  flash(0x00FF00);
}




static USB_JoystickReport_Input_t const CommandPROCESS(USB_JoystickReport_Input_t* const ReportData , command ThisCommand){
  switch (ThisCommand.button)
      {

        case UP:
          setPixel(0,0);
          setPixel(9,0);
          ReportData->LY = STICK_MIN;
          Serial1.println("UP");       
          break;

        case LEFT:
          setPixel(1,0);
          setPixel(2,0);
          setPixel(3,0);
          ReportData->LX = STICK_MIN;       
          break;

        case DOWN:
          setPixel(4,0);
          setPixel(5,0);
          ReportData->LY = STICK_MAX;
          Serial1.println("DOWN");       
          break;

        case RIGHT:
          setPixel(7,0);
          setPixel(6,0);
          setPixel(8,0);
          ReportData->LX = STICK_MAX;
          Serial1.println("RIGHT");       
          break;

        case UP_RIGHT:
          setPixel(0,0);
          setPixel(1,0);
          setPixel(2,0);
          ReportData->LY = STICK_MIN;
          ReportData->LX = STICK_MAX; 
          Serial1.println("UP_RIGHT");
          break;
        case UP_LEFT:
          setPixel(9,0);
          setPixel(8,0);
          setPixel(7,0);
          ReportData->LY = STICK_MIN;
          ReportData->LX = STICK_MIN;
          Serial1.println("UP_LEFT");
          break;
        case DOWN_RIGHT:
          setPixel(3,0);
          setPixel(4,0);
          setPixel(5,0);
          ReportData->LY = STICK_MAX;
          ReportData->LX = STICK_MAX;
          Serial1.println("DOWN_RIGHT");
          break;
        case DOWN_LEFT:
          setPixel(6,0);
          setPixel(7,0);
          setPixel(8,0);
          ReportData->LY = STICK_MAX;
          ReportData->LX = STICK_MIN;
          Serial1.println("DOWN_LEFT");
          break;

        case A:
          setPixel(A,A);
          ReportData->Button |= SWITCH_A;
          Serial1.println("A");
          break;

        case B:
          setPixel(B,B);
          ReportData->Button |= SWITCH_B;
          Serial1.println("B");
          break;

        case R:
          setPixel(R,R);
          ReportData->Button |= SWITCH_R;
          Serial1.println("R");
          break;
          
        case L:
          setPixel(L,L);
          ReportData->Button |= SWITCH_L;
          Serial1.println("L");
          break;  
          
        case X:
          setPixel(X,X);
          ReportData->Button |= SWITCH_X;
          Serial1.println("X");
          break;
          
        case Y:
          setPixel(Y,Y);
          ReportData->Button |= SWITCH_Y;
          Serial1.println("Y");
          break;

       case MINUS:
          setPixel(MINUS,MINUS);
          ReportData->Button |= SWITCH_MINUS;
          Serial1.println("MINUS");
          break;
          
        case PLUS:
          setPixel(PLUS,PLUS);
          ReportData->Button |= SWITCH_PLUS;
          Serial1.println("PLUS");
          break;
          
        case THROW:
          setPixel(1,0);
          setPixel(2,0);
          setPixel(3,0);
          setPixel(R,R);
          ReportData->LY = STICK_MIN;       
          ReportData->Button |= SWITCH_R;
          Serial1.println("THROWWN");
          break;

        case TRIGGERS:
          setPixel(1,0);
          setPixel(8,0);
          ReportData->Button |= SWITCH_L | SWITCH_R;
          Serial1.println("TRIGGERS");
          break;

        default:
          CircuitPlayground.clearPixels();
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

