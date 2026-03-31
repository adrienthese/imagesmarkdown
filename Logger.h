/**
 * @file Logger.h
 * @brief Logger helper class.
 * @author Patrick Lafarguette
 * @copyright 2024-205 CNRS - Patrick Lafarguette
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#if USE_SDFAT

extern SdFat SD;

#endif

/**
 * @class Logger
 * @brief Logger object.
 *
 * This class defines a logger used to output formatted messages to one or more
 * channels.
 *
 * Channels are:
 * - Stream like a serial port.
 * - File on an SD card.
 *
 */
class Logger {

    /**
     *
     */
    using Datetime = String (*)();

public:
    /**
     * @class Channel
     * @brief Helper class to handle logger channels.
     *
     */
    class Channel {
    public:
        /**
         * @brief Enumerates all possible channels.
         */
        enum : uint8_t {
            None = 0,           /**< No channel. */
            Stream = (1 << 0),  /**< Stream channel. */
            File = (1 << 1),    /**< File channel. */
        };
    };

    class Option {
    public:
        /**
         * @brief Enumerates all possible options.
         */
        enum : uint8_t {
            None = 0,               /**< No options. */
            Datetime = (1 << 0),    /**< Datetime message. */
        };
    };

    /**
     * @brief Creates a logger object.
     *
     * Set channel to both stream and file.
     *
     * @param stream Logger stream.
     * @param filename Optional logger filename.
     * @param option Logger option.
     */
    Logger(Stream &stream = Serial, char *filename = nullptr, const uint8_t option = Option::None) {
        _channel = Channel::Stream | Channel::File;
        _option = option;
        _filename = nullptr;
        _stream = &stream;
        setFilename(filename);
    }

    /**
     * @brief Logger destructor.
     */
    ~Logger() {
        free(_filename);
    }
    /**
     * @brief Prints formatted message to active channels.
     *
     * @param format Format string.
     * @param ... Arguments for format.
     */
    void printf(const char *format, ...) {
        va_list arguments;
        // Size
        va_start(arguments, format);
        int size = vsnprintf(nullptr, 0, format, arguments) + 1;
        va_end(arguments);
        if (size > 0) {
            char *buffer = static_cast<char*>(malloc(size));
            if (buffer) {
                // Format
                va_start(arguments, format);
                vsnprintf(buffer, size, format, arguments);
                va_end(arguments);
                // Datetime
                String datetime;
                if (_option & Option::Datetime) {
                    if (_onDatetime) {
                        datetime = _onDatetime() + ": ";
                    }
                }
                if (_channel & Channel::Stream) {
                    if (_option & Option::Datetime) {
                        _stream->print(datetime);
                    }
                    _stream->print(buffer);
                }
                if (_channel & Channel::File) {
                    File file = SD.open(_filename, FILE_WRITE);
                    if (file) {
                        if (_option & Option::Datetime) {
                            file.print(datetime);
                        }
                        file.print(buffer);
                        file.close();
                    } else {
                        _stream->printf("[ERROR] can not log to file\n");
                    }
                }
                free(buffer);
            }
        }
    }

    /**
     * @brief Sets filename for file channel.
     *
     * @param filename Filename.
     */
    void setFilename(const char* filename) {
        free(_filename);
        _filename = strdup(filename);
    }

    /**
     * @brief Sets on datetime user handler.
     *
     * @param datetime Datetime handler.
     */
    void onDatetime(Datetime datetime) {
        _onDatetime = datetime;
    }

private:
    uint8_t _channel;       /**< Logger channel. */
    uint8_t _option;        /**< Logger option. */
    char *_filename;        /**< Logger filename. */
    Stream *_stream;        /**< Logger stream. */

    /** On datetime user handler. */
    Datetime _onDatetime = nullptr;
};

#endif /* LOGGER_H_ */
