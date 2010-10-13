/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Logging component is used to record sequential error, warning and tracing
* information. All components in Java package shall use either this c++
* logger or Java logger to enable easier tracing for Java platform and
* Java application developers.
* The log files use UTF-8 encoding to enable usage of non-ASCII characters
* in the log messages.
*
* The Logger has 6 levels of logging messages:
*
*  * EError, used when there is an error, for example installation fails.
*  * EWarning, used when application installation / running is not interrupted,
*              but installation/application is behaving incorrectly (e.g. not
*              able to read opened file, not able to decode currupted image)
*  * EInfoPrd, used in product high level tracing, e.g. what application has been
*              started, which parameters has been passed to virtual machine,
*              what is the URL accessed with HTTP stack. These messages
*              must be understable without checking the Java platform
*              implementation.
*  * EInfo, used in high level tracing
*  * EEntryLog, used to log function entries
*  * EHeavyLoad, used in detailed tracing
*
*  EError, EWarning and EInfoPrd are enabled both in debug and release (product) builds
*  EError, EWarning, EInfoPrd and EInfo are enabled in debug builds.
*  EEntryLog and EHeavyLoad shall be enabled from this file
*
* Note that the logs are created in Symbian to two directories
* C:\logs\java  (the most important log files,
*                error level log entries can be found here)
* C:\logs\java\full (create this directory and define
*                    JAVA_LOGGER_ON when you want to enable full logging)
*
*/


#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <string>
#include <vector>
#include "javaosheaders.h"
#include "javacommonutils.h"

#ifdef _DEBUG
#define  JAVA_LOGGER_ON
#endif // _DEBUG


//#define  JAVA_LOGGER_ON
//#define  JAVA_HEAVY_LOGGER_ON
#define  J_LOG_DATE_TIME_ENABLED            //add date and time to log
#define  J_LOG_MILLISEC_ENABLED             //add time in millisec to log
#define  J_LOG_PID_ENABLED                  //add time pid to log

//#define J_LOG_ALL_LOGS_TO_SINGLE_FILE       //log to a single file


/**
 * List of component IDs.
 * Logger uses these IDs to identify source of tracing information.
 * Developers shall add new component IDs to the end of this list.
 * Keep the list in sync with the java side logging component
 * javacommons\utils\javasrc\com\nokia\mj\impl\utils\Logger.java
 */
enum TComponents
{
    EJavaCaptain            = 1,
    EJavaRuntime,       //  = 2
    EJavaComms,         //  = 3
    EAppRecognizer,     //  = 4
    EMidpRms,           //  = 5
    EUtils,             //  = 6
    EJavaUI,            //  = 7
    EJavaPush,          //  = 8
    EJVM,               //  = 9
    EGcf,               //  =10
    EJavaStorage,       //  =11
    EJavaInstaller,     //  =12
    EJavaFile,          //  =13
    ESOCKET,            //  =14
    EWMA,               //  =15
    EPim,               //  =16
    EJavaPreinstaller,  //  =17
    ETckRunner,         //  =18
    EJavaBluetooth,     //  =19
    EJavaSecurity,      //  =20
    EGcfProtocols,      //  =21
    EJavaMMAPI,         //  =22
    ESATSA,             //  =23
    EJavaLocation,      //  =24
    EBackup,            //  =25
    EJavaSystemAMS,     //  =26
    EJavaIapInfo,       //  =27
    EJavaContactless,   //  =28
    EJavaConverters,    //  =29
    EMidp2CenrepWrapper,//  =30
    EJavaWebServices,   //  =31
    EJavaGlobalInd,     //  =32
    ESensor,            //  =33
    EDebugApi,          //  =34
    EJavaAppMngrPlugin, //  =35
    EJavaBroadcast,     //  =36
    EJavaAMMS,					//  =37
    // add id of new components here
};

/**
 * Logger could store tracing information from different components
 * to a single file or create separate logging file for each component
 * (to log to single file enable J_LOG_ALL_LOGS_TO_SINGLE_FILE macro).
 *
 * This structure contains name of logging file for each component and
 * component's nickname which will be used when all logging info is stored
 * in a single file.
 */
