/*  
 *  Firmware for the ADB-BUS converter. Written for the STM32F103
 *  using an external resistor (5 kOhm) between A0 and A1. The data
 *  line is therefore working on 3.3 Volts, but the keyboard seems
 *  to work perfectly fine this way (even though it's specified for
 *  5V, but using 3.3 still gives enough margin for the digital
 *  signal to be distinguished well).

 *
 *  It provides all the basic functionalities that are required
 *  to use a keyboard. A couple notes:
 *
 *  - The option buttons (control, alt, shift, command) are always
 *    left, since the keyboard does not have separate keycodes. The
 *    right alt key can be separated though. The key-up has a second
 *    byte containing the same keycode. The left alt does not have
 *    a second byte. The problem is then not being able to send a
 *    proper key-down, since the left/right is not known until the
 *    key is released.
 * 
 *  - The power button is used as an enable/disable for the whole
 *    keyboard.
 *
 *  Firmware 0: Joe Simon. September the 16th, 2018.
 *  Firmware 1: Joe Simon. November 2019.
 */

#define LED                          PC13
#define DATA_PIN_OUTPUT              PA0
#define DATA_PIN_INPUT               PA1
#define POWER_BUTTON_PIN             PA2
#define MESSAGE_DELAY_MS             5
#define USE_DIRECT_DATA_PIN_ACCESS   true
#define USB_DEBUG_ENABLE             false
#define INITIAL_SCROLL_LOCK          false
#define INITIAL_CAPS_LOCK            false
#define INITIAL_NUMS_LOCK            false
#define ON                           LOW
#define OFF                          HIGH
#define ID_VENDOR                    0x6969
#define ID_PRODUCT                   0x0069
#define MANUFACTURER                 "Joe Simon"
#define PRODUCT                      "ADB-USB Converter"
#define SERIALNUMBER                 "01234567891011121314"

// ---- Includes

#include <USBComposite.h>
#include "adb_keyboard_mapping.h"
#if USB_DEBUG_ENABLE
#include "adb_keyboard_mapping_mock.hpp"
#endif

// ---- Global variables and macros

#if USB_DEBUG_ENABLE
USBSerial & USBSerialDebug = Serial;
#endif

USBHID HID;
HIDKeyboard USBKeyboard(HID);
HIDMouse USBMouse(HID); 

// To store the current status of the LEDs
uint16 currentStatus = 0xFF;

bool capsLockStatus   = INITIAL_CAPS_LOCK;
bool numLockStatus    = INITIAL_NUMS_LOCK;
bool scrollLockStatus = INITIAL_SCROLL_LOCK;

#define setCapsLockLED(value)   setStatusBit(STATUS_BIT_LED_CAPS_LOCK,   !value)
#define setNumsLockLED(value)   setStatusBit(STATUS_BIT_LED_NUM_LOCK,    !value)
#define setScrollLockLED(value) setStatusBit(STATUS_BIT_LED_SCROLL_LOCK, !value)

// ---- Functions

void attention();
void sendByte(byte data);
void setStatusBytes(uint16 statusBytes);
void setStatusBit(uint16 statusBit, bool value);
void processKeyCodes(byte data1, byte data2);
void setDataLow(uint32 delayTime);
void setDataHigh(uint32 delayTime);
void cellZero();
void cellOne();
void sendCommandByte(byte data);
bool powerButtonPushed();
bool receiveBit(byte * readBit);
bool lineStatus();
bool waitLineLow(uint16 timeout);
bool receiveData(uint16 * outputData);
void printBinaryByte(byte data);

void setStatusBytes(uint16 statusBytes)
{
#if USB_DEBUG_ENABLE
  USBSerialDebug.print("Sending status byte: ");
  USBSerialDebug.println(statusBytes, BIN);
#endif
  // Attention
  attention();
  
  // Start bit (1)
  cellOne();

  // Send command byte
  sendByte(COMMAND_KEYBOARD_SET_STATUS);

  // Stop bit (zero)
  cellZero();

  // Delay 200 us
  delayMicroseconds(200);

  // Start bit (1)
  cellOne();

  // Two data bytes
  sendByte(statusBytes >> 8);   // High
  sendByte(statusBytes & 0xFF); // Low

  // Stop bit(0)
  cellZero();
}

void setStatusBit(uint16 statusBit, bool value)
{
  if (value)
    bitSet(currentStatus, statusBit);
  else
    bitClear(currentStatus, statusBit);

  setStatusBytes(currentStatus);
}

