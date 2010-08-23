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
* Description:  ?Description
*
*/


#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jdebug_omj.h"
#include "logger.h"
#include "javacommonutils.h"

#ifdef __SYMBIAN32__
#include "unistd.h" //getpid() on Symbian
#endif

#include "com_nokia_mj_impl_utils_Logger.h"

using namespace java::util;


// ---------------------------------------------------------
//      static class members initialization
// ---------------------------------------------------------
const char TEntryExitLog::KMethodIn[]  = "--> ";
const char TEntryExitLog::KMethodOut[] = "<-- ";

const char Logger::KErrorString[]      = "[Err  ]";
const char Logger::KWarningString[]    = "[Warn ]";
const char Logger::KInfoString[]       = "[Info ]";
const char Logger::KDebugString[]      = "[Debug]";

const int  Logger::DES_FILE_CLOSED     = -1;
const int  Logger::DES_FILE_OVERFLOW   = -2;


#ifndef J_LOG_USE_RLOGGER_ENABLED
std::vector<int> Logger::i_file_descriptors(sizeof component_list / sizeof component_list[0], DES_FILE_CLOSED);
#endif //J_LOG_USE_RLOGGER_ENABLED

#ifdef __SYMBIAN32__
#include "javaredirector.h"
void redirect(const char* aText, TLogLevels aLevel)
{
    switch (aLevel)
    {
    case EError:
    case EWarning:
    case EInfoPrd:
    {
        int len = strlen(aText);
        TPtr8 ptr((unsigned char*)aText, len, len);
        Redirector::log(ptr);
        break;
    }
    default:
        break;
    }
}
#endif



// ---------------------------------------------------------
//      JEntryExitLog::JEntryExitLog
// ---------------------------------------------------------
OS_EXPORT TEntryExitLog::TEntryExitLog(TComponents a_component,
                                       const char* a_method_name) :
        i_component(a_component), i_log_level(EEntryLog), i_method_name(a_method_name)
{
    init();
}

// ---------------------------------------------------------
//      JEntryExitLog::JEntryExitLog
// ---------------------------------------------------------
OS_EXPORT TEntryExitLog::TEntryExitLog(TComponents a_component,
                                       int         a_log_level,
                                       const char* a_method_name) :
        i_component(a_component), i_log_level(a_log_level), i_method_name(a_method_name)
{
    init();
}

// ---------------------------------------------------------
//      JEntryExitLog::init
// ---------------------------------------------------------
void TEntryExitLog::init()
{
    if (Logger::LogNeeded() && ((Logger::GetLogLevel() & i_log_level) == i_log_level))
    {
        std::string log(KMethodIn);
        log.append(i_method_name);
        Logger::Log(i_component, EEntryLog, (char*)log.c_str());
    }
}

// ---------------------------------------------------------
//      JEntryExitLog::~JEntryExitLog
// ---------------------------------------------------------
OS_EXPORT TEntryExitLog::~TEntryExitLog()
{
    if (Logger::LogNeeded() && ((Logger::GetLogLevel() & i_log_level) == i_log_level))
    {
        std::string log(KMethodOut);
        log.append(i_method_name);
        Logger::Log(i_component, EEntryLog, (char*)log.c_str());
    }
}