const struct
{
    const char* log_file;
    const char* name;
} component_list[] =
{
    {"JavaLog.log",          NULL            },       // When all logs are directed to single log file

    {"JavaCaptain.log",      "[JavaCaptain]" },       // EJavaCaptain
    {"JavaRuntime.log",      "[JavaRuntime]" },       // EJavaRuntime
    {"JavaComms.log",        "[ JavaComms ]" },       // EJavaComms
    {"AppRecognizer.log",    "[AppRecognz ]" },       // EAppRecognizer
    {"JavaRms.log",          "[  JavaRms  ]" },       // EMidpRms
    {"JavaUtils.log",        "[ JavaUtils ]" },       // EUtils
    {"JavaUi.log",           "[  JavaUi   ]" },       // EJavaUi
    {"JavaPush.log",         "[ JavaPush  ]" },       // EJavaPush
    {"JavaVM.log",           "[  JavaVM   ]" },       // EJVM
    {"JavaGcf.log",          "[  JavaGcf  ]" },       // EGcf
    {"JavaStorage.log",      "[JavaStorage]" },       // EJavaStorage
    {"JavaInstaller.log",    "[JavaInstall]" },       // EJavaInstaller
    {"JavaFile.log",         "[ JavaFile  ]" },       // EJavaFile
    {"JavaSocket.log",       "[ SOCKET  ]" },         // ESOCKET
    {"JavaWma.log",          "[ WMA       ]" },       // EWMA
    {"JavaPim.log",          "[ PIM       ]" },       // EPIM
    {"JavaPreinstaller.log", "[JavaPreinstaller]" },  // EJavaPreinstaller
    {"JavaTckRunner.log",    "[ TckRunner ]" },       // ETckRunner
    {"JavaBluetooth.log",    "[JavaBluetooth]" },     // EJavaBluetooth
    {"JavaSecurity.log",     "[ JavaSecurity ]" },    // EJavaSecurity
    {"JavaGcfProtocols.log", "[JavaGcfProtocols ]" }, // EGcfProtocols
    {"JavaMMAPI.log",        "[ MMAPI ]"},            // EJavaMMAPI
    {"JavaSatsa.log",        "[JavaSatsa]" },         // ESATSA
    {"JavaLocation.log",     "[ JavaLocation }"},     // ELocation
    {"JavaBackup.log",       "[ JavaBackup ]"},       // EJavaBackup
    {"JavaSystemAMS.log",    "[JavaSystemAMS]"},      // EJavaSystemAMS
    {"JavaIapInfo.log",      "[ JavaIapInfo ]"},      // EJavaIapInfo
    {"JavaContactless.log",  "[JavaContactless]"},    // EJavaContactless
    {"JavaConverters.log",   "[JavaConverters]"},     // EJavaConverters
    {"JavaCenrepWrapper.log","[JavaCenrepWrapper]"},  // EMidp2CenrepWrapper
    {"JavaWebServices.log",  "[JavaWebServices]"},    // EJavaWebServices
    {"JavaGlobalInd.log",    "[JavaGlobalInd]"},      // EJavaGlobalInd
    {"JavaSensor.log",       "[JavaSensor   ]"},      // EJavaSensor
    {"JavaDebugApi.log",     "[JavaDebugApi]"},       // EDebugApi
    {"JavaAppMngrPlugin.log","[JavaAppMngrPlugin]"},  // EJavaAppMngrPlugin
    {"JavaBroadcast.log",    "[JavaBroadcast]"},      // EJavaBroadcast
    {"JavaMMAPI.log",         "[ MMAPPI ]"},					// EJavaAMMS
    // add new component file name and nickname here
};

/**
 * logging levels
 */
enum TLogLevels
{

    /**
     * logging disabled
     */
    ENoLog = 0,

    /**
     * Use ELOG(s) macros to log errors.
     * Activated both in debug and release builds.
     */
    EError = 0x1,

    /**
     * Use WLOG(s) macros to log warnigs.
     * Activated both in debug and release builds.
     */
    EWarning = 0x2,

    /**
     * Used for high level tracing. The logging level is enabled
     * both in release (products) and debug builds (emulators).
     * Use PLOG(s) macros.
     * @see logging level EInfo
     */
    EInfoPrd = 0x4,

