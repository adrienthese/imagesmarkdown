# Hardware

[TOC]

The main part of the system is the NUCLEO-L073RZ board.

It uses add-on boards.

- KIM1 shield version 3.
- [MIKROE Thermo 3 click](https://www.mikroe.com/thermo-3-click#/263-clickid-yes).
- [MIKROE microSD click](https://www.mikroe.com/microsd-click).
- [Adafruit Ultimate GPS FeatherWing](https://learn.adafruit.com/adafruit-ultimate-gps-featherwing).

## NUCLEO-L073RZ

GPS and KIM1 uses serial. The `Serial` default instance is often used for debug. 

Therefore, Nucleo serial devices must be setup with caution.

### KIM1 serial

The default `Serial` instance is `SerialLP1` on LPUART1. A side effect of using it, is that pins D0 and D1 are disabled. 

D0 and D1 are mandatory to allow communication with the KIM1 shield.

An hardware modification enable `Serial2` on UART2 pins D0 and D1 (see *6.8 USART communication* on [STM32 Nucleo-64 boards User manual UM1724](https://www.st.com/resource/en/user_manual/dm00105823-stm32-nucleo-64-boards-mb1136-stmicroelectronics.pdf)).

- SB62 and SB63 must be ON.
- SB13 and SB14 must be OFF.

To enable `Serial2` instance on default pins D0 and D1, add `-DENABLE_HWSERIAL2` to *Compile Options* of the project.

```
-DENABLE_HWSERIAL2
```

### GPS serial

`Serial5` instance is not enabled by default. It can be used to communicate with the GPS. 

To enable `Serial5` instance on default pins D2 and D5, add `-DENABLE_HWSERIAL5` to *Compile Options* of the project.

```
-DENABLE_HWSERIAL5
```

### Serial

`Serial1` instance is not enabled by default. It can be used to replace the default `Serial` instance on pins D2 and D8.

To enable `Serial1` as the `Serial` instance, add `-DSERIAL_UART_INSTANCE=1` to *Compile Options* of the project.

Default pins are still D0 and D1. To modify default pins to D2 and D8, add `-DPIN_SERIAL_RX=PA10 -DPIN_SERIAL_TX=PA9` to *Compile Options* of the project. 

```
-DSERIAL_UART_INSTANCE=1 -DPIN_SERIAL_RX=PA10 -DPIN_SERIAL_TX=PA9
```

@note Use an USB adapter like [Adaptateur Mini-USB vers TTL FT232RL](https://www.kubii.com/fr/hub-cables-adaptateurs/3320-adaptateur-usb-vers-ttl-ft232rl-3272496306400.html), to view serial output on a PC.

### Pinouts

This is the complete pinouts for the serial instances.

|Serial|NUCLEO-L073RZ|Arduino|
|-|-|-|
|UART1_TX|PA9|D8|
|UART1_RX|PA10|D2|
|UART2_TX|PA2|D1|
|UART2_RX|PA3|D0|
|UART5_TX|PB3|D3|
|UART5_RX|PB4|D5|

### Compile options

This is the complete compile options for the project.

```
-DSERIAL_UART_INSTANCE=1 -DPIN_SERIAL_RX=PA10 -DPIN_SERIAL_TX=PA9 -DENABLE_HWSERIAL2 -DENABLE_HWSERIAL5
```

## KIM1 shield version 3

The KIM1 module allows to send data to Kineis satellites.

The shield uses serial UART. The UART device on the Nucleo board is UART2.

It is possible to power on or off the KIM1 with a dedicated GPIO on the NUCLEO board.

|KIM1|NUCLEO-L073RZ|Arduino|
|-|-|-|
|RX_K1|PA2|D1|
|TX_K1|PA3|D0|
|ON/<span style="text-decoration: overline">OFF</span>|PB5|D4|

## MIKROE Thermo 3 click

The board uses a low power temperature sensor [TMP102](https://download.mikroe.com/documents/datasheets/tmp102-data-sheet.pdf) on the I2C bus at address 0x48.
 
|I2C|NUCLEO-L073RZ|Arduino|
|-|-|-|
|SDA|PB8|D14|
|SCL|PB9|D15|

@note SDA and SCL are available through the KIM1 shield.

## MIKROE microSD click

SD is available on SPI.

|SPI|NUCLEO-L073RZ|Arduino|
|-|-|-|
|SS|PB6|D10|
|MOSI|PA7|D11|
|MISO|PA6|D12|
|SCK|PA5|D13|

@note SS, MOSI, MISO and SCK are available through the KIM1 shield.

## Adafruit Ultimate GPS FeatherWing

This FeatherWing uses a GPS module PA6H with a MK3339 to provide location and time.

The shield uses serial UART. The UART device on the Nucleo board is UART5.

It is possible to enable or disable the GPS with a dedicated GPIO on the NUCLEO board.

|GPS|NUCLEO-L073RZ|Arduino|
|-|-|-|
|RX|PB3|D3|
|TX|PB4|D5|
|<span style="text-decoration: overline">EN</span>|PA8|D7|

## Battery

A battery can be used to power all the devices.

The Nucleo board monitors the battery voltage with an ADC on pin A0.

A voltage divider is used to adapt the 5V of the battery to the 3.3V used by the STM32.

|%Battery|NUCLEO-L073RZ|Arduino|
|-|-|-|
|Voltage|PA0|A0|

<div class="section_buttons">
 
| Previous               |                    Next |
|:-----------------------|------------------------:|
| [CosmoNode](README.md) | [Firmware](firmware.md) |
 
</div>