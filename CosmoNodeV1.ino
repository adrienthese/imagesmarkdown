/**
 * @file CosmoNodeV1.ino
 * @brief
 * @author Patrick Lafarguette
 * @copyright 2024-2026 CNRS - Patrick Lafarguette
 */

#include <CosmoNodeV1.h>
#include <ArduinoJson.h>

#define SerialGPS Serial5
// -DENABLE_HWSERIAL5
// -DPIN_SERIAL5_RX=PF4
// -DPIN_SERIAL5_TX=PF3

#define SerialKIM SerialLP1
// -DENABLE_HWSERIALLP1
// -DPIN_SERIALLP1_RX=PB10
// -DPIN_SERIALLP1_TX=PB11

/**
 * @def FAKECALENDAR
 * @brief Fake calendar conditional define.
 *
 * If set to 0, RTC scheduling is done from the calendar file.
 *
 * If set to 1, RTC scheduling is performed every 30 seconds for testing purposes.
 *
 */
#define FAKECALENDAR    1

/**
 * @def FAKEKINEIS
 * @brief Fake Kineis conditional define.
 *
 * If set to 0, data is actually sent to Kineis.
 *
 * If set to 1, no data is sent.
 *
 */
#define FAKEKINEIS      1

/**
 * @def FAKEGPS
 * @brief Fake GPS conditional define.
 *
 * If set to 0, GPS is used to get location, date and time.
 *
 * If set to 1, GPS is not used, location is unknown. Date and time are not set.
 *
 */
#define FAKEGPS         0

/**
 * @ingroup RTC
 * @brief Instance of the RTC.
 *
 */
STM32RTC &rtc = STM32RTC::getInstance();

/**
 * @ingroup GPS
 * @brief Instance of the GPS.
 *
 */
Adafruit_GPS gps(&SerialGPS);

/**
 * @ingroup Firmware
 * @brief Instance of worker.
 */
Worker worker;

/**
 * @ingroup Logger
 * @brief Instance of logger.
 */
Logger logger(Serial, LOGFILE, Logger::Option::Datetime);

/**
 * @brief System Clock Configuration
 *
 * @note SMPS and HSE are not supported.
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = { };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { };
    RCC_CRSInitTypeDef RCC_CRSInitStruct = { };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { };

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48
            | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = 8;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }

    /** Enable the SYSCFG APB clock
     */
    __HAL_RCC_CRS_CLK_ENABLE();

    /** Configures CRS
     */
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_LSE;
    RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(
            48000000, 32768);
    RCC_CRSInitStruct.ErrorLimitValue = 34;
    RCC_CRSInitStruct.HSI48CalibrationValue = 32;

    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC
            | RCC_PERIPHCLK_ADCDAC | RCC_PERIPHCLK_DAC1 | RCC_PERIPHCLK_LPUART1
            | RCC_PERIPHCLK_SDMMC | RCC_PERIPHCLK_CLK48 | RCC_PERIPHCLK_USBPHY;
    PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_SYSCLK;
    PeriphClkInit.Dac1ClockSelection = RCC_DAC1CLKSOURCE_LSE;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
    PeriphClkInit.IclkClockSelection = RCC_CLK48CLKSOURCE_HSI48;
    PeriphClkInit.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_CLK48;
    PeriphClkInit.UsbPhyClockSelection = RCC_USBPHYCLKSOURCE_HSE;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
    HAL_SYSCFG_SetOTGPHYReferenceClockSelection(SYSCFG_OTG_HS_PHY_CLK_SELECT_1);
}

/**
 * @defgroup GPIO GPIO
 * @brief GPIO functions.
 *
 * @{
 */

/**
 * @brief Initializes GPIOs.
 *
 */
void gpio_init() {
    // KIM1
    pinMode(KIM1_ON, OUTPUT);
    pinMode(KIM1_POWER, OUTPUT);
    // Battery
    pinMode(VBAT, INPUT);
    analogReadResolution(12);
    // GPS
    pinMode(GPS_ENABLE, OUTPUT);
}

/** @}*/

/**
 * @defgroup GPS GPS
 * @brief GPS functions.
 *
 * @{
 */

/**
 * @brief Initializes GPS device.
 *
 * - Enables GPS.
 * - Switches to 115200 bauds.
 * - Waits for a GPS fix.
 * - Disables GPS to save battery power.
 *
 */
