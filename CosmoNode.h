/**
 * @file CosmoNode.h
 * @brief
 * @author Patrick Lafarguette
 * @copyright 2024-2025 CNRS - Patrick Lafarguette
 *
 */

#ifndef COSMONODE_H_
#define COSMONODE_H_

#include <Arduino.h>

/**
 * @addtogroup Power
 * @{
 */

#include <STM32LowPower.h>

/** @}*/

/**
 * @addtogroup RTC
 * @{
 */

#include <STM32RTC.h>

/** @}*/

/**
 * @addtogroup GPS
 * @{
 */

#include <Adafruit_GPS.h>

/**
 * @def GPS_ENABLE
 * @brief MCU output pin connected to GPS enable signal.
 *
 * Active low.
 *
 */
#define GPS_ENABLE              PA8

/** @}*/

/**
 * @addtogroup Kineis
 * @{
 */

/**
 * @def KIM1_POWER
 * @brief MCU output pin connected to Kineis power line.
 *
 */
#define KIM1_POWER              PB5

/**
 * @def KIM1_DELAY
 * @brief Minimum time to wait before issuing the next command.
 * @see 3.2. Timing constraints, Integration manual, page 10.
 *
 */
#define KIM1_DELAY              10

/**
 * @def KIM1_TIMEOUT
 * @brief Timeout to read answer to TX command.
 *
 */
#define KIM1_TIMEOUT            5000

/** @}*/
/**
 * @addtogroup Sensor
 * @{
 */

#include <Wire.h>

/**
 * @def TMP102_ADDRESS
 * @brief TMP102 temperature sensor I²C address.
 *
 */
#define TMP102_ADDRESS          0x48

/**
 * @struct TMP102
 * @brief TMP102 helper struct.
 *
 */
struct TMP102 {
    int16_t raw;            /**< Temperature raw value */
    float celsius;          /**< Temperature in °C */
};

/**
 * @def VBAT
 * @brief MCU input pin connected to the battery.
 *
 */
#define VBAT                    PA0

/**
 * @struct Battery
 * @brief Battery helper struct.
 *
 */
struct Battery {
    float voltage;          /**< %Battery voltage */
};

/** @}*/

/**
 * @addtogroup SD
 * @{
 */

#if USE_SDFAT
/**
 * @def INCLUDE_SDIOS
 * @brief Includes stream functions to file.
 *
 */
#define INCLUDE_SDIOS 1

#define USE_SPI_ARRAY_TRANSFER 4

#include <SdFat.h>

/**
 * @def JSONFILE
 */
constexpr char *JSONFILE = "cosmonode.json";

/**
 * @def CSVFILE
 */
constexpr char *CSVFILE = "cosmonode.csv";

/**
 * @def LOGFILE
 */
constexpr char *LOGFILE = "cosmonode.log";

/**
 * @def CALENDARFILE
 */
constexpr char *CALENDARFILE = "calendar.txt";

#else

/**
 */
#include <SD.h>

/**
 * @def JSONFILE
 */
constexpr char *JSONFILE = "cosmonod.jsn";

/**
 * @def CSVFILE
 */
constexpr char *CSVFILE = "cosmonod.csv";

/**
 * @def LOGFILE
 */
constexpr char *LOGFILE = "cosmonod.log";

/**
 * @def CALENDARFILE
 */
constexpr char *CALENDARFILE = "cosmonod.cal";

#endif

/** @}*/

/**
 * @addtogroup Logger
 * @{
 */

#include <Logger.h>

/** @}*/

/**
 * @addtogroup Firmware
 * @{
 */

/**
 * @struct Date
 * @brief Date helper struct.
 *
 */
struct Date {
    uint8_t year;           /**< UTC year */
    uint8_t month;          /**< UTC month */
    uint8_t day;            /**< UTC day */
};

/**
 * @struct Time
 * @brief Time helper struct.
 *
 */
struct Time {
    uint8_t hour;           /**< UTC hour */
    uint8_t minute;         /**< UTC minute */
    uint8_t seconds;        /**< UTC seconds */
    uint16_t milliseconds;  /**< UTC milliseconds */
};

/**
 * @struct Location
 * @brief Location helper struct.
 *
 */
struct Location {
    float latitude;         /**< Latitude */
    float longitude;        /**< Longitude */
    float altitude;         /**< Altitude in meters */
};

/**
 * @struct Calendar
 * @brief Calendar helper struct.
 *
 */
struct Calendar {
    uint32_t position;      /**< Read position in calendar file */
    time_t alarm;           /**< Alarm epoch from calendar file */
    time_t wakeup;          /**< Wake up epoch */
    time_t data;            /**< Build message epoch */
    time_t send;            /**< Send message epoch */
    char *file;             /**< Calendar file name */
};

/**
 * @struct Format
 * @brief Helper struct to handle message format.
 *
 * The application can use raw or standard format.
 *
 * Raw format is only data.
 *
 * Standard format add CRC and error correction to data.
 *
 */
struct Format {
    /**
     * @brief Enumerates all supported message format.
     *
     */
    enum: uint8_t {
        Raw,                /**< Raw format */
        Standard,           /**< Standard format with CRC and error correction */
    };
};

/**
 * @struct Frame
 * @brief Helper struct to handle frame.
 *
 * The application can send standard or special frame.
 *
 * Standard frame handles real data.
 *
 * Special frame allows to evaluate the error rate.
 *
 */
struct Frame {
    /**
     * @brief Enumerates all supported frame.
     *
     */
    enum: uint8_t {
        Standard,/**< Standard frame */
        Special, /**< Special frame with pattern */
    };
};

/**
 * @struct Data
 * @brief Data helper struct.
 *
 */
struct Data {
    uint8_t format;         /**< Data format */
    uint8_t frame;          /**< Data frame */
    char *file;             /**< Data file name */
};

/**
  * @struct Worker
  * @brief Worker helper struct.
  *
  */
struct Worker {
    volatile bool wakeup;   /**< Flag set on wake up */
    bool gps;               /**< Flag set when GPS is initialized */
    uint16_t node;          /**< Node identifier */
    uint16_t counter;       /**< Generic purpose counter */
    uint16_t timeout;       /**< Timeout for communication with KIM1 */
    Date date;              /**< Application date */
    Time time;              /**< Application time */
    Location location;      /**< Application location */
    Calendar calendar;      /**< Application calendar */
    Data data;              /**< Application data */
    TMP102 tmp102;          /**< TMP102 sensor */
    Battery battery;        /**< %Battery information */
};

/**
 * @brief Application version number
 */
#define FIRMWARE_VERSION_MAJOR      (0x00U) /**< [31:24] major version */
#define FIRMWARE_VERSION_MINOR      (0x06U) /**< [23:16] minor version */
#define FIRMWARE_VERSION_PATCH      (0x00U) /**< [15:8]  patch version */

#define FIRMWARE_VERSION            ((FIRMWARE_VERSION_MAJOR << 16U)\
                                        |(FIRMWARE_VERSION_MINOR << 8U)\
                                        |(FIRMWARE_VERSION_PATCH))

/** @}*/

#endif /* COSMONODE_H_ */
