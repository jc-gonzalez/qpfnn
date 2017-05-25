/******************************************************************************
 * File:    log.h
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.Log
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Define Log main class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef LOG_H
#define LOG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - map
//   - vector
//   - fstream
//------------------------------------------------------------
#include <map>
#include <vector>
#include <fstream>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

//============================================================
// Group: Public Declarations
//============================================================

//------------------------------------------------------------
// Topic: Macros
//   - LogMsg(l, s) - Generic macro to call logMsg method
//   - TraceMsg(s)  - Macro to call logMsg for TRACE msgs
//   - DbgMsg(s)    - Macro to call logMsg for DEBUG msgs
//   - InfoMsg(s)   - Macro to call logMsg for INFO msgs
//   - WarnMsg(s)   - Macro to call logMsg for WARNING msgs
//   - ErrMsg(s)    - Macro to call logMsg for ERROR msgs
//   - FatalMsg(s)  - Macro to call logMsg for FATAL msgs
//------------------------------------------------------------

#define LogMsg(l,s)  Log::log(Log::System, l, s)
#define TraceMsg(s)  Log::log(Log::System, Log::TRACE,   s)
#define DbgMsg(s)    Log::log(Log::System, Log::DEBUG,   s)
#define InfoMsg(s)   Log::log(Log::System, Log::INFO,    s)
#define WarnMsg(s)   Log::log(Log::System, Log::WARNING, s)
#define ErrMsg(s)    Log::log(Log::System, Log::ERROR,   s)
#define FatalMsg(s)  Log::log(Log::System, Log::FATAL,   s)

////////////////////////////////////////////////////////////////////////////
// Namespace: LibComm
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace LibComm {

//==========================================================================
// Class: Log
//==========================================================================
class Log {

public:

    //----------------------------------------------------------------------
    // Typedef: LogLevel
    //   TRACE   - (=T) Trace information level
    //   DEBUG   - (=D) Debug information level
    //   INFO    - (=I) Normal informational level
    //   WARNING - (=W) Level for exceptional, non problematic situations
    //   ERROR   - (=E) Level for exceptional, problematic but recoverable cases
    //   FATAL   - (=F) Level for exceptional, problematic, non-recoverable cases
    //----------------------------------------------------------------------
    enum LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        FATAL = 5,
        T = TRACE,
        D = DEBUG,
        I = INFO,
        W = WARNING,
        E = ERROR,
        F = FATAL,
    };

    static const std::string System;

    //----------------------------------------------------------------------
    // Static Method: log
    // Put message into log stream
    //
    // Parameters:
    //   caller  - (I) Name of the caller entity
    //   level   - (I) Message criticality level
    //   message - (I) Message to be logged
    //----------------------------------------------------------------------
    static void log(std::string caller, LogLevel level = INFO, std::string message = "");

    //----------------------------------------------------------------------
    // Static Method: getLastLogMsgs
    // Get the content of the log buffer for the caller
    //
    // Parameters:
    //   caller   - (I) Name of the caller entity
    //   lastMsgs - (O) Vector with the last messages logged (from buffer)
    //----------------------------------------------------------------------
    static void getLastLogMsgs(std::string caller, std::vector<std::string> & lastMsgs);

    //----------------------------------------------------------------------
    // Static Method: defineLogSystem
    // Initialises a log system for a given entity
    //
    // Parameters:
    //   caller    - (I) Name of the caller entity
    //   logFile   - (I) Name of the log file
    //   numOfMsgs - (I) Size of the message buffer
    //----------------------------------------------------------------------
    static void defineLogSystem(std::string caller, int numOfMsgs = 1000);

    //----------------------------------------------------------------------
    // Static Method: defineSizeOfLogBuffer
    // Specifies the size of the log buffer
    //
    // Parameters:
    //   caller    - (I) Name of the caller entity
    //   numOfMsgs - (I) Size of the message buffer
    //----------------------------------------------------------------------
    static void defineSizeOfLogBuffer(std::string caller, int numOfMsgs);

    //----------------------------------------------------------------------
    // Function: getTimeTag
    // Provides a date+time tag in the form HH:MM:SS.ssssss
    //
    // Returns:
    //   A date+time tag in the form HH:MM:SS.ssssss
    //----------------------------------------------------------------------
    static std::string getTimeTag();

    //----------------------------------------------------------------------
    // Static Method: setConsoleOutput
    // Sets the consoleOutput internal flag
    //
    // Parameters:
    //   b - (I) The new value of the consoleOutput flag
    //----------------------------------------------------------------------
    static void setConsoleOutput(bool b);

    //----------------------------------------------------------------------
    // Static Method: setQuietExit
    // Change internal quietExit data member
    //----------------------------------------------------------------------
    static void setQuietExit(bool val = true);

    //----------------------------------------------------------------------
    // Static Method: setLogBaseDir
    // Change base directory for log file
    //
    // Parameters:
    //   base - (I) String with the new directory
    //----------------------------------------------------------------------
    static void setLogBaseDir(std::string base);

    //----------------------------------------------------------------------
    // Static Method: getLogFileName
    // Returns the log file name for a given entity name
    //
    // Parameters:
    //   entity - (I) String with the new directory
    //----------------------------------------------------------------------
    static std::string getLogFileName(std::string entity);

    //----------------------------------------------------------------------
    // Static Method: getLogBaseDir
    // Access to base directory for log file
    //
    // Returns:
    //   The base dir value
    //----------------------------------------------------------------------
    static std::string getLogBaseDir();

    //----------------------------------------------------------------------
    // Static Method: setMinLogLevel
    // Sets the value of the minimum log level
    //
    // Parameters:
    //   lvl - (I) Minimum log level to be used
    //----------------------------------------------------------------------
    static void setMinLogLevel(LogLevel lvl);

    //----------------------------------------------------------------------
    // Static Method: getMinLogLevel
    // Returns the value of the minimum log level
    //
    // Returns:
    //   Value of the minimum log level
    //----------------------------------------------------------------------
    static LogLevel getMinLogLevel();

private:
    //----------------------------------------------------------------------
    // Constructor: Log
    // This constructor is made protected to make a singleton
    //----------------------------------------------------------------------
    Log();

    //----------------------------------------------------------------------
    // Copy constructor
    //----------------------------------------------------------------------
    Log(Log const &) {}

    //----------------------------------------------------------------------
    // Assignment operator
    //----------------------------------------------------------------------
    void operator=(Log const &) {}

    //----------------------------------------------------------------------
    // Destructor: ~Log
    // Virtual destructor
    //----------------------------------------------------------------------
    virtual ~Log();

    //----------------------------------------------------------------------
    // Static Method: ensureSystemLogIsInitialized
    // Initialize the log associated to the entire system if not yet ready
    //----------------------------------------------------------------------
    static void ensureSystemLogIsInitialized();

    //----------------------------------------------------------------------
    // Static Method: closeLogStreams
    // Closes streams before exit
    //----------------------------------------------------------------------
    static void closeLogStreams();

private:

    // Variable: logStream
    // Maps the underlying log stream to the entity name
    static std::map<std::string, std::fstream *>            logStream;

    // Variable: logFileName
    // Maps log file associated name to the entity name
    static std::map<std::string, std::string>               logFileName;

    // Variable: sizeOfLogBuffer
    // Maximum size of buffer of log messages mapped to the entity name
    static std::map<std::string, int >                      sizeOfLogBuffer;

    // Variable: logBuffer
    // Buffer of log messages mapped to the entity name
    static std::map<std::string, std::vector<std::string> > logBuffer;

    // Variable: logBaseDir
    // Local base directory for log files
    static std::string logBaseDir;

    // Flag: consoleOutput
    // Flag to force output replication of messages through console
    static bool consoleOutput;

    // Flag: quietExit
    // Flag to force a quiet exit without any message
    static bool quietExit;

    // Variable: dateMsg
    // Shows the date at the beginning of an execution or after a day change
    static std::string dateString;

    // Variable: dateChangeShown
    // Maps the underlying log stream to the entity name
    static std::map<std::string, bool> dateChangeShown;

    // Variable: minimumLogLevel
    // Defines the minimum level to display log messages
    static LogLevel minimumLogLevel;

    // Flag: isInitialised
    // Flag to ensure the system log is properly initialised when needed
    static bool isInitialised;
};

//}

#endif  /* LOG_H */