#if 1
void gps_init() {
    gps_enable();
//    delay(500);
    gps.begin(9600);
//    gps.sendCommand(PMTK_SET_BAUD_115200);
//    SerialGPS.end();
//    gps.begin(115200);
    /**
     * PMTK_SET_NMEA_OUTPUT_RMCGGA fix possible without date.
     */
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    /**
     * PMTK_SET_NMEA_OUTPUT_RMCONLY no altitude.
     */
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    gps_synchronize();
    gps_disable();
    SerialGPS.flush();
    SerialGPS.end();
}
#endif

#if 0
void gps_init() {
    gps_enable();
    if (worker.gps == false) {
        gps.begin(9600);
        worker.gps = true;
    }
    /**
     * PMTK_SET_NMEA_OUTPUT_RMCGGA fix possible without date.
     */
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    /**
     * PMTK_SET_NMEA_OUTPUT_RMCONLY no altitude.
     */
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    gps_synchronize();
}
#endif

/**
 * @brief Enables GPS.
 *
 */
void gps_enable() {
    digitalWrite(GPS_ENABLE, LOW);
}

/**
 * @brief Disables GPS.
 *
 */
void gps_disable() {
    digitalWrite(GPS_ENABLE, HIGH);
}

/**
 * @brief Waits for GPS synchronization.
 *
 */
void gps_synchronize() {
    gps_reset();
    while ((gps_position() == false) || (gps_date() == false) || (gps_time() == false)) {
        gps_parse();
    }
    gps_read();
}

/**
 * @brief Resets GPS data.
 *
 */
void gps_reset() {
    gps.fix = false;
    gps.year = 0;
    gps.month = 0;
    gps.day = 0;
    gps.hour = 0;
    gps.minute = 0;
    gps.seconds = 0;
}

/**
 * @brief Check if GPS has position.
 *
 * @return True is GPS has a valid position, false otherwise.
 */
bool gps_position() {
    return gps.fix;
}

/**
 * @brief Check if GPS has date.
 *
 * @return True is GPS has a valid date, false otherwise.
 */
bool gps_date() {
    if (gps.year >= worker.date.year) {
        if (gps.month >= worker.date.month) {
            if (gps.day >= worker.date.day) {
                return gps.year + gps.month + gps.day;
            }
        }
    }
    return false;
}

/**
 * @brief Check if GPS has time.
 *
 * @return True is GPS has a valid time, false otherwise.
 */
bool gps_time() {
    return gps.hour + gps.minute + gps.seconds;
}
/**
 * @brief Reads and parses NMEA sentences.
 *
 */
void gps_parse() {
    while (gps.newNMEAreceived() == false) {
        gps.read();
    }
    gps_nmea();
    gps.parse(gps.lastNMEA());
}

/**
 * @brief Outputs last NMEA sentence.
 *
 * Only used to monitor GPS activity, set conditional define to 0 to disable.
 *
 */
void gps_nmea() {
#if 1
    String nmea(gps.lastNMEA());
    nmea.trim();
    logger.printf("[GPS] nmea %s\n", nmea.c_str());
#endif
}

/**
 * @brief Reads date, time and location from GPS.
 *
 * - Refreshes worker with current date, time and location from GPS.
 * - Updates RTC.
 *
 */
void gps_read() {
    logger.printf("[GPS] read date, time and position\n");
    // Date
    worker.date.year = gps.year;
    worker.date.month = gps.month;
    worker.date.day = gps.day;
    rtc.setDate(worker.date.day, worker.date.month, worker.date.year);
    // Time
    worker.time.hour = gps.hour;
    worker.time.minute = gps.minute;
    worker.time.seconds = gps.seconds;
    worker.time.milliseconds = gps.milliseconds;
    rtc.setTime(worker.time.hour, worker.time.minute, worker.time.seconds, worker.time.milliseconds);
    // Location
    worker.location.latitude = gps.latitudeDegrees;
    worker.location.longitude = gps.longitudeDegrees;
    worker.location.altitude = gps.altitude;
    // Output
    logger.printf("[GPS] latitude %.3f\n", worker.location.latitude);
    logger.printf("[GPS] longitude %.3f\n", worker.location.longitude);
    logger.printf("[GPS] altitude %.3f\n", worker.location.altitude);
    logger.printf("[RTC] set date and time to %02d/%02d/%02d %02d:%02d:%02d\n", rtc.getDay(),
            rtc.getMonth(), rtc.getYear(), rtc.getHours(), rtc.getMinutes(),
            rtc.getSeconds());
}

