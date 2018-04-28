#include <SPI.h>
#include <EEPROM.h>
#include <LUFA.h>
#include "nintendo.h"
#include "program.h"
#include <Wire.h>

#define SDCardSupport

#ifdef SDCardSupport
#include <SD.h>
File myFile;

#endif

#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;
USB_JoystickReport_Input_t TempReport;
int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int portsval = 0;
int IsWriteing = 0;

bool report = false;

USB_JoystickReport_SDRec curentReport;
uint16_t loopCount = 0;

int Serialstepcount = 0;
bool SaveToEprom = false;



const int ClearButtonPin = 2;
const int SwtichButtonPin = 3;

const int PlayLed = 5;


command prossing;
bool boot = false;
command Serialstep[50];
// Main entry point.
void setup() {

  // We'll start by performing hardware and peripheral setup.
  SetupHardware();

  pinMode(ClearButtonPin, INPUT);
  digitalWrite(ClearButtonPin, HIGH);       // turn on pullup resistors

  pinMode(SwtichButtonPin, INPUT);
  digitalWrite(SwtichButtonPin, HIGH);       // turn on pullup resistors

  pinMode(PlayLed, OUTPUT);
  digitalWrite(PlayLed, LOW);       // turn off the LED

  // We'll then enable global interrupts for our use.
  GlobalInterruptEnable();

  // Once that's done, we'll enter an infinite loop.

  Serial1.begin(19200);

  Serial1.println("Hello, world?");
  LoadEEPROM();
  prossing.button = NOTHING;

#ifdef SDCardSupport
  Serial1.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial1.println("initialization failed!");
    return;
  }
  Serial1.println("initialization done.");
  readPref();

  HandalFileOpening("log.usb", true);
  if (myFile) {
    Serial1.println("Starting Recording..");
  }
#endif
  Serial1.println("prossing..");
}

void loop() {
  // We need to run our task to process and deliver data for our IN and OUT endpoints.

  HID_Task();
  USB_USBTask();
}

bool playingBack = false;

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
      if (!boot) {
        TempReport = runScript(ReportData, SetupStep, SetupStepSize);
      }
      else if (slideSwitch()) {
        clearButton();
        if (Serial1.available() > 0) {
          byte in = Serial1.read();
          if (in == '$') {
            byte Buffer[sizeof(USB_JoystickReport_Input_t)];
            int reportlen = Serial1.readBytes(Buffer, sizeof(USB_JoystickReport_Input_t));
            if (reportlen == sizeof(USB_JoystickReport_Input_t)) {
              memcpy(&TempReport, Buffer, sizeof(USB_JoystickReport_Input_t));
              report = true;
            } else {
              report = false;
            }
          } else if (in == '%') {
            byte Buffer[7];
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
            byte Buffer[100];
            int prossed = Serial1.readBytesUntil('#', Buffer, 100);
            prossesCommandSet(Buffer);
            if (SaveToEprom) {
              if ( prossed > 1023) {
              } else {
                EEPROM.write(0, prossed);
                for (int i = 1; i < prossed + 1; i++) {
                  EEPROM.write(i, Buffer[i]);
                }
              }
            }
          }
          if (report) {
            memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
          }

        }
        if (Serialstepcount == 0) {
          duration_count = 0;
          bufindex = 0;
        } else {
          TempReport = runScript(ReportData, Serialstep, Serialstepcount);
          memcpy(ReportData, &TempReport, sizeof(USB_JoystickReport_Input_t));
        }
      } else  {
#ifdef SDCardSupport
        PlaybackButton();
        if (playingBack) {
          if (loopCount > curentReport.reportDelay) {
            curentReport = getNextReport();
          }
          loopCount++;
          memcpy(ReportData, &curentReport.report, sizeof(USB_JoystickReport_Input_t));
        } else {
#endif

          //#############################################################################################
          //#############################################################################################
          //#############################################################################################
          //          this is the bit to update the script that will run at boot
          //          see program.h for sample FrogCoinestep and FrogCoinestepSize

          TempReport = runScript(ReportData, FrogCoinestep, FrogCoinestepSize);

          //#############################################################################################
          //#############################################################################################
          //#############################################################################################
#ifdef SDCardSupport
        }
#endif

      }
#ifdef SDCardSupport
      // Prepare to echo this report
      if ((TempReport == last_report) == false && playingBack == false && boot) {
        saveReportToSD(last_report);
        loopCount = 0;
      } else {
        loopCount++;
      }
#endif
      memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
      echoes = ECHOES;
      break;
  }
}


#ifdef SDCardSupport


void resetPlayBack() {
  loopCount = 0;
  if (myFile) {
    myFile.seek(0);
  }
}

void PlaybackButton() {
  if ( clearButton()) {
    while ( clearButton()) {
    }
    playingBack = !playingBack;
    if (playingBack) {
      resetPlayBack();
      digitalWrite(PlayLed, HIGH);
    } else {
      digitalWrite(PlayLed, LOW);
    }
  }
}