bool isNumericKeypad(byte data)
{
  return (data == ADB_KEY_NUM_0        ||
          data == ADB_KEY_NUM_1        ||
          data == ADB_KEY_NUM_2        ||
          data == ADB_KEY_NUM_3        ||
          data == ADB_KEY_NUM_4        ||
          data == ADB_KEY_NUM_5        ||
          data == ADB_KEY_NUM_6        ||
          data == ADB_KEY_NUM_7        ||
          data == ADB_KEY_NUM_8        ||
          data == ADB_KEY_NUM_9        ||
          data == ADB_KEY_NUM_POINT    ||
          data == ADB_KEY_NUM_EQUAL    ||
          data == ADB_KEY_NUM_SLASH    ||
          data == ADB_KEY_NUM_ASTERISK ||
          data == ADB_KEY_NUM_MINUS    ||
          data == ADB_KEY_NUM_PLUS     ||
          data == ADB_KEY_NUM_ENTER
          );
}

volatile unsigned char ledScrollingCounter = 0;
volatile bool globalEnabled = true;
volatile uint16 savedStatus;

void processKeyCodes(byte data1, byte data2)
{
  // First bit is the up/down flag
  bool _keyUp1 = (data1 & 0x80);
  bool _keyUp2 = (data2 & 0x80);

  // "Caps-lock"
  if ((data1 & 0x7F) == ADB_KEY_CAPS_LOCK)
  {
    // The "auto detection" mode. The status of the host LED is
    // checked and the key is pushed until the desired status is
    // achieved. This way it will always be correct for both mac
    // and pc (*fingers crossed*).
    capsLockStatus = !_keyUp1;
    int counter = 0;
    while (isHostCapsLockLEDOn() != capsLockStatus && counter < 2)
    {
      // Try a key down
      USBKeyboard.press(KEY_CAPS_LOCK);
      if (isHostCapsLockLEDOn() == capsLockStatus) break;

      // Try a key up
      delay(150);
      USBKeyboard.release(KEY_CAPS_LOCK);      
      if (isHostCapsLockLEDOn() == capsLockStatus) break;
      delay(150);

      counter ++;
    }
    setCapsLockLED(isHostCapsLockLEDOn());
    return;
  }

  // "Num-lock" and "Clear"
  else if ((data1 & 0x7F) == ADB_KEY_NUM_LOCK)
  {
    if (!_keyUp1)
    {
      USBKeyboard.pressKeycode(HID_KEYCODE_NUMLOCK);
    }
    else
    {
      USBKeyboard.releaseKeycode(HID_KEYCODE_NUMLOCK);
    }
    numLockStatus = isHostNumLockLEDOn();
    setNumsLockLED(numLockStatus);
    return;
  }


  else if ((data1 & 0x7F) == ADB_KEY_SCROLL_LOCK)
  {
    scrollLockStatus = !scrollLockStatus;
    if (!_keyUp1)
    {
      USBKeyboard.press(HID_KEYCODE_SCROLLLOCK);
    }
    else
    {
      USBKeyboard.release(HID_KEYCODE_SCROLLLOCK);
    }
    scrollLockStatus = isHostScrollLockLEDOn();
    setScrollLockLED(scrollLockStatus);
    return;
  }


  // The power button enables / disables the whole keyboard
  else if (data1 == ADB_KEY_POWER_BUTTON &&
           data2 == ADB_KEY_POWER_BUTTON)
  {
    // There's only key-up (no key-down) for the power button when read this way
    globalEnabled = !globalEnabled;
    return;
  }

#if USB_DEBUG_ENABLE

  if(_keyUp1)
    USBSerialDebug.print("1-UP: ");
  else
    USBSerialDebug.print("1-DN: ");
  USBSerialDebug.print(adb_ascii_map_mock[data1 & 0x7F]);
  USBSerialDebug.print(" - ");
  if(_keyUp2)
    USBSerialDebug.print("2-UP: ");
  else
    USBSerialDebug.print("2-DN: ");
  USBSerialDebug.println(adb_ascii_map_mock[data2 & 0x7F]);
  return;

#else

  if(!globalEnabled)
    return;

  // Remove the up/down flag
  data1 = data1 & 0x7F;
  data2 = data2 & 0x7F;

  // If it's anything from the numeric-keypad, use the 
  // HID keycodes directly. This allows the host to distinguish
  // between the numbers above the letters and the numbers in the
  // numeric-keypad. This also allows PC hosts to use the "num-lock"
  // such that it becomes the alternate keymap with arrows and other
  // stuff (who actually needs that?).
  if (isNumericKeypad(data1))
  {
    if(_keyUp1)
    {
      USBKeyboard.releaseKeycode(adb_ascii_map[data1]);
    }
    else
    {
      USBKeyboard.pressKeycode(adb_ascii_map[data1]);
    }
  }
  else
  {
    // It's not a special key (modifiers, locks, power button) and
    // it's not a num-key.
    if(_keyUp1)
    {
      USBKeyboard.release(adb_ascii_map[data1]);
    }
    else
    {
      USBKeyboard.press(adb_ascii_map[data1]);
    }
  }

  if (isNumericKeypad(data2))
  {
    if(_keyUp2)
    {
      USBKeyboard.releaseKeycode(adb_ascii_map[data2]);
    }
    else
    {
      USBKeyboard.pressKeycode(adb_ascii_map[data2]);
    }
  }
  else
  {
    // It's not a special key (modifiers, locks, power button) and
    // it's not a num-key.
    if(_keyUp2)
    {
      USBKeyboard.release(adb_ascii_map[data2]);
    }
    else
    {
      USBKeyboard.press(adb_ascii_map[data2]);
    }
  }

#endif
}