/**
 * @brief Updates RTC date and time.
 *
 * @deprecated
 *
 */
void gps_rtc() {
    // Date
    rtc.setDate(worker.date.day, worker.date.month, worker.date.year);
    // Time
    rtc.setTime(worker.time.hour, worker.time.minute, worker.time.seconds, worker.time.milliseconds);
    // Output
    logger.printf("[RTC] set date and time to %02d/%02d/%02d %02d:%02d:%02d\n", rtc.getDay(),
            rtc.getMonth(), rtc.getYear(), rtc.getHours(), rtc.getMinutes(),
            rtc.getSeconds());
}

/** @}*/

/**
 * @defgroup Kineis Kineis
 * @brief Kineis KIM1 functions.
 *
 * @{
 */

/**
 * @brief Initializes KIM1.
 *
 * Executes commands to check if the module is functional.
 *
 * - Powers on KIM1.
 * - Sends PING command.
 * - Reads ID.
 * - Reads firmware version.
 * - Reads serial number.
 * - Powers off KIM1 to save battery power.
 *
 * ```
 * 01/01/00 00:00:00: [KIM1] ping OK
 * 01/01/00 00:00:00: [KIM1] id 271AF8B
 * 01/01/00 00:00:00: [KIM1] fw KIM1_V2.1
 * 01/01/00 00:00:00: [KIM1] sn KIM13211200248
 * 01/01/00 00:00:00: [KIM1] fmt 0,0,0
 * 01/01/00 00:00:00: [KIM1] pwr 1000
 * ```
 *
 */
void kineis_init() {
    bool update = false;
    kineis_on();
    bool ping = kineis_ping();
    uint32_t id = kineis_id();
    String fw = kineis_fw();
    String sn = kineis_sn();
    String fmt = kineis_fmt();
    uint32_t pwr = kineis_pwr();
    logger.printf("[KIM1] ping %s\n", ping ? "OK" : "KO");
    logger.printf("[KIM1] id %X\n", id);
    logger.printf("[KIM1] fw %s\n", fw.c_str());
    logger.printf("[KIM1] sn %s\n", sn.c_str());
    logger.printf("[KIM1] fmt %s\n", fmt.c_str());
    logger.printf("[KIM1] pwr %u\n", pwr);
    switch(worker.data.format) {
    case Format::Raw:
        if (fmt.equals("0,0,0") == false) {
            bool raw = kineis_raw();
            logger.printf("[KIM1] fmt raw %s\n", raw ? "OK" : "KO");
            update |= raw;
        }
        break;
    case Format::Standard:
        if (fmt.equals("1,16,32") == false) {
            bool standard = kineis_standard();
            logger.printf("[KIM1] fmt standard %s\n", standard ? "OK" : "KO");
            update |= standard;
        }
        break;
    }
    if (update) {
        bool save = kineis_save();
        logger.printf("[KIM1] save configuration %s\n", save ? "OK" : "KO");
    }
    kineis_off();
}

/**
 * @brief Powers on KIM1.
 *
 * Opens and setup the serial device then powers and switches on KIM1.
 *
 */
void kineis_on() {
    SerialKIM.begin(9600, SERIAL_8N1);
    SerialKIM.flush();
    digitalWrite(KIM1_POWER, HIGH);
    digitalWrite(KIM1_ON, HIGH);
    SerialKIM.readString();
}

/**
 * @brief Powers off KIM1.
 *
 * Powers and switches off KIM1 and closes the serial device.
 *
 */
void kineis_off() {
    digitalWrite(KIM1_ON, LOW);
    digitalWrite(KIM1_POWER, LOW);
    SerialKIM.end();
}

/**
 * @brief Sends a command to KIM1.
 *
 * @param command Command to send.
 * @return Answer from KIM1.
 */
String kineis_command(String& command) {
    SerialKIM.print(command);
    delay(KIM1_DELAY);
    return kineis_answer();
}

/**
 * @brief Reads answer from KIM1.
 *
 * @return Answer from KIM1.
 */
String kineis_answer() {
    String answer = SerialKIM.readString();
    answer.trim();
    return answer;
}
/**
 * @brief Sends PING command to KIM1.
 *
 * @return True if success, false on error.
 */