// ---------------------------------------------------------
//      Logger::Log()
// ---------------------------------------------------------
OS_EXPORT void Logger::Log(TComponents a_component, TLogLevels level, const char* format_str, ...)
{
    if (!Logger::LogNeeded() || ((Logger::GetLogLevel() & level) != level))
    {
        return;
    }

    std::string log_buffer;

#ifdef J_LOG_DATE_TIME_ENABLED

    struct tm *tp;

#ifdef __SYMBIAN32__
    tp = new tm(); //localtime() with pthreads leaks memory in Symbian.
    TTime symTime;
    symTime.HomeTime();
    TDateTime dt = symTime.DateTime();
    tp->tm_mday = dt.Day()+1;
    tp->tm_mon = dt.Month();
    tp->tm_year = dt.Year()-1900;
    tp->tm_hour = dt.Hour();
    tp->tm_min = dt.Minute();
    tp->tm_sec = dt.Second();
#else //__SYMBIAN32__
    time_t t;
    t = time(NULL);                // get current time in seconds from Epoc
    tp = localtime(&t);            // fill tm struct w.r.t localtime using localtime
#endif //__SYMBIAN32__


    // ostringstream leaks memory in Symbian when called for the first time
    std::ostringstream time;

    time << std::setw(2) << std::setfill('0') << tp->tm_mday << "/";
    time << std::setw(2) << std::setfill('0') << tp->tm_mon+1 << "/";
    time << std::setw(2) << std::setfill('0') << tp->tm_year + 1900 << " ";

    time << std::setw(2) << std::setfill('0') << tp->tm_hour << ":";
    time << std::setw(2) << std::setfill('0') << tp->tm_min << ":";
    time << std::setw(2) << std::setfill('0') << tp->tm_sec;

    log_buffer.append(time.str());


#ifdef __SYMBIAN32__
    delete tp;
#endif //__SYMBIAN32__


#endif //J_LOG_DATE_TIME_ENABLED


#ifdef J_LOG_MILLISEC_ENABLED

    log_buffer.append(".");
    std::ostringstream mills;

#ifdef __SYMBIAN32__
    mills << std::setw(6) << std::setfill('0') << dt.MicroSecond();  // max 6 digits for microseconds
#else //__SYMBIAN32__

    struct timeval tv;
    gettimeofday(&tv, NULL);

    mills << std::setw(6) << std::setfill('0') << tv.tv_usec;  // max 6 digits for microseconds
#endif //__SYMBIAN32__

    const int ms_precision = 3;
    log_buffer.append(mills.str(), 0, ms_precision);
#endif //J_LOG_MILLISEC_ENABLED


#ifdef J_LOG_PID_ENABLED

    // getpid() leaks memory in Symbian when called for the first time
    int pid = getpid();
    std::ostringstream process_id;
    process_id << " [pid:" <<  std::setw(5) << std::setfill('0') << pid << "] ";
    log_buffer.append(process_id.str());

#endif //J_LOG_PID_ENABLED


#ifdef J_LOG_ALL_LOGS_TO_SINGLE_FILE

    //log_buffer.append(component_names[a_component]);
    log_buffer.append(component_list[a_component].name);
    log_buffer.append(" ");

#endif //J_LOG_ALL_LOGS_TO_SINGLE_FILE


    switch (level)
    {
    case EError:
        log_buffer.append(KErrorString);
        break;

    case EWarning:
        log_buffer.append(KWarningString);
        break;

    case EInfo:
    case EInfoPrd:
        log_buffer.append(KInfoString);
        break;

    default:
        log_buffer.append(KDebugString);

    }

    log_buffer.append(" ");

    const int log_buf_len = 1024; // big enough for one log line
//    char buffer[log_buf_len + 1];
    ScopedCharArray buffer(log_buf_len + 1);

    // va_list or vsnprintf() leaks memory in Symbian when called for the first time
    va_list ap;
    va_start(ap, format_str);

    vsnprintf(buffer.get(), log_buf_len, format_str, ap);
    va_end(ap);

    log_buffer.append(buffer.get());

#ifdef J_LOG_USE_JDEBUG
    ERROR_STR("%s",log_buffer.c_str());
    return;
#endif //J_LOG_USE_JDEBUG

    log_buffer.append("\r\n");

    if (level & (EError | EWarning | EInfoPrd))
    {
#ifdef __SYMBIAN32__
        // Error & warnign level messages are logged always (to 'java.txt')
        TInt bufLen = strlen(log_buffer.c_str());
        TPtr8 ptr8((unsigned char *)log_buffer.c_str(), bufLen, bufLen);
        RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppendRaw, ptr8);
#else
        printf("%s\n", log_buffer.c_str());
#endif //__SYMBIAN32__
    }

    Print(log_buffer.c_str(), a_component);

