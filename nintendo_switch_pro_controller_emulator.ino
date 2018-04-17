#include <EEPROM.h>
#include <LUFA.h>
#include "nintendo.h"
#include "program.h"
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>


#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;
USB_JoystickReport_Input_t TempReport;
int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int portsval = 0;

command prossing;
bool boot = false;
command Serialstep[100];
// Main entry point.
void setup() {

  // We'll start by performing hardware and peripheral setup.
  SetupHardware();


  // We'll then enable global interrupts for our use.
  GlobalInterruptEnable();



  CircuitPlayground.begin();
  // Once that's done, we'll enter an infinite loop.

  Serial1.begin(19200);
  Serial1.println("Hello, world?");
  prossing.button = NOTHING;
}

void loop() {
  // We need to run our task to process and deliver data for our IN and OUT endpoints.


  CircuitPlayground.redLED(HIGH);
  HID_Task();
  CircuitPlayground.redLED(LOW);
  USB_USBTask();

}



// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);
  // We can then initialize our hardware and peripherals, including the USB stack.

#ifdef ALERT_WHEN_DONE
  // Both PORTD and PORTB will be used for the optional LED flashing and buzzer.
#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
PORTD will toggle when printing is done.
  DDRD  = 0xFF; //Teensy uses PORTD
  PORTD =  0x0;
  //We'll just flash all pins on both ports since the UNO R3
  DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
  PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