const bool kineis_ping() {
    String command("AT+PING=?\r\n");
    String ping = kineis_command(command);
    return ping.equals("+OK");
}

/**
 * @brief Reads ID from KIM1.
 *
 * @return KIM1 identifier.
 */
const uint32_t kineis_id() {
    String command("AT+ID=?\r\n");
    String id = kineis_command(command);
    return strtol(id.substring(id.indexOf('=') + 1).c_str(), nullptr, HEX);
}

/**
 * @brief Reads firmware version from KIM1.
 *
 * @return Firmware version.
 */
const String kineis_fw() {
    String command("AT+FW=?\r\n");
    String fw = kineis_command(command);
    return fw.substring(fw.indexOf('=') + 1);
}

/**
 * @brief Reads serial number from KIM1.
 *
 * @return Serial number.
 */
const String kineis_sn() {
    String command("AT+SN=?\r\n");
    String sn = kineis_command(command);
    return sn.substring(sn.indexOf('=') + 1);
}

/**
 * @brief Reads message format from KIM1.
 *
 * @return Message format.
 */
const String kineis_fmt() {
    String command("AT+AFMT=?\r\n");
    String fmt = kineis_command(command);
    return fmt.substring(fmt.indexOf('=') + 1);
}

/**
 * @brief Reads transmission power from KIM1.
 *
 * @return Transmission power.
 */
const uint32_t kineis_pwr() {
    String command("AT+PWR=?\r\n");
    String pwr = kineis_command(command);
    return strtol(pwr.substring(pwr.indexOf('=') + 1).c_str(), nullptr, DEC);
}

/**
 * @brief Sets KIM1 message format to raw.
 *
 * @return True if success, false on error.
 */
const bool kineis_raw() {
    String command("AT+AFMT=0,0,0\r\n");
    String raw = kineis_command(command);
    return raw.equals("+OK");
}

/**
 * @brief Sets KIM1 message format to standard.
 *
 * @return True if success, false on error.
 */
const bool kineis_standard() {
    String command("AT+AFMT=1,16,32\r\n");
    String raw = kineis_command(command);
    return raw.equals("+OK");
}

/**
 * @brief Saves KIM1 configuration to flash memory.
 *
 * @return True if success, false on error.
 */
const bool kineis_save() {
    String command("AT+SAVE_CFG\r\n");
    String save = kineis_command(command);
    return save.equals("+OK");
}

/**
 * @brief Sends data to Kineis.
 *
 */
void kineis_send() {
    kineis_on();
    // AT+TX=<data>
    // +OK
    String command = "AT+TX=" + kineis_data() + "\r\n";
    String ok = kineis_command(command);
    command.trim();
    logger.printf("[KIM1] command %s\n", command.c_str());
    ok.trim();
    logger.printf("[KIM1] tx %s\n", ok.c_str());
    auto timeout = SerialKIM.getTimeout();
#if 0
    logger.printf("[KIM1] timeout %d\n", timeout);
#endif
    SerialKIM.setTimeout(KIM1_TIMEOUT);
    // +TX=0,<data>
    String answer = kineis_answer();
    SerialKIM.setTimeout(timeout);
    logger.printf("[KIM1] tx %s\n", answer.c_str());
    kineis_off();
}

/**
 * @brief Formats a byte for Kineis.
 *
 * @param value Byte to format.
 * @return Byte as an hexadecimal string.
 */
String kineis_byte(const uint8_t value) {
    char buffer[sizeof(uint8_t) * 2 + 1];
    sprintf(buffer, "%02X", value);
    return String(buffer);
}

/**
 * @brief Formats a word for Kineis.
 *
 * @param value Word to format.
 * @return Word as an hexadecimal string.
 */
String kineis_word(const uint16_t value) {
    char buffer[sizeof(uint16_t) * 2 + 1];
    sprintf(buffer, "%04X", value);
    return String(buffer);
}

/**
 * @brief Formats a float for Kineis.
 *
 * @param value Float to format.
 * @return Float as an hexadecimal string.
 */

String kineis_float(const float value) {
    char buffer[sizeof(float) * 2 + 1];
    sprintf(buffer, "%08X", *(uint32_t*) &value);
    return String(buffer);
}

/**
 * @brief Builds Kineis data.
 *
 * @return Data as an hexadecimal string.
 */