// For the data line pin toggling
void setDataLow(uint32 delayTime)
{
#if USE_DIRECT_DATA_PIN_ACCESS
  PIN_MAP[DATA_PIN_OUTPUT].gpio_device->regs->BSRR = 
    (1U << PIN_MAP[DATA_PIN_OUTPUT].gpio_bit) << (16 * 1);
#else
  digitalWrite(DATA_PIN_OUTPUT, 1);
#endif
  delayMicroseconds(delayTime);
}

// For the data line pin toggling
void setDataHigh(uint32 delayTime)
{
#if USE_DIRECT_DATA_PIN_ACCESS
  PIN_MAP[DATA_PIN_OUTPUT].gpio_device->regs->BSRR = 
    (1U << PIN_MAP[DATA_PIN_OUTPUT].gpio_bit) << (16 * 0);
#else
  digitalWrite(DATA_PIN_OUTPUT, 0);
#endif
  delayMicroseconds(delayTime);
}

void cellZero()
{
  // Down long time, up short time
  setDataLow(64);
  setDataHigh(35);
}

void cellOne()
{
  // Down long time, up short time
  setDataLow(35);
  setDataHigh(64);
}

void sendByte(byte data)
{
  // Data byte
  for(int i = 0; i < 8; i++)
  {
    if(data & 0b10000000)
    {
      cellOne();
    }
    else
    {
      cellZero();
    }
    data = data << 1;
  }
}

void attention()
{
  // Attention (-35 for the low part of the start bit)
  setDataLow(800-35);  
}

void sendCommandByte(byte data)
{
  attention();

  // Start bit
  cellOne();

  sendByte(data);

  // Stop bit
  cellZero();
}

bool powerButtonPushed()
{
#if USE_DIRECT_DATA_PIN_ACCESS
  return !((PIN_MAP[POWER_BUTTON_PIN].gpio_device)->regs->IDR & BIT(POWER_BUTTON_PIN & 0x0F));
#else
  return !digitalRead(POWER_BUTTON_PIN); 
#endif
}

bool receiveBit(byte * readBit)
{
  // Wait for the low part
  if(!waitLineLow(60))
    return false;

  // Wait to get to the middle of the bit
  delayMicroseconds(50);

  // Check line level in the middle of the bit
  *readBit = (byte)lineStatus();

  // Wait until the line goes up again if it's a zero
  // otherwise it loses the "sync", since this program
  // is based on the line going down to start receiving
  // a bit
  if(*readBit == 0)
    while(!lineStatus()) {}

  return true;
}

bool lineStatus()
{
#if USE_DIRECT_DATA_PIN_ACCESS
  return (PIN_MAP[DATA_PIN_INPUT].gpio_device)->regs->IDR & BIT(DATA_PIN_INPUT & 0x0F);
#else
  return digitalRead(DATA_PIN_INPUT);
#endif
}

bool waitLineLow(uint16 timeout)
{
  // Wait until the line is low (with a timeout)
  while(lineStatus() && timeout)
  {
    delayMicroseconds(1);
    timeout--;
  }
  return timeout > 0;
}

bool receiveData(uint16 * outputData)
{
  byte _stopBit = 1, _tempBit;
  *outputData = 0;

  // -- Start bit --
  if(!waitLineLow(300))
    return false;

  delayMicroseconds(50);
  if(!lineStatus())
    return false;

  // -- Data -- (Receive 16 bits)
  for(int i = 15; i >= 0; i--)
  {
    if(!receiveBit(&_tempBit))
      return false;

    *outputData += ((uint16)_tempBit) << i;
  }

  // Wait for stop bit
  if(!receiveBit(&_stopBit))
    return false;    

  // Check stop bit
  return _stopBit == 0;
}

