/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef SWTLOG_H_
#define SWTLOG_H_

#include <QString>
#include <QTime>

// Un-comment the following line to enable JNI logging
//#define SWT_ENABLE_LOGGING

namespace Java { namespace eSWT {

/*
 * Categories/types of logs. 
 */
enum SwtLogType {
    SwtLogUnknown,
    SwtLogJNIEnter,
    SwtLogJNIExit,
    SwtLogFuncEnter,
    SwtLogFuncExit,
    SwtLogScopeEnter,
    SwtLogScopeExit,
    SwtLogEvent,
    SwtLogFatal,
    SwtLogDebugData,
    GfxLogJNIEnter,
    GfxLogJNIExit,
    GfxLogFuncEnter,
    GfxLogFuncExit,
    GfxLogScopeEnter,
    GfxLogScopeExit
};

// A Time stamp prefixed to all log entries
#define SWT_LOG_TIMESTAMP QTime::currentTime().toString("HH:mm:ss.zzz").toLatin1().data()

// The component name used as the prefix for all log entries
static const char* const swtLogComponentName = "Java SWT";
static const char* const gfxLogComponentName = "Java gfx";
static const char* const unkLogComponentName = "Java N/A";

// Descriptions of different types of logged data used for data log entries
static const char* const swtLogFatalDescription = "Fatal";
static const char* const swtLogEventDescription = "Event";
static const char* const swtLogDebugDataDescription = "Data";
static const char* const swtLogScopeEnteredDescription = "Entered";
static const char* const swtLogScopeExitingDescription = "Exiting";
static const char* const swtLogUnknownDataDescription = "Unknown";

//
// Macros for SWT component logging
//

#ifdef SWT_ENABLE_LOGGING

// Logging of a JNI native method call, JNI functions have this
#define SWT_LOG_JNI_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, SwtLogJNIEnter, SwtLogJNIExit)

// Logging of a function call other than the above
#define SWT_LOG_FUNC_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, SwtLogFuncEnter, SwtLogFuncExit)

// Logging of a scope other than function call
#define SWT_LOG_SCOPE_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, SwtLogScopeEnter, SwtLogScopeExit)

// Logging of events
#define SWT_LOG_EVENT_1(format, field1) SwtDataLog::LogData(format, field1, SwtLogEvent)
#define SWT_LOG_EVENT_2(format, field1, field2) SwtDataLog::LogData(format, field1, field2, SwtLogEvent)
#define SWT_LOG_EVENT_3(format, field1, field2, field3) SwtDataLog::LogData(format, field1, field2, field3, SwtLogEvent)
#define SWT_LOG_EVENT_4(format, field1, field2, field3, field4) SwtDataLog::LogData(format, field1, field2, field3, field4, SwtLogEvent)
#define SWT_LOG_EVENT_5(format, field1, field2, field3, field4, field5) SwtDataLog::LogData(format, field1, field2, field3, field4, field5, SwtLogEvent)

// Logging of debug data other than the above
#define SWT_LOG_DATA_1(format, data1) SwtDataLog::LogData(format, data1, SwtLogDebugData)
#define SWT_LOG_DATA_2(format, data1, data2) SwtDataLog::LogData(format, data1, data2, SwtLogDebugData)
#define SWT_LOG_DATA_3(format, data1, data2, data3) SwtDataLog::LogData(format, data1, data2, data3, SwtLogDebugData)
#define SWT_LOG_DATA_4(format, data1, data2, data3, data4) SwtDataLog::LogData(format, data1, data2, data3, data4, SwtLogDebugData)
#define SWT_LOG_DATA_5(format, data1, data2, data3, data4, data5) SwtDataLog::LogData(format, data1, data2, data3, data4, data5, SwtLogDebugData)

#else // SWT_ENABLE_LOGGING

#define SWT_LOG_JNI_CALL()
#define SWT_LOG_FUNC_CALL()
#define SWT_LOG_SCOPE_CALL()

#define SWT_LOG_EVENT_1(format, field1)
#define SWT_LOG_EVENT_2(format, field1, field2)
#define SWT_LOG_EVENT_3(format, field1, field2, field3)
#define SWT_LOG_EVENT_4(format, field1, field2, field3, field4)
#define SWT_LOG_EVENT_5(format, field1, field2, field3, field4, field5)

//#define SWT_LOG_FATAL_1(format, field1)

#define SWT_LOG_DATA_1(format, data1)
#define SWT_LOG_DATA_2(format, data1, data2)
#define SWT_LOG_DATA_3(format, data1, data2, data3)
#define SWT_LOG_DATA_4(format, data1, data2, data3, data4)
#define SWT_LOG_DATA_5(format, data1, data2, data3, data4, data5)

#endif // SWT_ENABLE_LOGGING

// Log fatal error conditions, application will automatically terminate with return value 1 when using this
#define SWT_LOG_FATAL_1(format, field1) SwtDataLog::LogData(format, field1, SwtLogFatal)

//
// Macros for Graphics component logging
//

// Logging of a JNI native method call, JNI functions have this
//#define GFX_LOG_JNI_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, GfxLogJNIEnter, GfxLogJNIExit)


/**
 * A class implementing logging of entering and leaving a scope. 
 * Additional type parameter is added for doing run time variation 
 * for different types of scope logs.  
 */
class SwtScopeLog
{
public:
	SwtScopeLog( const char* aFunctionName, const SwtLogType& aEnterType, const SwtLogType& aExitType );
	virtual ~SwtScopeLog();
protected:
	SwtScopeLog();
private:
	QString* mFunctionName;
	SwtLogType mExitLogType;
};

/**
 * Functions for logging data using the standard printf conventions. 
 * Each function takes the format string and the parameters. Additional type parameter
 * is added for doing run time variation for different types of debug data logs. 
 */
namespace SwtDataLog
    {    
    static inline void DoLog( const char* aLogEntry, const SwtLogType& aType )
        {
        const char* dataDescription = NULL;
        const char* logComponentName = NULL;
        switch( aType )
            {
            case SwtLogFatal:
                logComponentName = swtLogComponentName;
                dataDescription = swtLogFatalDescription;
                break;
            case SwtLogEvent:
                logComponentName = swtLogComponentName;
                dataDescription = swtLogEventDescription;
                break;
            case SwtLogDebugData:
                logComponentName = swtLogComponentName;
                dataDescription = swtLogDebugDataDescription;
                break;
            case SwtLogJNIEnter:
            case SwtLogFuncEnter:
            case SwtLogScopeEnter:
                logComponentName = swtLogComponentName;
                dataDescription = swtLogScopeEnteredDescription;
                break;
            case SwtLogJNIExit:
            case SwtLogFuncExit:
            case SwtLogScopeExit:
                logComponentName = swtLogComponentName;
                dataDescription = swtLogScopeExitingDescription;
                break;                
            case GfxLogJNIEnter:
            case GfxLogFuncEnter:
            case GfxLogScopeEnter:
                logComponentName = gfxLogComponentName;
                dataDescription = swtLogScopeEnteredDescription;
                break;
            case GfxLogJNIExit:
            case GfxLogFuncExit:
            case GfxLogScopeExit:  
                logComponentName = gfxLogComponentName;
                dataDescription = swtLogScopeExitingDescription;
                break;
            default:
                break;
            }
        
        if( !dataDescription )
            {
            dataDescription = swtLogUnknownDataDescription;
            }
        if( !logComponentName )
            {
            logComponentName = unkLogComponentName;
            }
        if( aType == SwtLogFatal )
            {
            qFatal( "%s %s %s: %s", SWT_LOG_TIMESTAMP, logComponentName, dataDescription, aLogEntry );
            }
        else
            {
		    qDebug( "%s %s %s: %s", SWT_LOG_TIMESTAMP, logComponentName, dataDescription, aLogEntry );
		    }
        }
        
