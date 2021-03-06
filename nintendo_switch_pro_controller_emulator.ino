#include <SPI.h>
#include <EEPROM.h>
#include <LUFA.h>
#include "nintendo.h"
#include "program.h"
#include <Wire.h>

//Comment this line out if you do not want SD card support.
#define SDCardSupport

#ifdef SDCardSupport
#include <SD.h>
File myFile;
void printDirectory(File dir, int numTabs);
File root;
#endif

const int ClearButtonPin = 4;
const int SwtichButtonPin = 5;

const int PlayLed = 6;
const int ErrorLed = 7;


// Main entry point.
void setup() {

  // We'll start by performing hardware and peripheral setup.
  SetupHardware();
  EEPROM.write(EEPROMCommandStartPos, 0);
  pinMode(ClearButtonPin, INPUT);
  digitalWrite(ClearButtonPin, HIGH);       // turn on pullup resistors

  pinMode(SwtichButtonPin, INPUT);
  digitalWrite(SwtichButtonPin, HIGH);       // turn on pullup resistors

  pinMode(PlayLed, OUTPUT);
  digitalWrite(PlayLed, LOW);       // turn off the LED
  pinMode(ErrorLed, OUTPUT);
  digitalWrite(ErrorLed, LOW);       // turn off the LED

  // We'll then enable global interrupts for our use.
  GlobalInterruptEnable();

  // Once that's done, we'll enter an infinite loop.
  getDefaultReport(&last_report);

  Serial1.begin(19200);

  voidDebugPrintln("Hello, world?");
  prossing.button = NOTHING;

#ifdef SDCardSupport
  voidDebugPrint("Initializing SD card...");
  if (!SD.begin(10)) {
    voidDebugPrintln("initialization failed!");
    flashErrot();
    flashErrot();
    flashErrot();
    flashErrot();
    return;
  }
  voidDebugPrintln("initialization done.");
  //readPref();
  root = SD.open("/");
  if (slideSwitch()) {
    startNewRecording();
  }


#endif
  voidDebugPrintln("prossing..");
}

void loop() {
  // We need to run our task to process and deliver data for our IN and OUT endpoints.

  HID_Task();
  USB_USBTask();
}

bool LasteSwitch = true;

// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

  // Prepare an empty report
  memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
  ReportData->LX = STICK_CENTER;
  ReportData->LY = STICK_CENTER;
  ReportData->RX = STICK_CENTER;
  ReportData->RY = STICK_CENTER;
  ReportData->HAT = HAT_CENTER;


  memcpy(&curentReport.report, &last_report, sizeof(USB_JoystickReport_Input_t));

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
        runScript(SetupStep, SetupStepStepSize);
        playingBack = true;
      }
      else if (slideSwitch()) {
        if (LasteSwitch == false) {
#ifdef SDCardSupport
          startNewRecording();
#endif
          LasteSwitch = true;
        }

        digitalWrite(PlayLed, LOW);       // turn off the LED
        if ( clearButton()) {
          while ( clearButton()) {
          }
          getDefaultReport(&curentReport.report);
          if (Serialstepcount > 0) {
            Serialstepcount = -1;
          }
          else if (Serialstepcount != 0) {
            Serialstepcount = 0;
            bufindex = 0;
            duration_count = 0;
          } else {
            Serialstepcount = -1;
          }
#ifdef SDCardSupport
          startNewRecording();
#endif
        }

        if (Serial1.available() > 0) {
          Serialstepcount = -1;

          byte in = Serial1.read();
          if (in == '$') {
            byte Buffer[sizeof(USB_JoystickReport_Input_t)];
            int reportlen = Serial1.readBytes(Buffer, sizeof(USB_JoystickReport_Input_t));
            if (reportlen == sizeof(USB_JoystickReport_Input_t)) {
              memcpy(&curentReport.report, Buffer, sizeof(USB_JoystickReport_Input_t));
            } else {
              voidDebugPrintln("Error Reading Command");
              flashErrot();
              flashErrot();
              flashErrot();
            }

          } else if (in == '#') {
            byte Buffer[EEPROMCommandSize * 2];
            int prossed = Serial1.readBytesUntil('#', Buffer, 100);
            prossesCommandSet(Buffer);
            if (SaveToEprom) {
              if ( prossed > 1023) {
              } else {
                EEPROM.write(EEPROMCommandStartPos, prossed);
                for (int i = 1; i < prossed + 1; i++) {
                  EEPROM.write(EEPROMCommandStartPos + i, Buffer[i]);
                }
              }
            }
          }

        } else {
          if (Serialstepcount == 0) {
            //LoadEEPROM(); // will atemnt to load the Plogram stored on EEPROM
            //if (Serialstepcount == 0) {
            //voidDebugPrintln("Running Default Falsh Program");
            //#############################################################################################
            //#############################################################################################
            //#############################################################################################
            //          this is the bit to update the script that will run at boot
            //          see program.h for sample FrogCoinestep

            runScript(FrogCoinestep, FrogCoinestepSize);


            //#############################################################################################
            //#############################################################################################
            //#############################################################################################
            //} else {
            //voidDebugPrintln("Loaded EEPROM Program");
            //}
            playingBack = true;
          } else if (Serialstepcount > 0) {
            runScript(Serialstep, Serialstepcount);
            playingBack = true;
          } else {
            playingBack = false;
          }
        }
      } else  {
#ifdef SDCardSupport
        if (LasteSwitch == true) {
          if (reportCount > 0) {
            saveReportToSD(last_report);
          }
          reportCount = 0;
          int lognum = FindLastLog(-1);
          while (HandalFileOpening(LogNumberToLogName(lognum), false) < 8) {
            lognum--;
            if (lognum < 1) {
              break;
            }
          }
          LasteSwitch = false;
        }
        PlaybackButton();
        if (playingBack) {
          if (loopCount > (curentReport.reportDelay)) {
            getNextUSBReport();
            loopCount = 0;
          }
          loopCount++;
        } else {
          getDefaultReport(&last_report);
          getDefaultReport(&curentReport.report);
        }

#endif

      }
