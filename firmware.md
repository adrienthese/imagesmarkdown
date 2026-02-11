# Firmware

[TOC]

The firmware is written with the official [STM32 Arduino core](https://github.com/stm32duino/Arduino_Core_STM32), currently in version 2.10.1.

It uses several libraries.

- [Adafruit GPS library](https://github.com/adafruit/Adafruit_GPS) version 1.7.5.
- [SD](https://github.com/arduino-libraries/SD) version 1.3.0.
- [STM32duino Low Power](https://github.com/stm32duino/STM32LowPower) version 1.4.0.
- [STM32duino RTC](https://github.com/stm32duino/STM32RTC) version 1.7.0.

## Files

The firmware uses 2 files stored on the SD card.

### Calendar

The calendar file stores the epoch when a satellite is insight.

Epoch are stored in integer textual format. Values are separated by line breaks.

```txt
1719532329
1719532389
1719532449
1719532509
```

@todo Binary format could be used.
    In 64-bit it uses 8 bytes for an epoch versus 12 in textual 32-bit mode (10 + CRLF).

### Data

CSV is used as data file format.

```
Node;Counter;Date;Time;Temperature;Latitude;Longitude;Altitude;Battery voltage
0001;00001;29/11/24;09:37:49;20.56;45.71;3.17;398.30;0.24
0001;00002;29/11/24;09:38:19;20.56;45.71;3.17;398.30;0.21
0001;00003;29/11/24;09:38:49;20.56;45.71;3.17;398.30;0.18
0001;00004;29/11/24;09:39:19;20.56;45.71;3.17;398.30;0.24
```

## Packet

Only 1 kind of packet is sent by the KIM1 module.

Data in the *TX* command is encoded in hexadecimal format.

|Offset|Data|Size|
|-|-|-|
|0|Node|2|
|2|Counter|4|
|6|Year|2|
|8|Month|2|
|10|Day|2|
|12|Hour|2|
|14|Minute|2|
|16|Seconds|2|
|18|Temperature|4|
|22|Latitude|8|
|30|Longitude|8|
|38|Altitude|8|
|46|%Battery voltage|8|

Total size is 54 encoded bytes.

Frame size in raw format is 27 bytes, 216 bits.

Example

```
010003180618081200139042370AF5404710CB430FB33340A4CDBB
```

@warning The format is slightly different from Kineis initial project.
    - Counter is an unsigned 16-bit integer.
    - Temperature is a raw signed 12-bit integer.
    - Latitude, longitude, altitude and %battery voltage are IEEE754 floats.

## Output

Firmware version 0.1.0

```
11:31:54: [BOOT] CosmoNode
11:31:54: [BOOT] Nov 29 2024, 11:29:38
11:31:54: [BOOT] CPU frequency 32.00 MHz
11:31:54: [BOOT] core 2.9.0
11:31:54: [BOOT] firmware 0.1.0
11:31:54: [BOOT] setup
11:31:59: [KIM1] ping OK
11:31:59: [KIM1] id 26EB68B
11:31:59: [KIM1] fw KIM1_V2.1
11:31:59: [KIM1] sn KIM13211200136
11:31:59: [SD] initialized
11:31:59: [GPS] nmea $PMTK001,314,3*36
11:31:59: [GPS] nmea $PMTK001,220,3*30
11:32:00: [GPS] nmea $GPGGA,235956.800,,,,,0,00,,,M,,M,,*7E
11:32:00: [GPS] nmea $GPRMC,235956.800,V,,,,,0.00,0.00,050180,,,N*47
11:32:01: [GPS] nmea $GPGGA,235957.800,,,,,0,00,,,M,,M,,*7F
11:32:01: [GPS] nmea $GPRMC,235957.800,V,,,,,0.00,0.00,050180,,,N*46
11:32:02: [GPS] nmea $GPGGA,235958.800,,,,,0,00,,,M,,M,,*70
11:32:02: [GPS] nmea $GPRMC,235958.800,V,,,,,0.00,0.00,050180,,,N*49
11:32:03: [GPS] nmea $GPGGA,235959.800,,,,,0,00,,,M,,M,,*71
11:32:03: [GPS] nmea $GPRMC,235959.800,V,,,,,0.00,0.00,050180,,,N*48
11:32:04: [GPS] nmea $GPGGA,000000.800,,,,,0,00,,,M,,M,,*70
11:32:04: [GPS] nmea $GPRMC,000000.800,V,,,,,0.00,0.00,060180,,,N*4A
11:32:05: [GPS] nmea $GPGGA,000001.800,,,,,0,00,,,M,,M,,*71
11:32:05: [GPS] nmea $GPRMC,000001.800,V,,,,,0.00,0.00,060180,,,N*4B
11:32:06: [GPS] nmea $GPGGA,000002.800,,,,,0,00,,,M,,M,,*72
11:32:06: [GPS] nmea $GPRMC,000002.800,V,,,,,0.00,0.00,060180,,,N*48
11:32:06: [GPS] nmea $GPGGA,000003.015,,,,,0,00,,,M,,M,,*7F
11:32:06: [GPS] nmea $GPRMC,000003.015,V,,,,,0.00,0.00,060180,,,N*45
11:32:07: [GPS] nmea $GPGGA,000004.015,,,,,0,00,,,M,,M,,*78
11:32:07: [GPS] nmea $GPRMC,000004.015,V,,,,,0.00,0.00,060180,,,N*42
11:32:08: [GPS] nmea $GPGGA,103208.184,,,,,0,00,,,M,,M,,*7D
11:32:08: [GPS] nmea $GPRMC,103208.184,V,,,,,0.00,0.00,110180,,,N*41
11:32:09: [GPS] nmea $GPGGA,103209.184,,,,,0,00,,,M,,M,,*7C
11:32:09: [GPS] nmea $GPRMC,103209.184,V,,,,,0.00,0.00,110180,,,N*40
11:32:10: [GPS] nmea $GPGGA,103210.184,,,,,0,00,,,M,,M,,*74
11:32:10: [GPS] nmea $GPRMC,103210.184,V,,,,,0.00,0.00,110180,,,N*48
11:32:11: [GPS] nmea $GPGGA,103211.184,,,,,0,00,,,M,,M,,*75
11:32:11: [GPS] nmea $GPRMC,103211.184,V,,,,,0.00,0.00,110180,,,N*49
11:32:12: [GPS] nmea $GPGGA,103212.092,,,,,0,00,,,M,,M,,*70
11:32:12: [GPS] nmea $GPRMC,103212.092,V,,,,,0.00,0.00,110180,,,N*4C
11:32:13: [GPS] nmea $GPGGA,103213.092,,,,,0,00,,,M,,M,,*71
11:32:13: [GPS] nmea $GPRMC,103213.092,V,,,,,0.00,0.00,110180,,,N*4D
11:32:14: [GPS] nmea $GPGGA,103214.092,,,,,0,00,,,M,,M,,*76
11:32:14: [GPS] nmea $GPRMC,103214.092,V,,,,,0.00,0.00,110180,,,N*4A
11:32:15: [GPS] nmea $GPGGA,103215.092,,,,,0,00,,,M,,M,,*77
11:32:15: [GPS] nmea $GPRMC,103215.092,V,,,,,0.00,0.00,110180,,,N*4B
11:32:16: [GPS] nmea $GPGGA,103216.092,,,,,0,00,,,M,,M,,*74
11:32:16: [GPS] nmea $GPRMC,103216.092,V,,,,,0.00,0.00,110180,,,N*48
11:32:17: [GPS] nmea $GPGGA,103217.092,,,,,0,00,,,M,,M,,*75
11:32:17: [GPS] nmea $GPRMC,103217.092,V,,,,,0.00,0.00,110180,,,N*49
11:32:18: [GPS] nmea $GPGGA,103218.092,,,,,0,00,,,M,,M,,*7A
11:32:18: [GPS] nmea $GPRMC,103218.092,V,,,,,0.00,0.00,110180,,,N*46
11:32:19: [GPS] nmea $GPGGA,103219.092,,,,,0,00,,,M,,M,,*7B
11:32:19: [GPS] nmea $GPRMC,103219.092,V,,,,,0.00,0.00,110180,,,N*47
11:32:20: [GPS] nmea $GPGGA,103220.092,,,,,0,00,,,M,,M,,*71
11:32:20: [GPS] nmea $GPRMC,103220.092,V,,,,,0.00,0.00,110180,,,N*4D
11:32:21: [GPS] nmea $GPGGA,103221.092,,,,,0,00,,,M,,M,,*70
11:32:21: [GPS] nmea $GPRMC,103221.092,V,,,,,0.00,0.00,110180,,,N*4C
11:32:22: [GPS] nmea $GPGGA,103222.092,,,,,0,00,,,M,,M,,*73
11:32:22: [GPS] nmea $GPRMC,103222.092,V,,,,,0.00,0.00,110180,,,N*4F
11:32:23: [GPS] nmea $GPGGA,103223.092,,,,,0,00,,,M,,M,,*72
11:32:23: [GPS] nmea $GPRMC,103223.092,V,,,,,0.00,0.00,110180,,,N*4E
11:32:24: [GPS] nmea $GPGGA,103224.092,,,,,0,00,,,M,,M,,*75
11:32:24: [GPS] nmea $GPRMC,103224.092,V,,,,,0.00,0.00,110180,,,N*49
11:32:25: [GPS] nmea $GPGGA,103225.092,,,,,0,00,,,M,,M,,*74
11:32:25: [GPS] nmea $GPRMC,103225.092,V,,,,,0.00,0.00,110180,,,N*48
11:32:26: [GPS] nmea $GPGGA,103226.092,,,,,0,00,,,M,,M,,*77
11:32:26: [GPS] nmea $GPRMC,103226.092,V,,,,,0.00,0.00,110180,,,N*4B
11:32:27: [GPS] nmea $GPGGA,103227.092,,,,,0,00,,,M,,M,,*76
11:32:27: [GPS] nmea $GPRMC,103227.092,V,,,,,0.00,0.00,110180,,,N*4A
11:32:28: [GPS] nmea $GPGGA,103228.092,,,,,0,00,,,M,,M,,*79
11:32:28: [GPS] nmea $GPRMC,103228.092,V,,,,,0.00,0.00,110180,,,N*45
11:32:29: [GPS] nmea $GPGGA,103229.092,,,,,0,00,,,M,,M,,*78
11:32:29: [GPS] nmea $GPRMC,103229.092,V,,,,,0.00,0.00,110180,,,N*44
11:32:30: [GPS] nmea $GPGGA,103230.087,,,,,0,04,,,M,,M,,*70
11:32:30: [GPS] nmea $GPRMC,103230.087,V,,,,,0.25,100.81,291124,,,N*43
11:32:31: [GPS] nmea $GPGGA,103231.087,4542.6421,N,00310.0168,E,1,04,2.15,403.1,M,48.7,M,,*65
11:32:31: [GPS] date, time and location OK
11:33:01: [LOOP] wake up
11:33:31: [LOOP] wake up
```

Firmware version 0.2.0

```
17:26:33: [BOOT] CosmoNode
17:26:33: [BOOT] Dec 20 2024, 17:25:52
17:26:33: [BOOT] CPU frequency 32.00 MHz
17:26:33: [BOOT] core 2.9.0
17:26:33: [BOOT] firmware 0.2.0
17:26:33: [BOOT] setup
17:26:35: [ERROR] SD can not be initialized
17:26:40: [KIM1] ping OK
17:26:40: [KIM1] id 26EB68B
17:26:40: [KIM1] fw KIM1_V2.1
17:26:40: [KIM1] sn KIM13211200136
17:26:40: [GPS] nmea $PMTK001,314,3*36
17:26:40: [GPS] nmea $PMTK001,220,3*30
17:26:40: [GPS] nmea $GPGGA,162640.143,,,,,0,00,,,M,,M,,*79
17:26:40: [GPS] nmea $GPRMC,162640.143,V,,,,,0.00,0.00,110180,,,N*45
17:26:41: [GPS] nmea $GPGGA,162641.143,,,,,0,00,,,M,,M,,*78
17:26:41: [GPS] nmea $GPRMC,162641.143,V,,,,,0.00,0.00,110180,,,N*44
17:26:42: [GPS] nmea $GPGGA,162642.103,,,,,0,00,,,M,,M,,*7F
17:26:42: [GPS] nmea $GPRMC,162642.103,V,,,,,0.00,0.00,110180,,,N*43
17:26:43: [GPS] nmea $GPGGA,162643.103,,,,,0,00,,,M,,M,,*7E
17:26:43: [GPS] nmea $GPRMC,162643.103,V,,,,,0.00,0.00,110180,,,N*42
17:26:44: [GPS] nmea $GPGGA,162644.103,,,,,0,00,,,M,,M,,*79
17:26:44: [GPS] nmea $GPRMC,162644.103,V,,,,,0.00,0.00,110180,,,N*45
17:26:45: [GPS] nmea $GPGGA,162645.103,,,,,0,00,,,M,,M,,*78
17:26:45: [GPS] nmea $GPRMC,162645.103,V,,,,,0.00,0.00,110180,,,N*44
17:26:46: [GPS] nmea $GPGGA,162646.103,,,,,0,00,,,M,,M,,*7B
17:26:46: [GPS] nmea $GPRMC,162646.103,V,,,,,0.00,0.00,110180,,,N*47
17:26:47: [GPS] nmea $GPGGA,162647.103,,,,,0,00,,,M,,M,,*7A
17:26:47: [GPS] nmea $GPRMC,162647.103,V,,,,,0.00,0.00,110180,,,N*46
17:26:48: [GPS] nmea $GPGGA,162648.103,,,,,0,00,,,M,,M,,*75
17:26:48: [GPS] nmea $GPRMC,162648.103,V,,,,,0.00,0.00,110180,,,N*49
17:26:49: [GPS] nmea $GPGGA,162649.103,,,,,0,00,,,M,,M,,*74
17:26:49: [GPS] nmea $GPRMC,162649.103,V,,,,,0.00,0.00,110180,,,N*48
17:26:50: [GPS] nmea $GPGGA,162650.103,,,,,0,00,,,M,,M,,*7C
17:26:50: [GPS] nmea $GPRMC,162650.103,V,,,,,0.00,0.00,110180,,,N*40
17:26:51: [GPS] nmea $GPGGA,162651.103,,,,,0,00,,,M,,M,,*7D
17:26:51: [GPS] nmea $GPRMC,162651.103,V,,,,,0.00,0.00,110180,,,N*41
17:26:52: [GPS] nmea $GPGGA,162652.103,,,,,0,00,,,M,,M,,*7E
17:26:52: [GPS] nmea $GPRMC,162652.103,V,,,,,0.00,0.00,110180,,,N*42
17:26:53: [GPS] nmea $GPGGA,162653.103,,,,,0,00,,,M,,M,,*7F
17:26:53: [GPS] nmea $GPRMC,162653.103,V,,,,,0.00,0.00,110180,,,N*43
17:26:54: [GPS] nmea $GPGGA,162654.091,,,,,0,00,,,M,,M,,*72
17:26:54: [GPS] nmea $GPRMC,162654.091,V,,,,,0.00,0.00,110180,,,N*4E
17:26:55: [GPS] nmea $GPGGA,162654.310,,,,,0,00,,,M,,M,,*78
17:26:55: [GPS] nmea $GPRMC,162654.310,V,,,,,0.00,0.00,110180,,,N*44
17:26:56: [GPS] nmea $GPGGA,162655.310,,,,,0,00,,,M,,M,,*79
17:26:56: [GPS] nmea $GPRMC,162655.310,V,,,,,0.00,0.00,110180,,,N*45
17:26:57: [GPS] nmea $GPGGA,162656.310,,,,,0,00,,,M,,M,,*7A
17:26:57: [GPS] nmea $GPRMC,162656.310,V,,,,,0.00,0.00,110180,,,N*46
17:26:58: [GPS] nmea $GPGGA,162657.310,,,,,0,00,,,M,,M,,*7B
17:26:58: [GPS] nmea $GPRMC,162657.310,V,,,,,0.00,0.00,110180,,,N*47
17:26:59: [GPS] nmea $GPGGA,162658.310,,,,,0,00,,,M,,M,,*74
17:26:59: [GPS] nmea $GPRMC,162658.310,V,,,,,0.00,0.00,110180,,,N*48
17:27:00: [GPS] nmea $GPGGA,162659.310,,,,,0,00,,,M,,M,,*75
17:27:00: [GPS] nmea $GPRMC,162659.310,V,,,,,0.00,0.00,110180,,,N*49
17:27:01: [GPS] nmea $GPGGA,162700.091,,,,,0,00,,,M,,M,,*72
17:27:01: [GPS] nmea $GPRMC,162700.091,V,,,,,0.00,0.00,201224,,,N*40
17:27:01: [GPS] nmea $GPGGA,162700.310,,,,,0,00,,,M,,M,,*78
17:27:01: [GPS] nmea $GPRMC,162700.310,V,,,,,0.00,0.00,201224,,,N*4A
17:27:02: [GPS] nmea $GPGGA,162701.310,,,,,0,00,,,M,,M,,*79
17:27:02: [GPS] nmea $GPRMC,162701.310,V,,,,,0.00,0.00,201224,,,N*4B
17:27:03: [GPS] nmea $GPGGA,162702.310,,,,,0,00,,,M,,M,,*7A
17:27:03: [GPS] nmea $GPRMC,162702.310,V,,,,,0.00,0.00,201224,,,N*48
17:27:04: [GPS] nmea $GPGGA,162703.310,,,,,0,00,,,M,,M,,*7B
17:27:04: [GPS] nmea $GPRMC,162703.310,V,,,,,0.00,0.00,201224,,,N*49
17:27:05: [GPS] nmea $GPGGA,162704.310,,,,,0,00,,,M,,M,,*7C
17:27:05: [GPS] nmea $GPRMC,162704.310,V,,,,,0.00,0.00,201224,,,N*4E
17:27:06: [GPS] nmea $GPGGA,162705.310,,,,,0,00,,,M,,M,,*7D
17:27:06: [GPS] nmea $GPRMC,162705.310,V,,,,,0.00,0.00,201224,,,N*4F
17:27:06: [GPS] nmea $GPGGA,162706.088,,,,,0,00,,,M,,M,,*7C
17:27:06: [GPS] nmea $GPRMC,162706.088,V,,,,,0.00,0.00,201224,,,N*4E
17:27:07: [GPS] nmea $GPGGA,162706.309,,,,,0,00,,,M,,M,,*76
17:27:07: [GPS] nmea $GPRMC,162706.309,V,,,,,0.00,0.00,201224,,,N*44
17:27:08: [GPS] nmea $GPGGA,162707.309,,,,,0,00,,,M,,M,,*77
17:27:08: [GPS] nmea $GPRMC,162707.309,V,,,,,0.00,0.00,201224,,,N*45
17:27:09: [GPS] nmea $GPGGA,162708.309,,,,,0,00,,,M,,M,,*78
17:27:09: [GPS] nmea $GPRMC,162708.309,V,,,,,0.00,0.00,201224,,,N*4A
17:27:10: [GPS] nmea $GPGGA,162709.309,,,,,0,00,,,M,,M,,*79
17:27:10: [GPS] nmea $GPRMC,162709.309,V,,,,,0.00,0.00,201224,,,N*4B
17:27:11: [GPS] nmea $GPGGA,162710.309,,,,,0,00,,,M,,M,,*71
17:27:11: [GPS] nmea $GPRMC,162710.309,V,,,,,0.00,0.00,201224,,,N*43
17:27:12: [GPS] nmea $GPGGA,162711.309,,,,,0,00,,,M,,M,,*70
17:27:12: [GPS] nmea $GPRMC,162711.309,V,,,,,0.00,0.00,201224,,,N*42
17:27:12: [GPS] nmea $GPGGA,162712.093,,,,,0,00,,,M,,M,,*73
17:27:12: [GPS] nmea $GPRMC,162712.093,V,,,,,0.00,0.00,201224,,,N*41
17:27:13: [GPS] nmea $GPGGA,162712.314,,,,,0,00,,,M,,M,,*7F
17:27:13: [GPS] nmea $GPRMC,162712.314,V,,,,,0.00,0.00,201224,,,N*4D
17:27:14: [GPS] nmea $GPGGA,162713.314,,,,,0,00,,,M,,M,,*7E
17:27:14: [GPS] nmea $GPRMC,162713.314,V,,,,,0.00,0.00,201224,,,N*4C
17:27:15: [GPS] nmea $GPGGA,162714.314,,,,,0,00,,,M,,M,,*79
17:27:15: [GPS] nmea $GPRMC,162714.314,V,,,,,0.00,0.00,201224,,,N*4B
17:27:16: [GPS] nmea $GPGGA,162715.314,,,,,0,00,,,M,,M,,*78
17:27:16: [GPS] nmea $GPRMC,162715.314,V,,,,,0.00,0.00,201224,,,N*4A
17:27:17: [GPS] nmea $GPGGA,162716.314,,,,,0,00,,,M,,M,,*7B
17:27:17: [GPS] nmea $GPRMC,162716.314,V,,,,,0.00,0.00,201224,,,N*49
17:27:18: [GPS] nmea $GPGGA,162717.314,,,,,0,00,,,M,,M,,*7A
17:27:18: [GPS] nmea $GPRMC,162717.314,V,,,,,0.00,0.00,201224,,,N*48
17:27:18: [GPS] nmea $GPGGA,162718.093,,,,,0,00,,,M,,M,,*79
17:27:18: [GPS] nmea $GPRMC,162718.093,V,,,,,0.00,0.00,201224,,,N*4B
17:27:19: [GPS] nmea $GPGGA,162718.314,,,,,0,00,,,M,,M,,*75
17:27:19: [GPS] nmea $GPRMC,162718.314,V,,,,,0.00,0.00,201224,,,N*47
17:27:20: [GPS] nmea $GPGGA,162719.314,,,,,0,00,,,M,,M,,*74
17:27:20: [GPS] nmea $GPRMC,162719.314,V,,,,,0.00,0.00,201224,,,N*46
17:27:21: [GPS] nmea $GPGGA,162720.314,,,,,0,00,,,M,,M,,*7E
17:27:21: [GPS] nmea $GPRMC,162720.314,V,,,,,0.00,0.00,201224,,,N*4C
17:27:22: [GPS] nmea $GPGGA,162721.314,,,,,0,00,,,M,,M,,*7F
17:27:22: [GPS] nmea $GPRMC,162721.314,V,,,,,0.00,0.00,201224,,,N*4D
17:27:23: [GPS] nmea $GPGGA,162722.314,,,,,0,00,,,M,,M,,*7C
17:27:23: [GPS] nmea $GPRMC,162722.314,V,,,,,0.00,0.00,201224,,,N*4E
17:27:24: [GPS] nmea $GPGGA,162723.314,,,,,0,00,,,M,,M,,*7D
17:27:24: [GPS] nmea $GPRMC,162723.314,V,,,,,0.00,0.00,201224,,,N*4F
17:27:24: [GPS] nmea $GPGGA,162724.096,,,,,0,03,,,M,,M,,*70
17:27:24: [GPS] nmea $GPRMC,162724.096,V,,,,,0.10,87.04,201224,,,N*7B
17:27:25: [GPS] nmea $GPGGA,162725.096,,,,,0,04,,,M,,M,,*76
17:27:25: [GPS] nmea $GPRMC,162725.096,V,,,,,0.26,82.72,201224,,,N*7B
17:27:26: [GPS] nmea $GPGGA,162726.096,4542.6427,N,00310.0150,E,1,04,2.53,400.7,M,48.7,M,,*6B
17:27:26: [GPS] read date, time and position
17:27:26: [GPS] latitude 45.711
17:27:26: [GPS] longitude 3.167
17:27:26: [GPS] altitude 400.700
17:27:26: [RTC] set date and time
17:27:26: [RTC] 20/12/24 16:27:26
17:27:26: [ERROR] unable to open CALENDAR file
```

<!-- 
Example frame received on 2024-06-24 08:18:08.373
```
0100031806180812001390 42370AF5 404710CB 430FB333 40A4CDBB

0100031806180812001390 F50A3742 CB104740 33B30F43 BBCDA440

01 0003 18 06 18 08 12 00 13 90 F50A3742 CB104740 33B30F43 BBCDA440
01
   3
        24 06 24
                 08 18 00
                          19 144
                                45.7607  3.1104   143.7
                                                           5.1501136
``` 
-->

<!-- 
@note PC and STM32 are little endian. Why use big endian in packet?

@bug Temperature is not accurate. This is not the 12-bit raw value nor the degree Celsius.

@note Latitude, longitude, altitude and %battery voltage are float values. Double is available with better precision but at the cost of doubling the encoded size. 
-->

<div class="section_buttons">
 
| Previous                |
|:------------------------|
| [Hardware](hardware.md) |
 
</div>