#ifndef __ADB_KEYBOARD_MAPPING__
#define __ADB_KEYBOARD_MAPPING__

#define STATUS_BIT_LED_NUM_LOCK      0
#define STATUS_BIT_LED_CAPS_LOCK     1
#define STATUS_BIT_LED_SCROLL_LOCK   2
#define STATUS_BIT_SCROLLLOCK        6
#define STATUS_BIT_NUMLOCK           7
#define STATUS_BIT_APPLE_COMMAND     8
#define STATUS_BIT_OPTION            9
#define STATUS_BIT_SHIFT             10
#define STATUS_BIT_CONTROL           11
#define STATUS_BIT_RESET_POWER       12
#define STATUS_BIT_CAPSLOCK          13
#define STATUS_BIT_DELETE            14

#define KEYBOARD_ADDRESS             2
#define COMMAND_RESET                0
#define COMMAND_FLUSH                1
#define COMMAND_LISTEN               2
#define COMMAND_TALK                 3
#define KEYBOARD_REGISTER_KEYS       0
#define KEYBOARD_REGISTER_STATUS     2
#define COMMAND_KEYBOARD_GET_CHAR    KEYBOARD_ADDRESS << 4 | COMMAND_TALK   << 2 | KEYBOARD_REGISTER_KEYS
#define COMMAND_KEYBOARD_GET_STATUS  KEYBOARD_ADDRESS << 4 | COMMAND_TALK   << 2 | KEYBOARD_REGISTER_STATUS
#define COMMAND_KEYBOARD_SET_STATUS  KEYBOARD_ADDRESS << 4 | COMMAND_LISTEN << 2 | KEYBOARD_REGISTER_STATUS
#define COMMAND_KEYBOARD_RESET       KEYBOARD_ADDRESS << 4 | COMMAND_RESET  << 2

#define MOUSE_ADDRESS                3
#define MOUSE_REGISTER_CURSOR        0
#define MOUSE_REGISTER_STATUS        2
#define COMMAND_MOUSE_GET_STATUS     MOUSE_ADDRESS << 4 | COMMAND_TALK   << 2 | MOUSE_REGISTER_STATUS
#define COMMAND_MOUSE_GET_CURSOR     MOUSE_ADDRESS << 4 | COMMAND_TALK   << 2 | MOUSE_REGISTER_CURSOR
#define COMMAND_MOUSE_RESET          MOUSE_ADDRESS << 4 | COMMAND_RESET  << 2

#define ADB_KEY_CAPS_LOCK            57
#define ADB_KEY_NUM_LOCK             71
#define ADB_KEY_SCROLL_LOCK          107
#define ADB_KEY_POWER_BUTTON         255

#define ADB_KEY_NUM_0                82
#define ADB_KEY_NUM_1                83
#define ADB_KEY_NUM_2                84
#define ADB_KEY_NUM_3                85
#define ADB_KEY_NUM_4                86
#define ADB_KEY_NUM_5                87
#define ADB_KEY_NUM_6                88
#define ADB_KEY_NUM_7                89
#define ADB_KEY_NUM_8                91
#define ADB_KEY_NUM_9                92
#define ADB_KEY_NUM_POINT            65
#define ADB_KEY_NUM_EQUAL            81
#define ADB_KEY_NUM_SLASH            75
#define ADB_KEY_NUM_ASTERISK         67
#define ADB_KEY_NUM_MINUS            78
#define ADB_KEY_NUM_PLUS             69
#define ADB_KEY_NUM_ENTER            76

// Added because the arduino Keyboard is missing these
#define KEY_PRNT_SCRN 0xCE
#define KEY_F13 0x68
#define KEY_F14 0x69
#define KEY_F15 0x6A