#ifdef SDCardSupport
      // Prepare to echo this report
      if ((curentReport.report == last_report) == false && playingBack == false && boot) {
        saveReportToSD(last_report);
        loopCount = 0;
        printReport(&curentReport.report, "saveReportToSD");
        reportCount ++;
      } else {
        loopCount++;
      }
#endif
      memcpy(ReportData, &curentReport.report, sizeof(USB_JoystickReport_Input_t));
      memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));

      echoes = ECHOES;
      break;
  }
}


void flashErrot() {
  digitalWrite(ErrorLed, HIGH);
  delay(1000);
  digitalWrite(ErrorLed, LOW);
  delay(200);
}


#ifdef SDCardSupport



int FindLastLog(int befor) {
  int logNumber = -1;
  root.seek(0);
  while (true) {
    File entry =  root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    //voidDebugPrint(entry.name());
    String name  = String(entry.name());
    if (name.startsWith("LOG") && name.endsWith(".USB")) {
      String curentLog = name.substring(4, name.indexOf(".USB"));

      int templogNumber = curentLog.toInt();
      if (befor > 0) {
        if ((templogNumber < befor) && (entry.size() > 8) && (templogNumber > logNumber)) {
          logNumber = templogNumber;
        }
      } else if ((entry.size() > 8) && (templogNumber > logNumber)) {
        logNumber = templogNumber;
      }

    } else {
      //voidDebugPrintln("  No Matchg");
    }
    entry.close();
  }
  return logNumber;
}



String LogNumberToLogName(int LogNumber) {
  return  "log_" + String(LogNumber) + ".usb";
}

void startNewRecording() {
  if (myFile) {
    if (reportCount > 0) {
      saveReportToSD(last_report);
    }
    myFile.close();
    voidDebugPrintln(String(myFile.name()) + " closed");
  }
  reportCount = 0;
  int logNumber = FindLastLog(-1);
  logNumber++;
  loopCount = 0;
  HandalFileOpening(LogNumberToLogName(logNumber), true);
  if (myFile) {
    voidDebugPrintln("Starting Recording..");
  } else {
    voidDebugPrintln("Error Starting Recording..");
    flashErrot();
    flashErrot();
  }
  getDefaultReport(&last_report);
  getDefaultReport(&curentReport.report);

}



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
      printUSBCommands();
      resetPlayBack();
      digitalWrite(PlayLed, HIGH);
      voidDebugPrintln("Playback Started:");
      getNextUSBReport();
    } else {
      digitalWrite(PlayLed, LOW);
      voidDebugPrintln("Playback Stoped:");
    }
  }
}


void printUSBCommands() {
  byte Buffer[sizeof(USB_JoystickReport_SDRec)];
  while (myFile.available()) {
    int reportlen = myFile.read(Buffer, sizeof(USB_JoystickReport_SDRec));
    if (reportlen == sizeof(USB_JoystickReport_SDRec)) {
      memcpy(&curentReport, Buffer, sizeof(USB_JoystickReport_SDRec));
      voidDebugPrint("reportDelay: ");
      voidDebugPrint(curentReport.reportDelay, DEC);
      voidDebugPrint("  reportButtons: ");
      voidDebugPrint(curentReport.report.Button, DEC);
      voidDebugPrint("  LX,LY: [");
      voidDebugPrint(curentReport.report.LX, DEC);
      voidDebugPrint(",");
      voidDebugPrint(curentReport.report.LY, DEC);
      voidDebugPrint("]  RX,RY: [");
      voidDebugPrint(curentReport.report.RX, DEC);
      voidDebugPrint(",");
      voidDebugPrint(curentReport.report.RY, DEC);
      voidDebugPrintln( "]");

    } else {
      voidDebugPrintln( "reportlen Under");
      flashErrot();
    }
  }
  myFile.seek(0);
}