void readPref() {
  HandalFileOpening("pref.ini", false);
  if (myFile) {
    while (myFile.available()) {
      Serial1.write(myFile.read());
    }
  }
  HandalFileOpening("log.usb", false);
  if (myFile) {
    byte Buffer[sizeof(USB_JoystickReport_SDRec)];
    while (myFile.available()) {
      int reportlen = myFile.read(Buffer, sizeof(USB_JoystickReport_SDRec));
      if (reportlen == sizeof(USB_JoystickReport_SDRec)) {
        memcpy(&curentReport, Buffer, sizeof(USB_JoystickReport_SDRec));
        Serial1.print("reportDelay: ");
        Serial1.print(curentReport.reportDelay, DEC);
        Serial1.print("  reportButtons: ");
        Serial1.print(curentReport.report.Button, DEC);
        Serial1.print("  LX,LY: [");
        Serial1.print(curentReport.report.LX, DEC);
        Serial1.print(",");
        Serial1.print(curentReport.report.LY, DEC);
        Serial1.print("]  RX,RY: [");
        Serial1.print(curentReport.report.RX, DEC);
        Serial1.print(",");
        Serial1.print(curentReport.report.RY, DEC);
        Serial1.println( "]");

      }
    }
  }
}

void saveReportToSD(USB_JoystickReport_Input_t ReportData) {
  if (myFile) {
    USB_JoystickReport_SDRec thisReport;
    thisReport.report = ReportData;
    thisReport.reportDelay = loopCount;

    char b[sizeof(USB_JoystickReport_SDRec)];
    memcpy(b, &thisReport, sizeof(USB_JoystickReport_SDRec));
    if (myFile.write(b, sizeof(b)) > 0) {
      Serial1.println("saved:");
    } else {
      Serial1.println("Error writing");
    }
  }

}


USB_JoystickReport_SDRec getNextReport() {
  if (myFile) {
    USB_JoystickReport_SDRec thisReport;
    char b[sizeof(USB_JoystickReport_SDRec)];
    while (myFile.available()) {
      if (myFile.read(b, sizeof(b)) > 0) {
        memcpy(b, &thisReport, sizeof(USB_JoystickReport_SDRec));
      }
    }
  } else {
    Serial1.println("Error No File Open");
  }

}

void HandalFileOpening(String name, bool Writeing) {
  IsWriteing = -1;
  if (myFile) {
    myFile.close();
    Serial1.println("myFile.close()");
  }
  if (Writeing) {
    myFile = SD.open(name, FILE_WRITE);
    if (myFile) {
      IsWriteing = 1;
      Serial1.println(name + " Is Writeing");
    } else {
      Serial1.println(name + " Error opening for writeing");
    }
  } else {
    myFile = SD.open(name);
    if (myFile) {
      IsWriteing = 0;
      Serial1.println(name + " Is Reading");
    } else {
      Serial1.println(name + " Error opening for reading");
    }
  }
}
#endif


// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);
  // We can then initialize our hardware and peripherals, including the USB stack.

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


USB_JoystickReport_Input_t runScript(USB_JoystickReport_Input_t* const ReportData, command CommandStep[], int CommandStepSize) {
  if (bufindex < CommandStepSize) {
    digitalWrite(PlayLed, HIGH);
    prossing = CommandStep[bufindex];

    USB_JoystickReport_Input_t TempReport = CommandPROCESS(ReportData, prossing);
    duration_count++;
    if (duration_count > prossing.duration)
    {
      bufindex++;
      duration_count = 0;
    }
    return TempReport;
  } else if ( !boot) {
    bufindex = 0;
    duration_count = 0;
    boot = true;
  }
  digitalWrite(PlayLed, LOW);
  TempReport.LX = STICK_CENTER;
  TempReport.LY = STICK_CENTER;
  TempReport.RX = STICK_CENTER;
  TempReport.RY = STICK_CENTER;
  TempReport.HAT = HAT_CENTER;
  state = BREATHE;
  return TempReport;
}

void prossesCommandSet(byte Buffer[]) {
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

void LoadEEPROM() {
  int CommandLen = EEPROM.read(0);
  byte Buffer[CommandLen];
  if (CommandLen > 0) {
    memset(Buffer, 0, sizeof(Buffer));
    for (int i = 0; i < CommandLen; i++) {
      Buffer[i] = EEPROM.read(i + 1);
    }
    prossesCommandSet(Buffer);
    if (Serialstepcount > 0) {
      //good
    } else {
      //error
    }
  } else {
    //error
  }
  delay(200);

}

bool slideSwitch() {
  return digitalRead(SwtichButtonPin);
}

bool clearButton() {
  if (digitalRead(ClearButtonPin) == 0) {
    Serialstepcount = 0;
    report = false;
    return true;
  }
  return false;
}