    /**
     * Used for debug tracing information. Could be activated
     * in debug builds, disabled for release builds completely.
     * Use ILOG(s) macros.
     */
    EInfo = 0x8,

    /**
    * Used for function entries. Could be activated
    * in debug builds with JAVA_HEAVY_LOGGER_ON flag.
    * Use JELOG(s) macros.
    */
    EEntryLog = 0x10,

    /**
     * Used for heavy tracing, could generate huge log files.
     * Activated by JAVA_HEAVY_LOGGER_ON compilation flag.
     * Use HLOG(s) macros.
     */
    EInfoHeavyLoad = 0x20,
};


// ---------------------------------------------------------
//    Logging macros
// ---------------------------------------------------------

#ifdef __SYMBIAN32__
#define J_LOG_USE_RLOGGER_ENABLED //use Symbian logger for output
#endif

/**
 * Macros for Error level logging are always enabled.
 *
 * params:
 *   component - component id from TComponents enum
 *   str, a, b, c, d - string and up to 4 parameters in printf() family functions style
 */
#define ELOG(component, str)                    java::util::Logger::Log(component, EError, str)
#define ELOG1(component, str, a)                java::util::Logger::Log(component, EError, str, a)
#define ELOG2(component, str, a, b)             java::util::Logger::Log(component, EError, str, a, b)
#define ELOG3(component, str, a, b, c)          java::util::Logger::Log(component, EError, str, a, b, c)
#define ELOG4(component, str, a, b, c, d)       java::util::Logger::Log(component, EError, str, a, b, c, d)

#define ELOG1WSTR(component, str, wstr)         { char* msg =  java::util::JavaCommonUtils::wstringToUtf8(wstr); \
                                                  java::util::Logger::Log(component, EError, str, msg); \
                                                  delete[] msg; }

/**
 * Macros for Warning level logging are always enabled.
 *
 * params:
 *   component - component id from TComponents enum
 *   str, a, b, c, d - string and up to 4 parameters in printf() family functions style
 */
#define WLOG(component, str)                    java::util::Logger::Log(component, EWarning, str)
#define WLOG1(component, str, a)                java::util::Logger::Log(component, EWarning, str, a)
#define WLOG2(component, str, a, b)             java::util::Logger::Log(component, EWarning, str, a, b)
#define WLOG3(component, str, a, b, c)          java::util::Logger::Log(component, EWarning, str, a, b, c)
#define WLOG4(component, str, a, b, c, d)       java::util::Logger::Log(component, EWarning, str, a, b, c, d)

#define WLOG1WSTR(component, str, wstr)         { char* msg =  java::util::JavaCommonUtils::wstringToUtf8(wstr); \
                                                  java::util::Logger::Log(component, EWarning, str, msg); \
                                                  delete[] msg; }

/**
 * Macros for Info level logging ending to the products.
 * These are always enabled.
 *
 * params:
 *   component - component id from TComponents enum
 *   str, a, b, c, d - string and up to 4 parameters in printf() family functions style
 */
#define PLOG(component, str)                    java::util::Logger::Log(component, EInfoPrd, str)
#define PLOG1(component, str, a)                java::util::Logger::Log(component, EInfoPrd, str, a)
#define PLOG2(component, str, a, b)             java::util::Logger::Log(component, EInfoPrd, str, a, b)
#define PLOG3(component, str, a, b, c)          java::util::Logger::Log(component, EInfoPrd, str, a, b, c)
#define PLOG4(component, str, a, b, c, d)       java::util::Logger::Log(component, EInfoPrd, str, a, b, c, d)

#define PLOG1WSTR(component, str, wstr)         { char* msg =  java::util::JavaCommonUtils::wstringToUtf8(wstr); \
                                                  java::util::Logger::Log(component, EInfoPrd, str, msg); \
                                                  delete[] msg; }

#ifdef JAVA_LOGGER_ON
/**
 * Tracing macros for EInfo level logging. There are always enabled in debug builds.
 *
 * params:
 *   component - component id from TComponents enum
 *   str, a, b, c, d - string and up to 4 parameters in printf() family functions style
 */