void readPref() {
  HandalFileOpening("pref.ini", false);
  if (myFile) {
    while (myFile.available()) {
      Serial1.write(myFile.read());
    }
    voidDebugPrintln("--------------------:");
    voidDebugPrintln("");
  }
  /*
    HandalFileOpening("log.usb", false);
    if (myFile) {
    byte Buffer[sizeof(USB_JoystickReport_SDRec)];
    while (myFile.available()) {
      int reportlen = myFile.read(Buffer, sizeof(USB_JoystickReport_SDRec));
      if (reportlen == sizeof(USB_JoystickReport_SDRec)) {
        memcpy(&curentReport, Buffer, sizeof(USB_JoystickReport_SDRec));
        voidDebugPrint("reportDelay: ");
        voidDebugPrint(curentReport.reportDelay, DEC);
        voidDebugPrint("  reportButtons: ");
        voidDebugPrint(curentReport.report.Button, DEC);
        voidDebugPrint("  LX,LY: [");
        voidDebugPrint(curentReport.report.LX, DEC);
        voidDebugPrint(",");
        voidDebugPrint(curentReport.report.LY, DEC);
        voidDebugPrint("]  RX,RY: [");
        voidDebugPrint(curentReport.report.RX, DEC);
        voidDebugPrint(",");
        voidDebugPrint(curentReport.report.RY, DEC);
        voidDebugPrintln( "]");

      }
    }
    }
  */
}

void saveReportToSD(USB_JoystickReport_Input_t ReportData) {
  if (myFile) {
    USB_JoystickReport_SDRec thisReport;
    memcpy(&thisReport.report, &ReportData, sizeof(USB_JoystickReport_SDRec));
    thisReport.report = ReportData;
    thisReport.reportDelay = loopCount;

    char b[sizeof(USB_JoystickReport_SDRec)];
    memcpy(b, &thisReport, sizeof(USB_JoystickReport_SDRec));
    if (myFile.write(b, sizeof(b)) > 0) {
      voidDebugPrintln("saved: USB report");
    } else {
      voidDebugPrintln("Error writing");
      flashErrot();
    }
  }

}


void getNextUSBReport() {
  //curentReport.report = getDefaultReport();
  //curentReport.reportDelay = 1;

  if (myFile) {
    byte b[sizeof(USB_JoystickReport_SDRec)];

    while (myFile.available()) {
      if (myFile.read(b, sizeof(b)) == sizeof(USB_JoystickReport_SDRec)) {
        memcpy(&curentReport, b, sizeof(USB_JoystickReport_SDRec));
        printReport(&curentReport.report, "Read Report: ");
        break;
      }
    }

  } else {
    //voidDebugPrintln("Error No File Open");

  }
}

int HandalFileOpening(String name, bool Writeing) {
  IsWriteing = -1;
  if (myFile) {
    myFile.close();
    voidDebugPrintln(String(myFile.name()) + " closed");
  }
  if (Writeing) {
    myFile = SD.open(name, FILE_WRITE);
    if (myFile) {
      IsWriteing = 1;
      voidDebugPrintln(name + " Is Writeing");
    } else {
      voidDebugPrintln(name + " Error opening for writeing");
      flashErrot();
      flashErrot();
      flashErrot();
      flashErrot();
      flashErrot();
    }
    return 0;
  } else {
    myFile = SD.open(name);
    if (myFile) {
      IsWriteing = 0;
      voidDebugPrintln(name + " Is Reading");
      return myFile.size();
    } else {
      voidDebugPrintln(name + " Error opening for reading");
      flashErrot();
      flashErrot();
      flashErrot();
      flashErrot();
      flashErrot();
      return 0;
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


void runScript(command CommandStep[], int CommandStepSize) {

  if (bufindex < CommandStepSize) {
    digitalWrite(PlayLed, HIGH);
    prossing = CommandStep[bufindex];
    CommandPROCESS(&curentReport.report, prossing);
    duration_count++;
    if (duration_count > prossing.duration)
    {
      bufindex++;
      duration_count = 0;
    }
    return;
  } else if ( !boot) {
    boot = true;
    getDefaultReport(&last_report);
    getDefaultReport(&curentReport.report);
  }
  bufindex = 0;
  duration_count = 0;

  digitalWrite(PlayLed, LOW);
  getDefaultReport(&curentReport.report);
  state = BREATHE;
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
  int CommandLen = EEPROM.read(EEPROMCommandStartPos);
  byte Buffer[CommandLen];
  if (CommandLen > 0) {
    memset(Buffer, 0, sizeof(Buffer));
    for (int i = 0; i < CommandLen; i++) {
      Buffer[i] = EEPROM.read(EEPROMCommandStartPos + i + 1);
    }
    prossesCommandSet(Buffer);
    if (Serialstepcount > 0) {
      //good
    } else {
      flashErrot();
    }
  } else {
    flashErrot();
  }
  delay(200);

}

bool slideSwitch() {
  return digitalRead(SwtichButtonPin);
}

bool clearButton() {
  if (digitalRead(ClearButtonPin) == 0) {
    return true;
  }
  return false;
}