#endif
  // The USB stack should be initialized last.
  USB_Init(1);
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
  // We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
  // We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
  bool ConfigSuccess = true;

  // We setup the HID report endpoints.
  ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
  ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

  // We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
  // We can handle two control requests: a GetReport and a SetReport.

  // Not used here, it looks like we don't receive control request from the Switch.
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
  // If the device isn't connected and properly configured, we can't do anything here.
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  // We'll start with the OUT endpoint.
  Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
  // We'll check to see if we received something on the OUT endpoint.
  if (Endpoint_IsOUTReceived())
  {
    // If we did, and the packet has data, we'll react to it.
    if (Endpoint_IsReadWriteAllowed())
    {
      // We'll create a place to store our data received from the host.
      USB_JoystickReport_Output_t JoystickOutputData;
      // We'll then take in that data, setting it up in our storage.
      while (Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
      // At this point, we can react to this data.

      // However, since we're not doing anything with this data, we abandon it.
    }
    // Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
    Endpoint_ClearOUT();
  }

  // We'll then move on to the IN endpoint.
  Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
  // We first check to see if the host is ready to accept data.
  if (Endpoint_IsINReady())
  {
    // We'll create an empty report.
    USB_JoystickReport_Input_t JoystickInputData;
    // We'll then populate this report with what we want to send to the host.
    GetNextReport(&JoystickInputData);
    // Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
    while (Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
    // We then send an IN packet on this endpoint.
    Endpoint_ClearIN();
  }
}
int duration_count = 0;

USB_JoystickReport_Input_t runScript(USB_JoystickReport_Input_t* const ReportData, command CommandStep[], int CommandStepSize) {
  prossing = CommandStep[bufindex];

  USB_JoystickReport_Input_t TempReport = CommandPROCESS(ReportData, prossing);
  duration_count++;
  if (duration_count > prossing.duration)
  {
    bufindex++;
    duration_count = 0;
  }
  if (bufindex > CommandStepSize - 1)
  {
    bufindex = 0;
    duration_count = 0;
    TempReport.LX = STICK_CENTER;
    TempReport.LY = STICK_CENTER;
    TempReport.RX = STICK_CENTER;
    TempReport.RY = STICK_CENTER;
    TempReport.HAT = HAT_CENTER;
    state = BREATHE;
    if (!boot) {
      boot = true;
    }
  }
  return TempReport;
}
const int bSize = 100;
char Buffer[bSize];  // Serial buffer
int Serialstepcount = 0;
bool report = false;
bool SaveToEprom = false;



void prossesCommandSet() {
  byte _sentencePos = 0;
  Serialstepcount = 0;
  SaveToEprom = false;
  while (1)
  {
    if (Buffer[_sentencePos] == NULL) {
      break;
    }
    if (Buffer[_sentencePos] == '!') {
      SaveToEprom = true;
      _sentencePos++;
    }
    if (Buffer[_sentencePos] == '$')
    {
      _sentencePos++;
      byte button = Buffer[_sentencePos];
      prossing.button = button;
      _sentencePos++;
      byte duration = Buffer[_sentencePos];
      prossing.duration = duration;
      Serialstep[Serialstepcount] = prossing;
      Serialstepcount++;
      _sentencePos++;
    } else {
      _sentencePos++;
    }
  }
}
// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

  // Prepare an empty report
  memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
  ReportData->LX = STICK_CENTER;
  ReportData->LY = STICK_CENTER;
  ReportData->RX = STICK_CENTER;
  ReportData->RY = STICK_CENTER;
  ReportData->HAT = HAT_CENTER;

  // Repeat ECHOES times the last report
  if (echoes > 0)
  {
    memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
    echoes--;
    return;
  }

  // States and moves management
  switch (state)
  {

    case SYNC_CONTROLLER:
      state = BREATHE;
      break;

    case BREATHE:
      state = PROCESS;
      break;

    case PROCESS:

      if (CircuitPlayground.slideSwitch()) {
        if (Serial1.available() > 0) {
          byte in = Serial1.read();
          if (in == '$') {
            Serialstepcount = 0;
            prossing.button = Serial1.read();
            Serial1.write('.');
            TempReport = CommandPROCESS(ReportData, prossing);
            memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
          } else if (in == '@') {
            int reportlen = Serial1.readBytes(Buffer, 7);
            if (reportlen == 7) {
              TempReport.HAT = Buffer[2];
              TempReport.LX = Buffer[3];
              TempReport.LY = Buffer[4];
              TempReport.RX = Buffer[5];
              TempReport.RY = Buffer[6];
              TempReport.Button = ((uint16_t)Buffer[1] << 8) | Buffer[0];
              report = true;
            } else {
              report = false;
            }
          } else if (in == '#') {
            memset(Buffer, 0, sizeof(Buffer));
            int prossed = Serial1.readBytesUntil('#', Buffer, bSize);
            prossesCommandSet();
            if (SaveToEprom) {
              if ( prossed > 1023) {
                flashRed();
              } else {
                flashGreen();
                EEPROM.write(0, prossed);
                for (int i = 1; i < prossed + 1; i++) {
                  EEPROM.write(i, Buffer[i]);
                }
              }
              delay(200);
              flash(0);
            }
          }
        }
        if ( CircuitPlayground.leftButton()) {
          Serialstepcount = 0;
          report = false;
          flashRed();
          delay(200);
          flash(0);
        }
        if ( CircuitPlayground.rightButton()) {
          int CommandLen = EEPROM.read(0);
          if (CommandLen > 0) {
            flashGreen();
            memset(Buffer, 0, sizeof(Buffer));
            for (int i = 0; i < CommandLen; i++) {
              Buffer[i] = EEPROM.read(i + 1);
            }
            prossesCommandSet();
            delay(200);
            flash(0);
            delay(200);
            if (Serialstepcount > 0) {
              flashGreen();
            } else {
              flashRed();
            }
          } else {
            flashRed();
          }
          delay(200);
          flash(0);
        }
        if (Serialstepcount == 0) {
          duration_count = 0;
          bufindex = 0;
        } else {
          TempReport = runScript(ReportData, Serialstep, Serialstepcount);
          memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
        }
        if (report) {
          memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
        }
        boot = false;
      } else {
        if (!boot) {
          TempReport = runScript(ReportData, SetupStep, SetupStepSize);
        } else {
          TempReport = runScript(ReportData, FrogCoinestep, FrogCoinestepSize);
        }
        memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
      }
      CircuitPlayground.strip.show();
      break;
  }
  // Prepare to echo this report
  memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
  echoes = ECHOES;

}