String kineis_data() {
    String data;
    switch (worker.data.format) {
    case Format::Raw:
        data.reserve(27 * 2 + 1);
        switch (worker.data.frame) {
        case Frame::Standard:
            data += kineis_byte(worker.node);
            data += kineis_word(worker.counter);
            data += kineis_byte(rtc.getYear());
            data += kineis_byte(rtc.getMonth());
            data += kineis_byte(rtc.getDay());
            data += kineis_byte(rtc.getHours());
            data += kineis_byte(rtc.getMinutes());
            data += kineis_byte(rtc.getSeconds());
            data += kineis_word(worker.tmp102.raw);
            data += kineis_float(worker.location.latitude);
            data += kineis_float(worker.location.longitude);
            data += kineis_float(worker.location.altitude);
            data += kineis_float(worker.battery.voltage);
            break;
        case Frame::Special:
            for (auto index = 0; index < 54; ++index) {
                data += '5';
            }
            break;
        }
        break;
    case Format::Standard:
        data.reserve(23 * 2 + 1);
        switch (worker.data.frame) {
        case Frame::Standard:
//            27
//            data += kineis_byte(worker.node);
//            -1
//            26
//            data += kineis_word(worker.counter);
//            -1
//            25
            data += kineis_byte(worker.counter);
//            data += kineis_byte(rtc.getYear());
//            data += kineis_byte(rtc.getMonth());
//            data += kineis_byte(rtc.getDay());
//            -1
//            24
            data += kineis_word((rtc.getYear() - 1980) << 9 | rtc.getMonth() << 5 | rtc.getDay());
//            data += kineis_byte(rtc.getHours());
//            data += kineis_byte(rtc.getMinutes());
//            data += kineis_byte(rtc.getSeconds());
//            -1
//            23
            data += kineis_word(rtc.getHours() << 11 | rtc.getMinutes() << 5 | rtc.getSeconds() >> 1);
            data += kineis_word(worker.tmp102.raw);
            data += kineis_float(worker.location.latitude);
            data += kineis_float(worker.location.longitude);
            data += kineis_float(worker.location.altitude);
            data += kineis_float(worker.battery.voltage);
            break;
        case Frame::Special:
            for (auto index = 0; index < 46; ++index) {
                data += '5';
            }
            break;
        }
        break;
    }
    return data;
}

/** @}*/

/**
 * @defgroup RTC RTC
 * @brief RTC functions.
 *
 * @{
 */

/**
 * @brief Initializes RTC.
 *
 * - Set clock source to LSE.
 * - Update date and time from GPS.
 *
 */
void rtc_init() {
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin();
#if FAKEGPS
#else
    gps_init();
#endif
}

/**
 * @brief Outputs RTC epoch.
 *
 * @deprecated
 *
 */
void rtc_output() {
    Serial.printf("[RTC] epoch %d\n", rtc.getEpoch());
}

/**
 * @brief Schedules wake up.
 *
 * Wake up when a satellite is in view and data can be sent.
 *
 * Date and time of the next wake up is read from a calendar file on the SD card.
 *
 * An alarm is set so the board can enter low power mode.
 *
 */
void rtc_schedule() {
#if FAKECALENDAR
    worker.calendar.alarm = rtc.getEpoch() + 30;
    rtc.setAlarmEpoch(worker.calendar.alarm, STM32RTC::MATCH_YYMMDDHHMMSS, ALARM_A);
    rtc.enableAlarm(STM32RTC::MATCH_YYMMDDHHMMSS);
#else
    File file = SD.open(worker.calendar.file);
    if (file) {
        file.seek(worker.calendar.position);
        while (file.available() && (rtc.getEpoch() > worker.calendar.alarm)) {
            worker.calendar.alarm = file.parseInt();
        }
        worker.calendar.position = file.position();
        bool eof = file.position() == file.size();
        file.close();
        if (eof) {
            logger.printf("[ERROR] no more data in calendar file\n");
        } else {
            rtc.setAlarmEpoch(worker.calendar.alarm, STM32RTC::MATCH_YYMMDDHHMMSS, ALARM_A);
            rtc.enableAlarm(STM32RTC::MATCH_YYMMDDHHMMSS);
            tm* alarm = gmtime(&worker.calendar.alarm);
            // Output
            logger.printf("[RTC] set alarm to %02d/%02d/%02d %02d:%02d:%02d %lu\n",
                    alarm->tm_mday, alarm->tm_mon + 1, alarm->tm_year - 100,
                    alarm->tm_hour, alarm->tm_min, alarm->tm_sec, worker.calendar.alarm);
        }
    } else {
        serial_error("unable to open calendar file");
    }
#endif
}