	template<class T1> 
	void LogData( const char* format, const T1& data1, const SwtLogType& type )
		{
		QString str;
		DoLog( str.sprintf( format, data1 ).toLatin1().data(), type );
		}
		
	template<class T1, class T2>
	void LogData( const char* format, const T1& data1, const T2& data2, const SwtLogType& type )
		{
		QString str;
		DoLog( str.sprintf( format, data1, data2 ).toLatin1().data(), type );
		}
		
	template<class T1, class T2, class T3>
	void LogData( const char* format, const T1& data1, const T2& data2, const T3& data3, const SwtLogType& type )
		{
		QString str;
		DoLog( str.sprintf( format, data1, data2, data3 ).toLatin1().data(), type );
		}
		
	template<class T1, class T2, class T3, class T4>
	void LogData( const char* format, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const SwtLogType& type )
		{
		QString str;
		DoLog( str.sprintf( format, data1, data2, data3, data4 ).toLatin1().data(), type );
		}
		
	template<class T1, class T2, class T3, class T4, class T5>
	void LogData( const char* format, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const SwtLogType& type )
		{
		QString str;
		DoLog( str.sprintf( format, data1, data2, data3, data4, data5 ).toLatin1().data(), type );
		}
    }

}}

#endif // SWTLOG_H_