void printBinaryByte(byte data)
{
#if USB_DEBUG_ENABLE
    for(int i = 7; i >= 0 ; i--)
    {
      USBSerialDebug.print(((int)data & (0b0000000000000001 << i)) >> i, BIN);
    }
#endif
}

void updateLEDsAccordingToHost()
{
  setCapsLockLED(isHostCapsLockLEDOn());
  setNumsLockLED(isHostNumLockLEDOn());
  setScrollLockLED(isHostScrollLockLEDOn());
}

bool isHostNumLockLEDOn()
{
  uint8 _LEDs = USBKeyboard.getLEDs();
  return _LEDs & 0x01;
}

bool isHostCapsLockLEDOn()
{
  uint8 _LEDs = USBKeyboard.getLEDs();
  return _LEDs & 0x02;
}

bool isHostScrollLockLEDOn()
{
  uint8 _LEDs = USBKeyboard.getLEDs();
  return _LEDs & 0x04;
}

void setup()
{
#if USB_DEBUG_ENABLE
  Serial.begin();
#else
  // NOTE: this overload of begin() was written by me, so these files are custom:
  //  /Users/myUser/Library/Arduino15/packages/stm32duino/hardware/STM32F1/2019.10.9/libraries/USBComposite/USBHID.h
  //  /Users/myUser/Library/Arduino15/packages/stm32duino/hardware/STM32F1/2019.10.9/libraries/USBComposite/USBHID.cpp
  HID.begin(HID_KEYBOARD_MOUSE, ID_VENDOR, ID_PRODUCT, MANUFACTURER, PRODUCT, SERIALNUMBER);
  USBKeyboard.begin();
#endif

  // The on-board LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ON);

  // To drive the transistor that pulls the data line low
  pinMode(DATA_PIN_OUTPUT, OUTPUT);
  setDataHigh(1);

  // To read the data line
  pinMode(DATA_PIN_INPUT, INPUT);

  // For the power switch in the keyboard
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);

  // Reset
  setDataLow(4000);
  setDataHigh(1000);
  //sendCommandByte(COMMAND_KEYBOARD_RESET);

  // Not entirely sure about this one, though
  USBKeyboard.releaseAll();

  // This is to fix a bug in the library actually (let the host
  // handle it, since we're sending the caps-lock keystroke).
  USBKeyboard.setAdjustForHostCapsLock(false);

  // To match the LEDs to the previous host state
  // NOTE: this doesn't work. I suspect it's because the usb connection
  // is not ready or the host hasn't answered fast enough (so we don't
  // actually have that info yet).
  // updateLEDsAccordingToHost();

  setCapsLockLED(0);
  setNumsLockLED(0);
  setScrollLockLED(0);
}

void loop()
{
  uint16 _receivedData = 0xFFFF, _commandData;

  // A little wait up time
  for(byte j = 0; j < 5; j++)
  {
    delay(50);
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
  }

#if USB_DEBUG_ENABLE
  USBSerialDebug.println("** ADB-USB - Joe Simon 2018-2019 **\nStarting now ...");
#endif

  // Read the initial status bits
  sendCommandByte(COMMAND_KEYBOARD_GET_STATUS);
  receiveData(&currentStatus);

  // Counter to poll the host about the locks LEDs
  const int askForLocksUpdateLoopTimes = 50;
  int counterLockLEDsUpdate = 0;

  while(1)
  {
    delay(MESSAGE_DELAY_MS);

    // To refresh the LEDs of the locks every now and then (they may
    // have changed in the host for some reason, like having another
    // keyboard of in software).
    counterLockLEDsUpdate++;
    if (counterLockLEDsUpdate > askForLocksUpdateLoopTimes)
    {
      updateLEDsAccordingToHost();
      counterLockLEDsUpdate = 0;
    }

    // Do the LED scrolling
    if(!globalEnabled)
    {
      ledScrollingCounter++;
      if (ledScrollingCounter == 3)
        ledScrollingCounter = 0;

      setScrollLockLED(ledScrollingCounter == 2);
      delay(10);
      setNumsLockLED(ledScrollingCounter == 1);
      delay(10);
      setCapsLockLED(ledScrollingCounter == 0);
      delay(100);
    }

    sendCommandByte(COMMAND_KEYBOARD_GET_CHAR);
    if (!receiveData(&_receivedData))
    {
      continue;
    }

    digitalWrite(LED, ON);
    processKeyCodes((_receivedData & 0xFF00) >> 8, _receivedData & 0x00FF);
    digitalWrite(LED, OFF);
  }
}