/**
 * @brief RTC alarm handler.
 *
 */
void rtc_alarm(void*) {
    worker.wakeup = true;
}

/** @}*/

/**
 * @defgroup Power Power
 * @brief Power functions.
 *
 * @{
 */

/**
 * @brief Initializes power system.
 *
 * Enables wake up from RTC.
 *
 */
void power_init() {
    LowPower.begin();
    LowPower.enableWakeupFrom(&rtc, rtc_alarm);
}

/** @}*/

/**
 * @defgroup Serial Serial
 * @brief Serial functions.
 *
 * Serial is used to output messages.
 *
 * @{
 */

/**
 * @brief Initializes serial.
 *
 * Opens serial at 921600 bauds.
 *
 * Outputs some hardware and software informations.
 *
 */
void serial_init() {
    Serial.begin(921600);
    Serial.println("[BOOT] CosmoNode");
    Serial.print("[BOOT] firmware ");
    Serial.print(FIRMWARE_VERSION_MAJOR);
    Serial.print(".");
    Serial.print(FIRMWARE_VERSION_MINOR);
    Serial.print(".");
    Serial.println(FIRMWARE_VERSION_PATCH);
    Serial.print("[BOOT] ");
    Serial.print(__DATE__);
    Serial.print(", ");
    Serial.println(__TIME__);
    Serial.print("[BOOT] core ");
    Serial.print(STM32_CORE_VERSION_MAJOR);
    Serial.print(".");
    Serial.print(STM32_CORE_VERSION_MINOR);
    Serial.print(".");
    Serial.println(STM32_CORE_VERSION_PATCH);
    Serial.print("[BOOT] board ");
#if defined(ARDUINO_NUCLEO_L073RZ)
    Serial.println("NUCLEO-L073RZ");
#endif
#if defined(ARDUINO_NUCLEO_U5A5ZJ_Q)
    Serial.println("NUCLEO-U5A5ZJ-Q");
#endif
    Serial.print("[BOOT] CPU frequency ");
    Serial.print(0.000001 * F_CPU);
    Serial.println(" MHz");
    Serial.println("[BOOT] setup");
}

/**
 * @brief Outputs a message to serial.
 *
 * @deprecated
 *
 * @param type Type
 * @param message Message
 */
void serial_output(const char *type, const char *message) {
    Serial.printf("[%s] %s\n", type, message);
}

/**
 * @brief Outputs an error message to serial.
 *
 * @deprecated
 *
 * @param message Error message.
 */
void serial_error(const char *message) {
    serial_output("ERROR", message);
}

/** @}*/

/**
 * @defgroup SD SD
 * @brief SD functions.
 *
 * SD is used to store data files.
 *
 * @{
 */

/**
 * @brief Initializes SD.
 *
 */
void sd_init() {
//    SdFile::dateTimeCallback(sd_datetime);
    if (SD.begin(PC13)) {
        logger.printf("[SD] initialized\n");
        sd_configuration();
        sd_create();
    } else {
        logger.printf("[ERROR] SD can not be initialized\n");
    }
}

/**
 * @brief SD datetime handler.
 *
 * @param date Pointer to date.
 * @param time Pointer to time.
 */
void sd_datetime(uint16_t *date, uint16_t *time) {
//    *date = FAT_DATE((uint16_t)rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay());
//    *time = FAT_TIME(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
}

/**
 * @brief
 *
 */
void sd_configuration() {
    const char *FORMAT[] = { "raw", "standard" };
    const char *FRAME[] = { "standard", "special" };
    if (SD.exists(JSONFILE)) {
        File file = SD.open(JSONFILE, FILE_READ);
        if (file) {
            JsonDocument document;
            DeserializationError error = deserializeJson(document, file);
            file.close();
            if (error == DeserializationError::Ok) {
                logger.printf("[CFG] JSON file read\n");
                worker.timeout = document["timeout"] | KIM1_TIMEOUT;
                if (document["calendar"]["file"]) {
                    worker.calendar.file = strdup(static_cast<const char*>(document["calendar"]["file"]));
                }
                if (document["data"]["format"]) {
                    worker.data.format = document["data"]["format"] | Format::Raw;
                }
                if (document["data"]["frame"]) {
                    worker.data.frame = document["data"]["frame"] | Frame::Standard;
                }
                if (document["data"]["file"]) {
                    worker.data.file = strdup(static_cast<const char*>(document["data"]["file"]));
                }
            } else {
                logger.printf("[ERROR] JSON file can not be parsed\n");
            }
        } else {
            logger.printf("[ERROR] JSON file can not be read\n");
        }
    } else {
        logger.printf("[ERROR] JSON file not found\n");
    }
    logger.printf("[CFG] timeout %u ms\n", worker.timeout);
    logger.printf("[CFG] calendar file %s\n", worker.calendar.file);
    logger.printf("[CFG] data format %s\n", FORMAT[worker.data.format]);
    logger.printf("[CFG] data frame %s\n", FRAME[worker.data.frame]);
    logger.printf("[CFG] data file %s\n", worker.data.file);
}