#define ILOG(component, str)                    java::util::Logger::Log(component, EInfo, str)
#define ILOG1(component, str, a)                java::util::Logger::Log(component, EInfo, str, a)
#define ILOG2(component, str, a, b)             java::util::Logger::Log(component, EInfo, str, a, b)
#define ILOG3(component, str, a, b, c)          java::util::Logger::Log(component, EInfo, str, a, b, c)
#define ILOG4(component, str, a, b, c, d)       java::util::Logger::Log(component, EInfo, str, a, b, c, d)
#define ILOG1WSTR(component, level, str, wstr)        { char* msg =  java::util::JavaCommonUtils::wstringToUtf8(wstr); \
                                                      java::util::Logger::Log(component, EInfo, str, msg); \
                                                      delete[] msg; }

#else /* JAVA_LOGGER_ON */

#define ILOG(component, str)
#define ILOG1(component, str, a)
#define ILOG2(component, str, a, b)
#define ILOG3(component, str, a, b, c)
#define ILOG4(component, str, a, b, c, d)

#define ILOG1WSTR(component, level, str, wstr)

#endif /* JAVA_LOGGER_ON */


#ifdef JAVA_HEAVY_LOGGER_ON
/**
 * Tracing macros to label entering and exiting points of C++ function.
 *
 * These macros could be placed in the begining of the function body and they will
 * automatically generate tracing information when function exit.
 * activated by JAVA_HEAVY_LOGGER_ON compilation flag.
 *
 * params:
 *   component - component id from TComponents enum
 *   level - tracing level from TLogLevels enum, EEntry by default
 *   str - tracing info, type char*, name and signature of function by default
 *
 */
#define JELOG(component, str)                   java::util::TEntryExitLog _x_(component, str)
#define JELOG2(component)                       java::util::TEntryExitLog _x_(component, __PRETTY_FUNCTION__)
#define JELOG3(component, level, str)           java::util::TEntryExitLog _x_(component, level, str)
#define JELOG4(component, level)                java::util::TEntryExitLog _x_(component, level, __PRETTY_FUNCTION__)

#else /* JAVA_HEAVY_LOGGER_ON */

#define JELOG(component, str)
#define JELOG2(component)
#define JELOG3(component, level, str)
#define JELOG4(component, level)

#endif /* JAVA_HEAVY_LOGGER_ON */


#ifdef JAVA_HEAVY_LOGGER_ON
/**
 * Tracing macros to heavy logging.
 *
 * Log can be activated by JAVA_HEAVY_LOGGER_ON compilation flag.
 *
 * params:
 *   component - component id from TComponents enum
 *   str - tracing info, type char*, name and signature of function by default
 *
 */
#define HLOG(component, str)                    java::util::Logger::Log(component, EInfoHeavyLoad, str)
#define HLOG1(component, str, a)                java::util::Logger::Log(component, EInfoHeavyLoad, str, a)
#define HLOG2(component, str, a, b)             java::util::Logger::Log(component, EInfoHeavyLoad, str, a, b)
#define HLOG3(component, str, a, b, c)          java::util::Logger::Log(component, EInfoHeavyLoad, str, a, b, c)
#define HLOG4(component, str, a, b, c, d)       java::util::Logger::Log(component, EInfoHeavyLoad, str, a, b, c, d)

#else /* JAVA_HEAVY_LOGGER_ON */

#define HLOG(component, level, str)
#define HLOG1(component, level, str, a)
#define HLOG2(component, level, str, a, b)
#define HLOG3(component, level, str, a, b, c)
#define HLOG4(component, level, str, a, b, c, d)

#endif /* JAVA_HEAVY_LOGGER_ON */

#ifdef JAVA_LOGGER_ON
/**
 * Old tracing macros for EInfo and EInfoHeavyLoad level logging.
 * Please use ILOG and HLOG macros in new code.
 *
 * params:
 *   component - component id from TComponents enum
 *   level - tracing level from TLogLevels enum (EInfo or EInfoHeavyLoad)
 *   str, a, b, c, d - string and up to 4 parameters in printf() family functions style
 */