// Added the last ones missing
// Got these from: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
#define HID_KEYCODE_SCROLLLOCK 0x47 // Keyboard Scroll Lock
#define HID_KEYCODE_NUMLOCK    0x53 // Keyboard Num Lock and Clear
#define HID_KEYCODE_KPSLASH    0x54 // Keypad /
#define HID_KEYCODE_KPASTERISK 0x55 // Keypad *
#define HID_KEYCODE_KPMINUS    0x56 // Keypad -
#define HID_KEYCODE_KPPLUS     0x57 // Keypad +
#define HID_KEYCODE_KPENTER    0x58 // Keypad ENTER
#define HID_KEYCODE_KP1        0x59 // Keypad 1 and End
#define HID_KEYCODE_KP2        0x5a // Keypad 2 and Down Arrow
#define HID_KEYCODE_KP3        0x5b // Keypad 3 and PageDn
#define HID_KEYCODE_KP4        0x5c // Keypad 4 and Left Arrow
#define HID_KEYCODE_KP5        0x5d // Keypad 5
#define HID_KEYCODE_KP6        0x5e // Keypad 6 and Right Arrow
#define HID_KEYCODE_KP7        0x5f // Keypad 7 and Home
#define HID_KEYCODE_KP8        0x60 // Keypad 8 and Up Arrow
#define HID_KEYCODE_KP9        0x61 // Keypad 9 and Page Up
#define HID_KEYCODE_KP0        0x62 // Keypad 0 and Insert
#define HID_KEYCODE_KPDOT      0x63 // Keypad . and Delete
#define HID_KEYCODE_POWER      0x66 // Keyboard Power
#define HID_KEYCODE_KPEQUAL    0x67 // Keypad =


/*
  Online resources:

  https://source.android.com/devices/input/keyboard-devices
  https://github.com/tmk/tmk_core/blob/master/protocol/adb.c#L301-L478
  https://github.com/tmk/tmk_core/blob/master/protocol/adb.h
  https://github.com/tmk/tmk_core/blob/master/protocol/adb.c#L105
  http://www.goldstarsoftware.com/applesite/Documentation/AppleIIgsHardwareReferenceManual.PDF
  https://geekhack.org/index.php?PHPSESSID=707vc2p13cua9ah9f3je11q4qh0ufib3&topic=14290.0
  http://213.114.131.21/_pdf/Processor_Memory/PIC1617/MIDRANGE/00591A.PDF
  https://cdn-learn.adafruit.com/assets/assets/000/002/901/original/microcontrollers_next.jpg?1396788484
  https://learn.adafruit.com/assets/2901
  https://learn.adafruit.com/assets/2901
  https://learn.adafruit.com/usb-next-keyboard-with-arduino-micro/research
  https://blog.arduino.cc/2013/01/10/an-arduino-based-adb-to-usb-adapter-for-next-keyboards/
*/