#ifdef __SYMBIAN32__
    redirect(log_buffer.c_str(), level);
#endif

    return;
}
// ---------------------------------------------------------
//      Logger::LogNeeded()
// ---------------------------------------------------------
bool Logger::LogNeeded()
{
    // Not currently need to be implemented.
    return true;
}

// ---------------------------------------------------------
//       Logger::GetLogLevel()
// ---------------------------------------------------------
int Logger::GetLogLevel()
{

#ifdef  JAVA_HEAVY_LOGGER_ON
    int level = EError | EWarning | EInfoPrd | EInfo | EEntryLog | EInfoHeavyLoad;
#else
    int level = EError | EWarning | EInfoPrd | EInfo;
#endif

    return level;
}


// ---------------------------------------------------------
//       Logger::Print()
// ---------------------------------------------------------
void Logger::Print(const char* txt, int index)
{

#ifdef J_LOG_USE_RLOGGER_ENABLED

    _LIT(KJavaLogDir, "java\\full");


#ifdef J_LOG_ALL_LOGS_TO_SINGLE_FILE
    const char* fileName = component_list[0].log_file; //log_file_names[0];
#else //J_LOG_ALL_LOGS_TO_SINGLE_FILE
    const char* fileName = component_list[index].log_file; //log_file_names[index];
#endif //J_LOG_ALL_LOGS_TO_SINGLE_FILE


    int logFileNameLen = strlen(fileName);
    TPtr8 fileNamePtr((unsigned char*)fileName, logFileNameLen, logFileNameLen);
    RBuf nameBuf;
    int rc = nameBuf.Create(fileNamePtr.MaxLength());
    if (rc == KErrNone)
    {
        nameBuf.Copy(fileNamePtr);

        TInt len = strlen(txt);
        TPtr8 ptr((unsigned char*)txt, len, len);
        RFileLogger::Write(KJavaLogDir, nameBuf, EFileLoggingModeAppendRaw, ptr);
        nameBuf.Close();
    }

#else //J_LOG_USE_RLOGGER_ENABLED

    int fd = GetFileDescriptor(index);
    if (fd != DES_FILE_CLOSED)
    {
        lseek(fd, 0, SEEK_END);
        write(fd, txt, strlen(txt));
    }

#endif //J_LOG_USE_RLOGGER_ENABLED
}


#ifndef J_LOG_USE_RLOGGER_ENABLED

