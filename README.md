# Minimal TVOut for Arduino

This is a library for generating composite video on an ATmega microcontroller.

The output is NTSC or PAL at a resolution of 128x96 by default.

This is a branch of the [Avamander/arduino-tvout](https://github.com/Avamander/arduino-tvout) branch of the original TVout library.

In this branch I have removed all of the 'convenience' features for handling graphics, text, and audio since these features are generally implemented in my own code. This reduces the library to a background task which references bit-mapped pixel data.

## Device Compatibility

The library currently works on ATmega168,328,1280,2560,644p,1284p,32U4, and AT90USB1286.

More can be added by editing `spec/hardware_setup.h`.

There are some timing issues with the m1284p, may be related to sanguino core.

```
MCU         SYNC  VIDEO  Arduino          SYNC  VIDEO
m168,m328   B 1   D 7    NG,Decimila,UNO  9     7
m1280,m2560 B 5   A 7    Mega             11    A7(D29)
m644,m1284p D 5   A 7    sanguino         13    A7(D24)
m32u4       B 5   B 4    Leonardo         9     8
AT90USB1286 B 5   F 7    --               --    --
```

## Connections

`SYNC` is on `OCR1A`

There are some timing issues with the m1284p, which may be related to sanguino core.

On NG, Decimila, UNO and Nano `SYNC` is pin 9 and `VIDEO` is on 7.

On Mega2560 `SYNC` is pin 11, `VIDEO` is on `A7(D29)`.

## Usage Example

```
  // The width of our display, in pixels.
  static const kResolutionX = 128;

  // The height of our display, in pixels.
  static const kResolutionY = 96;

  static uint8_t screenBuffer[(kResolutionX / 8) * kResolutionY];

  ...

  TVVideoGeneratorBegin(TVFormatNTSC, kResolutionX, kResolutionY, &screenBuffer);
```

## Videos

These are examples of the original TVout library.

https://youtu.be/MEg_V4YZDh0

https://youtu.be/bHpFv_x_8Kk
