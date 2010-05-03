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


#ifndef NATIVECOMMCONNECTION_H
#define NATIVECOMMCONNECTION_H

#include <stdlib.h>
#include <string>
#include <map>
#include <stdio.h>
#include <c32comm.h>

#include "nativestreambase.h"
#include "functionserver.h"


typedef std::pair<std::string, std::string> TPair;

namespace java
{

namespace comm
{

class NativeCommConnection: public NativeStreamBase
{

public:

    /**
     * Constructor
     */
    NativeCommConnection(java::util::FunctionServer *aFuncServ);

    /**
     * Destructor
     */
    ~NativeCommConnection();

    /**
     * openCommPortConnection is called by CJavaCommConnection.
     * Refer to the CJavaCommConnection::openCommPortConnection() for more details.
     */
    int openCommPortConnection(const std::string aJavaPortName,
                               int aPortNumber, int *aNumericOptions);

    /**
     * writeBytes is called by StreamConnectionBase when the output stream
     * buffer in java overflows
     * It is also called if flush() is called at the Java side.
     * @param[in]  aJavaBuffer: A byte array of length aLength
     * @return the number of bytes written. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    int writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer, int aOffset,
                   int aLength);

    /**
     * ReadBytes is called by StreamConnectionBase when the buffer at
     * input stream buffer runs out of data.
     * @param[in]  aJavaBuffer: Byte array to which the data read is copied into
     * @return the number of bytes read. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    int readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer);

    /**
     * StopReading is called by StreamConnectionBase once InputStream is closed
     * from the Application.
     */
    void stopReading();

    /**
     * StopWriting is called by StreamConnectionBase OutputStream is closed
     * from the Application.
     */

    void stopWriting();

    /**
     * getBaudRate is called by CJavaCommConnection.
     * Refer to the CJavaCommConnection::getBaudRate() for more details.
     */
    int getBaudRate();

    /**
     * setBaudRate is called by CJavaCommConnection.
     * Refer to the CJavaCommConnection::setBaudRate() for more details.
     */

    int setBaudRate(const int);

    /**
     * close is called by CJavaCommConnection class to close the connection to
     * the serial port.
     */
    void close();

    int doWrite(int aNativeCommConnectionHandle, int aBufferHandle);

    int doRead(JNIEnv& aJni, jbyteArray aJavaBuffer);

    void doOpenConnnection(int portNameHandle);

    int openPortConnection(std::string aJavaPortName, int aPortNumber,
                           int *aNumericOptions);

    int doSetBaudRate(const int aBaudRate);

    int doGetBaudRate();

    void doClose();

    int available();

    int doAvailable();

    void doStopWriting();

    void doStopReading();

private:

    /**
     * TBpsToTInt is internal function called by getBaudRate() function.
     * In Symbian, the speed(baudrate) of any logical port is specfied using
     * TBps datatype. The java api should return the integer value to the
     * application. This function is used to convert TBps to integer.
     * @param  aBaudRate: The platform specific baud rate unit to be converted.
     * @return the speed of the port as integer value.
     */

    int TBpsToTInt(TBps aRate);

    /**
     * TIntToTBps is internal function called by setBaudRate() function.
     * In Symbian, the speed(baudrate) of any logical port is specfied using
     * TBps datatype. The java setBaudRate() api provides the speed as an
     * integer. This function is used to convert integer to TBps.
     * @param  aRate: The int value of speed to be converted.
     * @return the speed of the port as TBps unit.
     */
    TBps TIntToTBps(const TInt aRate);

    /**
     * initializePortSettings is an internal function called by
     * openCommPortConnection() function.
     * This function initializes the port with all the options specified by the
     * Connector.open URI.
     * @param  aNumericOptions: A array of options to be set for this serial
     *                          port which is opened.
     *         aNumericOptions[0]: value of the option "baudrate"
     *         aNumericOptions[1]: value of the option "bitsperchar"
     *         aNumericOptions[2]: value of the option "stopbits"
     *         aStringOptions[3]:  value of the option "parity"
     *         aStringOptions[4]:  value of the option "blocking"
     *         aStringOptions[5]:  value of the option "autocts"
     *         aStringOptions[6]:  value of the option "autorts"
     */
    void initializePortSettings(int *aNumericOptions);

    /**
     * connectToCommServer is an internal function called by
     * openCommPortConnection() to get the RCommServer object.
     * @return 0 on success or error value.
     */
    int connectToCommServer();

    /**
     * mapToSymbianPortName is an internal function called by
     * openCommPortConnection() to get the symbian platform
     * specific port.
     * @return mapped port string
     *
     */
    std::basic_string<char>
    mapToSymbianPortName(const std::string aJavaPortName);

private:
    // hash map of valid port names
    std::map<std::string, std::string> mPortNames;
    // port name
    std::basic_string<char> mNativePortName;
    RCommServ mCommServer;
    bool mConnectionIsOpen;
    RComm mComm;
    TCommConfigV01 mCommConfig;
    TInt mPortLowIndex;
    HBufC8* mInternalBuf;
    int mInternalBuflength;
    int mReadInProgress;
    int mWriteInProgress;
public:
    java::util::FunctionServer *iFuncServ;

};

} // end comm namespace

} // java namespace

#endif // NATIVECOMMCONNECTION_H
