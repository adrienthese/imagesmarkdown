#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2026-01-15 16:05:45

#include "Arduino.h"
#include <CosmoNodeV1.h>
#include <ArduinoJson.h>

void SystemClock_Config(void) ;
void gpio_init() ;
void gps_init() ;
void gps_enable() ;
void gps_disable() ;
void gps_synchronize() ;
void gps_reset() ;
bool gps_position() ;
bool gps_date() ;
bool gps_time() ;
void gps_parse() ;
void gps_nmea() ;
void gps_read() ;
void gps_rtc() ;
void kineis_init() ;
void kineis_on() ;
void kineis_off() ;
String kineis_command(String& command) ;
String kineis_answer() ;
const bool kineis_ping() ;
const uint32_t kineis_id() ;
const String kineis_fw() ;
const String kineis_sn() ;
const String kineis_fmt() ;
const uint32_t kineis_pwr() ;
const bool kineis_raw() ;
const bool kineis_standard() ;
const bool kineis_save() ;
void kineis_send() ;
String kineis_byte(const uint8_t value) ;
String kineis_word(const uint16_t value) ;
String kineis_float(const float value) ;
String kineis_data() ;
void rtc_init() ;
void rtc_output() ;
void rtc_schedule() ;
void rtc_alarm(void*) ;
void power_init() ;
void serial_init() ;
void serial_output(const char *type, const char *message) ;
void serial_error(const char *message) ;
void sd_init() ;
void sd_datetime(uint16_t *date, uint16_t *time) ;
void sd_configuration() ;
void sd_create() ;
void sd_write() ;
void sensor_init() ;
void sensor_read() ;
void sensor_output() ;
void worker_init() ;
String logger_datetime() ;
void logger_filename() ;
void setup() ;
void loop() ;

#include "CosmoNodeV1.ino"


#endif