/**
 * @brief Creates data file.
 *
 */
void sd_create() {
    if (SD.exists(worker.data.file) == false) {
        File file = SD.open(worker.data.file, FILE_WRITE);
        if (file) {
            file.printf("Node;Counter;Date;Time;Temperature;Latitude;Longitude;Altitude;Battery voltage\n");
            file.close();
            logger.printf("[SD] data file %s created\n", worker.data.file);
        } else {
            logger.printf("[ERROR] data file %s can not be created\n", worker.data.file);
        }
    }
}

/**
 * @brief Outputs data to file.
 *
 * ```
 * 1 ; 1 ; 28/6/24 ; 12:26:11 ; 27,48 ; 16.01, -61.72, 268.30 ; 4.46
 * 1 ; 2 ; 28/6/24 ; 12:27:11 ; 27,48 ; 16.01, -61.72, 268.30 ; 4.47
 * 1 ; 3 ; 28/6/24 ; 12:28:11 ; 27,48 ; 16.01, -61.72, 268.30 ; 4.46
 * 1 ; 4 ; 28/6/24 ; 12:29:11 ; 27,64 ; 16.01, -61.72, 268.30 ; 4.45
 * ```
 *
 */
void sd_write() {
    // TODO write worker.calendar.alarm and rtc.getEpoch()
    File file = SD.open(worker.data.file, FILE_WRITE);
    if (file) {
        // Node
        file.printf("%04d;", worker.node);
        // Counter
        file.printf("%05d;", worker.counter);
        // Datetime
        file.printf("%02d/%02d/%02d;", rtc.getDay(), rtc.getMonth(), rtc.getYear());
        file.printf("%02d:%02d:%02d;", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
        // Temperature
        file.printf("%.2f;", worker.tmp102.celsius);
        // Location
        file.printf("%.2f;%.2f;%.2f;", worker.location.latitude, worker.location.longitude ,worker.location.altitude);
        // Battery
        file.printf("%.2f\n", worker.battery.voltage);
        file.close();
        logger.printf("[SD] data file %s written\n", worker.data.file);
    } else {
        logger.printf("[ERROR] data file %s can not be written\n", worker.data.file);
    }
}

/** @}*/

/**
 * @defgroup Sensor Sensor
 * @brief Sensor functions.
 *
 * @{
 */

/**
 * @brief Initializes sensor.
 *
 */
void sensor_init() {
    Wire.begin();
    sensor_read();
    sensor_output();
}

/**
 * @brief Reads sensor.
 *
 */
void sensor_read() {
    // Read TMP102
    Wire.requestFrom(TMP102_ADDRESS, 2);
    // 12-bit signed raw temperature
    worker.tmp102.raw = Wire.read();
    worker.tmp102.raw <<= 8;
    worker.tmp102.raw |= Wire.read();
    worker.tmp102.raw >>= 4;
    // Convert to °C
    worker.tmp102.celsius = worker.tmp102.raw * 0.0625;
    // Read battery voltage
    worker.battery.voltage = analogRead(VBAT) * 3.3 * 5 / 3.16 / 4095.0;
}

/**
 * @brief Outputs sensor.
 *
 */
void sensor_output() {
    logger.printf("[TMP102] temperature %.2f °C 0x%04X\n", worker.tmp102.celsius, worker.tmp102.raw);
    logger.printf("[BATTERY] voltage %.2f V\n", worker.battery.voltage);
}

/** @}*/

/**
 * @defgroup Firmware Firmware
 * @brief Firmware functions.
 *
 * @{
 */

/**
 * @brief Initializes worker.
 *
 * - Resets wake up flag.
 * - Sets node to 1.
 * - Resets counter.
 * - Resets alarm flag.
 * - Resets read position of calendar.
 *
 */
void worker_init() {
    worker.wakeup = false;
    worker.gps = false;
    worker.node = 1;
    worker.counter = 0;
    worker.timeout = KIM1_TIMEOUT;
    worker.calendar.file = CALENDARFILE;
    worker.data.format = Format::Raw;
    worker.data.frame = Frame::Standard;
    worker.data.file = CSVFILE;
    worker.calendar.alarm = 0;
    worker.calendar.position = 0;
}

/** @}*/

/**
 * @brief Formats RTC datetime as string.
 *
 * @return Datetime as String.
 */
String logger_datetime() {
    String datetime;
    int size = snprintf(nullptr, 0, "%02d/%02d/%02d %02d:%02d:%02d",
            rtc.getDay(), rtc.getMonth(), rtc.getYear(), rtc.getHours(),
            rtc.getMinutes(), rtc.getSeconds()) + 1;
    if (size > 0) {
        char *buffer = static_cast<char*>(malloc(size));
        if (buffer) {
            sprintf(buffer, "%02d/%02d/%02d %02d:%02d:%02d", rtc.getDay(),
                    rtc.getMonth(), rtc.getYear(), rtc.getHours(),
                    rtc.getMinutes(), rtc.getSeconds());
            datetime.concat(buffer);
            free(buffer);
        }
    }
    return datetime;
}

/**
 * @brief Sets logger filename from RTC date.
 *
 * Logger uses a log file per day.
 *
 */
void logger_filename() {
    int size = snprintf(nullptr, 0, "%04d%02d%02d.log", rtc.getYear() + 2000,
            rtc.getMonth(), rtc.getDay()) + 1;
    if (size > 0) {
        char *buffer = static_cast<char*>(malloc(size));
        if (buffer) {
            sprintf(buffer, "%04d%02d%02d.log", rtc.getYear() + 2000,
                    rtc.getMonth(), rtc.getDay());
            logger.setFilename(buffer);
            free(buffer);
        }
    }
}

/**
 * @defgroup Arduino Arduino
 * @brief Arduino functions.
 *
 * @{
 */

/**
 * @brief Setup function.
 *
 * Initializes software and hardware.
 */
void setup() {
    // Logger
    logger.onDatetime(logger_datetime);
    // Worker
    worker_init();
    // Serial
    serial_init();
#if 0
    String data = kineis_data();
    Serial.println(data);
#endif
    // GPIO
    gpio_init();
    // SD
    sd_init();
    // Sensor
    sensor_init();
    // KIM1
    kineis_init();
    // Power
    power_init();
    // RTC
    rtc_init();
    rtc_schedule();
    logger_filename();
}

/**
 * @brief Loop function.
 *
 * Executes software tasks.
 */
void loop() {
#if 0
    logger.printf("[LOOP] RTC force refresh\n");
    gps_init();
    delay(60000 * 5);
    return;
#endif
    if (worker.wakeup) {
        worker.calendar.wakeup = rtc.getEpoch();
        logger.printf("[LOOP] wake up at %lu (delta %lu)\n",
                worker.calendar.wakeup,
                worker.calendar.wakeup - worker.calendar.alarm);
        worker.wakeup = false;
        worker.counter++;
        sensor_read();
#if FAKEKINEIS
#else
        kineis_send();
#endif
        sd_write();
        rtc_schedule();
    }
    if (rtc.getDay() != worker.date.day) {
#if 0
        logger.printf("[LOOP] RTC year %d\n", rtc.getYear());
        logger.printf("[LOOP] GPS year %d\n", worker.date.year);
        logger.printf("[LOOP] RTC month %d\n", rtc.getMonth());
        logger.printf("[LOOP] GPS month %d\n", worker.date.month);
        logger.printf("[LOOP] RTC day %d\n", rtc.getDay());
        logger.printf("[LOOP] GPS day %d\n", worker.date.day);
#endif
        logger_filename();
        if ((worker.calendar.alarm - rtc.getEpoch()) > 300) {
            logger.printf("[LOOP] RTC refresh\n");
            gps_init();
        }
    }
    Serial.flush();
    if (worker.calendar.alarm) {
        LowPower.deepSleep();
    }
}

/** @}*/