#define LOG(component, level, str)                    java::util::Logger::Log(component, level, str)
#define LOG1(component, level, str, a)                java::util::Logger::Log(component, level, str, a)
#define LOG2(component, level, str, a, b)             java::util::Logger::Log(component, level, str, a, b)
#define LOG3(component, level, str, a, b, c)          java::util::Logger::Log(component, level, str, a, b, c)
#define LOG4(component, level, str, a, b, c, d)       java::util::Logger::Log(component, level, str, a, b, c, d)

#define LOG1WSTR(component, level, str, wstr)        { char* msg =  java::util::JavaCommonUtils::wstringToUtf8(wstr); \
                                                      java::util::Logger::Log(component, level, str, msg); \
                                                      delete[] msg; }

#else /* JAVA_LOGGER_ON */

#define LOG(component, level, str)
#define LOG1(component, level, str, a)
#define LOG2(component, level, str, a, b)
#define LOG3(component, level, str, a, b, c)
#define LOG4(component, level, str, a, b, c, d)

#define LOG1WSTR(component, level, str, wstr)

#endif /* JAVA_LOGGER_ON */


#ifndef J_LOG_USE_RLOGGER_ENABLED
#if defined(__SYMBIAN32__) && !defined(__WINSCW__)
const int  MAX_LOG_FILE_SIZE = 1024*1024*2;    // 2Mb max size of log file
#else
const int  MAX_LOG_FILE_SIZE = 1024*1024*16;    // 16Mb max size of log file
#endif
#endif


namespace java
{
namespace util
{

// ---------------------------------------------------------
//    TEntryExitLog class
//    Logs entry and exit message whenever instantiated in a method
// ---------------------------------------------------------
class TEntryExitLog
{

public:

    /**
     * Logs entry message in a function
     *
     * @param aComponent the code of a user component
     * @param aMethod the name of a user method
     */
    OS_IMPORT TEntryExitLog(TComponents component,
                            const char* method_name);

    /**
     * Logs entry message in a function
     *
     * @param aComponent the code of a user component
     * @param aMethod the name of a user method
     */
    OS_IMPORT TEntryExitLog(TComponents component,
                            int         log_level,
                            const char* method_name);

    /**
     * Logs exit message when a functions exited
     */
    OS_IMPORT ~TEntryExitLog();

private:
    void init();

private:
    static const char KMethodIn[];
    static const char KMethodOut[];

    TComponents          i_component;
    int                  i_log_level;
    const char*          i_method_name;

};


// ---------------------------------------------------------
//    Logger class
//    Write log messages to corresponding files
// ---------------------------------------------------------
class Logger
{

public:
    /**
     * Check if logging is enabled
     *
     * @return true if log folder exists, otherwise false.
     * Checking up the folder is made only at the first call
     */
    static bool LogNeeded();

    /**
     * Log level can be read with this method.
     *
     * @return the current log level
     */
    static int GetLogLevel();

    /**
     * Formats and writes log message to file
     * @param component - id of a logging component
     * @param level - logging level
     * @param format_str - C string that contains the text to be written to the log.
     *                     It can optionally contain embedded format tags (in printf() family functions style ) that are
     *                     replaced by the values specified in subsequent additional arguments and formatted as requested
     */
    OS_IMPORT static void Log(TComponents component, TLogLevels level, const char* format_str, ...);

private:

    /**
     * prints log info to file
     * @param txt -  C string to be printed to log
     * @param index - index of log file
     */
    static void Print(const char* txt, int index);

#ifndef J_LOG_USE_RLOGGER_ENABLED

    /**
     * Returns descriptor of opened file by index, opens it if it wasn't opened before
     * @param index - index of file in i_file_descriptors table
     * @return descriptor of opened file
     */
    static int GetFileDescriptor(int index);

    /**
     * table of file descriptors
     */
    static std::vector<int> i_file_descriptors;

#endif //J_LOG_USE_RLOGGER_ENABLED

    static const char KErrorString[];
    static const char KWarningString[];
    static const char KInfoString[];
    static const char KDebugString[];

    static const int  DES_FILE_CLOSED;
    static const int  DES_FILE_OVERFLOW;

};

} //end namespace util
} //end namespace java

#endif // LOGGER_H