const unsigned char adb_ascii_map[128] = {

  // ADB
  // number    USB/ASCII                ADB-Keyboard description
  // -----------------------------------------------------------
  /*   0 */    'a',
  /*   1 */    's',
  /*   2 */    'd',
  /*   3 */    'f',
  /*   4 */    'h',
  /*   5 */    'g',
  /*   6 */    'z',
  /*   7 */    'x',
  /*   8 */    'c',
  /*   9 */    'v',
  /*  10 */    '?',
  /*  11 */    'b',
  /*  12 */    'q',
  /*  13 */    'w',
  /*  14 */    'e',
  /*  15 */    'r',
  /*  16 */    'y',
  /*  17 */    't',
  /*  18 */    '1',
  /*  19 */    '2',
  /*  20 */    '3',
  /*  21 */    '4',
  /*  22 */    '6',
  /*  23 */    '5',
  /*  24 */    '=',
  /*  25 */    '9',
  /*  26 */    '7',
  /*  27 */    '-',
  /*  28 */    '8',
  /*  29 */    '0',
  /*  30 */    ']',
  /*  31 */    'o',
  /*  32 */    'u',
  /*  33 */    '[',
  /*  34 */    'i',
  /*  35 */    'p',
  /*  36 */    KEY_RETURN,
  /*  37 */    'l',
  /*  38 */    'j',
  /*  39 */    '\'',
  /*  40 */    'k',
  /*  41 */    ';',
  /*  42 */    '\\',
  /*  43 */    ',',
  /*  44 */    '/',
  /*  45 */    'n',
  /*  46 */    'm',
  /*  47 */    '.',
  /*  48 */    KEY_TAB,
  /*  49 */    ' ',
  /*  50 */    96,  // "`"
  /*  51 */    KEY_BACKSPACE,
  /*  52 */    0,
  /*  53 */    KEY_ESC,
  /*  54 */    KEY_LEFT_CTRL,
  /*  55 */    KEY_LEFT_GUI,
  /*  56 */    KEY_LEFT_SHIFT,
  /*  57 */    KEY_CAPS_LOCK,
  /*  58 */    KEY_LEFT_ALT,
  /*  59 */    KEY_LEFT_ARROW,
  /*  60 */    KEY_RIGHT_ARROW,
  /*  61 */    KEY_DOWN_ARROW,
  /*  62 */    KEY_UP_ARROW,
  /*  63 */    0,
  /*  64 */    0,
  /*  65 */    HID_KEYCODE_KPDOT,       //  num-point     // ------ Send as raw HID keycodes ------
  /*  66 */    0,                                         //   ^
  /*  67 */    HID_KEYCODE_KPASTERISK,  //  num-asterisk  //   |
  /*  68 */    0,                                         //   |
  /*  69 */    HID_KEYCODE_KPPLUS,      //  num-plus      //   |
  /*  70 */    0,                                         //   |
  /*  71 */    HID_KEYCODE_NUMLOCK,     //  Num lock      //   |
  /*  72 */    0,                                         //   |
  /*  73 */    0,                                         //   |
  /*  74 */    0,                                         //   |
  /*  75 */    HID_KEYCODE_KPSLASH,     //  num-slash     //   |
  /*  76 */    HID_KEYCODE_KPENTER,     //  num-enter     //   |
  /*  77 */    0,                                         //   |
  /*  78 */    HID_KEYCODE_KPMINUS,     //  num-minus     //   |
  /*  79 */    0,                                         //   |
  /*  80 */    0,                                         //   |
  /*  81 */    HID_KEYCODE_KPEQUAL,     //  num-equal     //   |
  /*  82 */    HID_KEYCODE_KP0,         //  num-0         //   |
  /*  83 */    HID_KEYCODE_KP1,         //  num-1         //   |
  /*  84 */    HID_KEYCODE_KP2,         //  num-2         //   |
  /*  85 */    HID_KEYCODE_KP3,         //  num-3         //   |
  /*  86 */    HID_KEYCODE_KP4,         //  num-4         //   |
  /*  87 */    HID_KEYCODE_KP5,         //  num-5         //   |
  /*  88 */    HID_KEYCODE_KP6,         //  num-6         //   |
  /*  89 */    HID_KEYCODE_KP7,         //  num-7         //   |
  /*  90 */    0,                                         //   |
  /*  91 */    HID_KEYCODE_KP8,         //  num-8         //   v
  /*  92 */    HID_KEYCODE_KP9,         //  num-9         // ------ Send as raw HID keycodes ------
  /*  93 */    0,
  /*  94 */    0,
  /*  95 */    0,
  /*  96 */    KEY_F5,                  //  F5
  /*  97 */    KEY_F6,                  //  F6
  /*  98 */    KEY_F7,                  //  F7
  /*  99 */    KEY_F3,                  //  F3
  /* 100 */    KEY_F8,                  //  F8
  /* 101 */    KEY_F9,                  //  F9
  /* 102 */    0,
  /* 103 */    KEY_F11,                 //  F11
  /* 104 */    0,
  /* 105 */    KEY_PRNT_SCRN,           //  F13-print screen
  /* 106 */    0,
  /* 107 */    KEY_F14,                 //  F14-scroll lock
  /* 108 */    0,
  /* 109 */    KEY_F10,                 //  F10
  /* 110 */    0,
  /* 111 */    KEY_F12,                 //  F12
  /* 112 */    0,
  /* 113 */    KEY_F15,                 //  F15-pause
  /* 114 */    KEY_INSERT,              //  ins
  /* 115 */    KEY_HOME,                //  home
  /* 116 */    KEY_PAGE_UP,             //  page up
  /* 117 */    KEY_DELETE,              //  del
  /* 118 */    KEY_F4,                  //  F4
  /* 119 */    KEY_END,                 //  end
  /* 120 */    KEY_F2,                  //  F2
  /* 121 */    KEY_PAGE_DOWN,           //  page down
  /* 122 */    KEY_F1,                  //  F1
  /* 123 */    0,
  /* 124 */    0,
  /* 125 */    0,
  /* 126 */    0,
  /* 127 */    0,                       //  Power button
    };

#endif