// ---------------------------------------------------------
//    Logger::GetFileDescriptor
// ---------------------------------------------------------
int Logger::GetFileDescriptor(int index)
{
    int fd = i_file_descriptors[index];

    if (fd == DES_FILE_OVERFLOW)
    {
        // log overflow
        return DES_FILE_CLOSED;
    }
    if (fd == DES_FILE_CLOSED)
    {
        char* bin_dir = getenv("JAVA_BIN_ROOT");
        if (bin_dir != 0)
        {

#ifdef J_LOG_ALL_LOGS_TO_SINGLE_FILE
            const char* fileName = component_list[0].log_file; //log_file_names[0];
#else //J_LOG_ALL_LOGS_TO_SINGLE_FILE
            const char* fileName = component_list[index].log_file; //log_file_names[index];
#endif //J_LOG_ALL_LOGS_TO_SINGLE_FILE

            std::string full_path(bin_dir);
            full_path.append(1, '/');

            full_path.append(fileName);

            fd = open(full_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            i_file_descriptors[index] = fd;
            if (fd == -1)
                DEBUG_STR("logger_thread_function() WARNING: can't open log file %s", full_path.c_str());
        }
        else
        {
            DEBUG("logger_thread_function() WARNING: JAVA_BIN_ROOT is not defined, ignore logging");
        }
    }

    // check if log file too big
    struct stat fileStatBuf;
    if (fstat(fd, &fileStatBuf) != -1)
    {
        if (fileStatBuf.st_size > MAX_LOG_FILE_SIZE)
        {
            i_file_descriptors[index] = DES_FILE_OVERFLOW; //no more logging to the log file

            std::string warning("LOG FILE OVERFLOW, PLEASE CLEAN UP \n");
            lseek(fd, 0, SEEK_END);
            write(fd, warning.c_str(), warning.length());
            close(fd);
            return DES_FILE_CLOSED;
        }
    }
    return fd;
}

#endif //J_LOG_USE_RLOGGER_ENABLED


/**
 * Class:     com_nokia_mj_impl_utils_Logger
 * Method:    _logging
 * Signature: (IILjava/lang/String;)V
 *
 * Native static Logger._logging() method write log message to file and
 * accepting three input parameters:
 * component id, severity level id of emiting information and tracing information
 */

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_Logger__1logging
(JNIEnv *aEnv, jclass, jint aComponent, jint aLevel, jstring aLogString)
{
    const char* log = aEnv->GetStringUTFChars(aLogString, 0);

    if (aLevel == com_nokia_mj_impl_utils_Logger_EError)
    {
        ELOG1((TComponents)aComponent, "%s", log);
    }
    else if (aLevel == com_nokia_mj_impl_utils_Logger_EWarning)
    {
        WLOG1((TComponents)aComponent, "%s", log);
    }
    else if (aLevel == com_nokia_mj_impl_utils_Logger_EInfoPrd)
    {
        PLOG1((TComponents)aComponent, "%s", log);
    }
    else
    {
        LOG1((TComponents)aComponent, EInfo, "%s", log);
    }

    aEnv->ReleaseStringUTFChars(aLogString, log);
}


/*
 * Class:     com_nokia_mj_impl_utils_Logger
 * Method:    _loggingException
 * Signature: (IILjava/lang/String;Ljava/lang/Throwable;Ljava/io/ByteArrayOutputStream;Ljava/io/PrintStream;)V
 *
 * Method prints stack trace and Throwable info to log file
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_Logger__1loggingException
(JNIEnv *aEnv, jclass /*aClassH*/, jint aComponent, jint aLevel, jstring aLogString,
 jthrowable aThrowable, jobject aByteStream, jobject aPrintStream)
{
    /* get logging string */
    const char* log = aEnv->GetStringUTFChars(aLogString, 0);

    /*
     * call Throwable.printStackTrace(java.io.PrintStream)
     * this method is not part of CLDC spec, but it's supported by VM vendors
     */
    jclass class_Throwable = aEnv->GetObjectClass(aThrowable);
    jmethodID methodId = aEnv->GetMethodID(class_Throwable, "printStackTrace", "(Ljava/io/PrintStream;)V");
    aEnv->CallVoidMethod(aThrowable, methodId, aPrintStream);

    /* call ByteArrayOutputStream.toString() */
    jclass class_ByteArrayOutputStream = aEnv->GetObjectClass(aByteStream);
    methodId = aEnv->GetMethodID(class_ByteArrayOutputStream, "toString", "()Ljava/lang/String;");
    jstring stacktrace_jstr = (jstring) aEnv->CallObjectMethod(aByteStream, methodId);
    const char *stacktrace = aEnv->GetStringUTFChars(stacktrace_jstr, 0);

    if (aLevel == com_nokia_mj_impl_utils_Logger_EError)
    {
        ELOG2((TComponents)aComponent, "%s: %s", log, stacktrace);
    }
    else if (aLevel == com_nokia_mj_impl_utils_Logger_EWarning)
    {
        WLOG2((TComponents)aComponent, "%s: %s", log, stacktrace);
    }
    else if (aLevel == com_nokia_mj_impl_utils_Logger_EInfoPrd)
    {
        PLOG2((TComponents)aComponent, "%s: %s", log, stacktrace);
    }
    else
    {
        LOG2((TComponents)aComponent, EInfo, "%s: %s", log, stacktrace);
    }

    aEnv->ReleaseStringUTFChars(aLogString, log);
    aEnv->ReleaseStringUTFChars(stacktrace_jstr, stacktrace);
}
