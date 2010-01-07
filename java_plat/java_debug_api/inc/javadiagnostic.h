/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java Diagnostic API
*
*/


#ifndef JAVADIAGNOSTIC_H
#define JAVADIAGNOSTIC_H

#include <e32cmn.h>

namespace java
{
namespace debug
{

/**
* DiagnosticListener will be called when Java Runtime outputs to standard out
* or makes log message.
*
* DiagnosticListener may not perform any long lasting or blocking tasks
* in the callback.
*
* Example below illustrates how Java Runtime outputs could be logged to file.
*
* @code
* class LoggingListener : public DiagnosticListener
* {
* public:
*     virtual void systemOut(const TDesC8& aData) { flog(aData); };
*     virtual void systemErr(const TDesC8& aData) { flog(aData); };
*     virtual void log(const TDesC8& aData) { flog(aData); };
* protected:
*     virtual void flog(const TDesC8& aData) {
*         RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppendRaw, aData);
*     };
* }
*
* LoggingListener listener;
* JavaDiagnostic* jd = JavaDiagnostic::createInstance();
* jd->setDiagnosticListener(listener);
* ...
* jd->removeDiagnosticListener();
* delete jd;
* @endcode
*/
class DiagnosticListener
{
public:
    /**
     * Called when Java Runtime writes to the standard output stream
     * @param[in] aText may contain multiple lines of data
     */
    virtual void systemOut(const TDesC8& aText) = 0;

    /**
     * Called when Java Runtime writes to the standard error stream
     * @param[in] aText may contain multiple lines of data
     */
    virtual void systemErr(const TDesC8& aText) = 0;

    /**
     * Called when Java Runtime makes log writing
     * @param[in] aText may contain multiple lines of data ending with newline
     */
    virtual void log(const TDesC8& aText) = 0;
};


/**
* Java Diagnostic can be used to monitor Java Runtime standard outs and
* log messages.
*
* DiagnosticListener should be set before making any Java Debug API operation,
* like installing or starting Java application, to ensure that all output
* is captured.
*
* There can be only one running Java Diagnostic instance in the system, i.e.
* instance with set listener.
*
* @code
* JavaDiagnostic* jd = JavaDiagnostic::createInstance();
* jd->setDiagnosticListener(listener);
* ...
* jd->removeDiagnosticListener();
* delete jd;
* @endcode
*
* @lib javadebugapi.lib
* @since S60 v9.2
*/
class JavaDiagnostic
{
public:
    virtual ~JavaDiagnostic() {};

    /**
     * Factory method for JavaDiagnostic.
     * Returned pointer must be deleted once done using it.
     * @return new JavaDiagnostic object
     */
    IMPORT_C static JavaDiagnostic* createInstance();

    /**
     * Sets listener and starts monitoring Java Runtime standard outs and log
     * messages. There can be only one running JavaDiagnostic instance in
     * the system.
     * @param[in] aListener will be called when Java Runtime outputs to standard out
     *                      or makes log message
     * @return KErrNone if listener was set and monitoring was started succesfully
     * @return KErrAlreadyExists if JavaDiagnostic is already running or setDiagnosticListener
     *              is called twice without removing listener in between
     */
    virtual TInt setDiagnosticListener(DiagnosticListener& aListener) = 0;

    /**
     * Removes listener and stops monitoring Java Runtime standard outs and
     * log messages.
     */
    virtual void removeDiagnosticListener() = 0;
};

} // end namespace debug
} // end namespace java

#endif // JAVADIAGNOSTIC_H
