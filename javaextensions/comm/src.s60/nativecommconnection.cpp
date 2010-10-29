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



#include <stdlib.h>
#include <stdio.h>
#include <usbman.h>
#include <acminterface.h>
#include "logger.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "com_nokia_mj_impl_comm_CommConnectionImpl.h"
#include "nativecommconnection.h"
#include "fs_methodcall.h"


#if defined (__WINS__)
_LIT(PDD_NAME,"ECDRV");
_LIT(LDD_NAME,"ECOMM");
#else
_LIT(PDD_NAME, "EUART1");
_LIT(LDD_NAME, "ECOMM");
#endif

_LIT(KSymbianIRComm1, "IRComm");
_LIT(KSymbianIRComm2, "IRCOMM");
_LIT(KSymbianComm,"COMM");
_LIT(KSymbianAcm,"ACM");
_LIT(KSymbianBTComm1,"BTComm");
_LIT(KSymbianBTComm2,"BTCOMM");
_LIT(KSymbianECUART,"ECUART");

enum TCOMMOPTIONSINDEX
{
    ECommBaudRateIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_BAUDRATE_INDEX,

    ECommBitsPerCharIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_BITSPERCHAR_INDEX,

    ECommStopBitsIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_STOPBITS_INDEX,

    ECommParityIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_PARITY_INDEX,

    ECommBlockingIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_BLOCKING_INDEX,

    ECommAutoRtsIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_AUTORTS_INDEX,

    ECommAutoCtsIndex = com_nokia_mj_impl_comm_CommConnectionImpl_COMM_AUTOCTS_INDEX
};

using namespace java::util;
using namespace java::comm;

NativeCommConnection::NativeCommConnection(FunctionServer *aFuncServ) :  mConnectionIsOpen(false),mInternalBuf(NULL),mInternalBuflength(0)
{
    JELOG2(ESOCKET);
    iFuncServ = aFuncServ;
    mPortNames.insert(TPair("IR", "IRComm"));
    mPortNames.insert(TPair("USB", "ACM"));
    mPortNames.insert(TPair("COM", "COMM"));
    mPortNames.insert(TPair("BT", "BTComm"));
    mReadInProgress = false;
    mWriteInProgress = false;
}

NativeCommConnection::~NativeCommConnection()
{
    JELOG2(ESOCKET);

}

int NativeCommConnection::doGetBaudRate()
{
    JELOG2(ESOCKET);
    if (mConnectionIsOpen == false)
        return -1;
    return TBpsToTInt(mCommConfig.iRate);

}
int NativeCommConnection::getBaudRate()
{
    JELOG2(ESOCKET);
    int rate = 0;
    CallMethod(rate,this, &NativeCommConnection::doGetBaudRate,iFuncServ);
    ILOG1(ESOCKET, "NativeCommConnection::getBaudRate(), returning %d",rate);
    return rate;

}

int NativeCommConnection::doSetBaudRate(const int aBaudRate)
{
    JELOG2(ESOCKET);
    int oldvalue = doGetBaudRate();
    TBps tmpRate = mCommConfig.iRate;       // store the old rate, in case the setConfig fails, restore back this old one

    mCommConfig.iRate = TIntToTBps(aBaudRate);
    TCommConfig configBuf;
    TCommConfigV01& configRef = configBuf();
    configRef = mCommConfig;
    int ret = mComm.SetConfig(configBuf);
    if (ret < 0)
    {
        ELOG1(ESOCKET, "NativeCommConnection::doSetBaudRate(), setConfig failed for input %d",aBaudRate);
        /* value provided in aBaudRate is not supported, so restore back the old one */
        mCommConfig.iRate = tmpRate;
    }
    return oldvalue;
}

int NativeCommConnection::setBaudRate(const int aBaudRate)
{
    int oldRate = 0;
    CallMethod(oldRate,this, &NativeCommConnection::doSetBaudRate,aBaudRate,iFuncServ);
    return oldRate;
}

int NativeCommConnection::writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer,
                                     int aOffset, int aLength)
{
    JELOG2(ESOCKET);
    TBuf8<512> writeBuffer;
    int ret = S60CommonUtils::CopyToNative(aJni, aJavaBuffer, aOffset, aLength,
                                           writeBuffer);
    ILOG(ESOCKET, "NativeCommConnection::writeBytes(), copy to native completed");

    //TDesC8 tmp = writeBuffer.
    int handle = reinterpret_cast<int>(this);
    int bufhandle = reinterpret_cast<int>(&writeBuffer);


    //TRequestStatus status;
    //  mComm.Write(status, writeBuffer);
    //  User::WaitForRequest(status);

    int retValue = 0;
    CallMethod(retValue,this, &NativeCommConnection::doWrite,handle,bufhandle,iFuncServ);

    if (retValue != KErrNone)
    {
        ELOG1(ESOCKET, "NativeCommConnection::writeBytes() failed, status = %d",retValue);
        return retValue;
    }
    else
    {
        int tmp = writeBuffer.Length();
        return aLength;
    }

}


int NativeCommConnection::doWrite(int aNativeCommConnectionHandle, int aBufferHandle)
{
    JELOG2(ESOCKET);
    mWriteInProgress = true;
    NativeCommConnection *nativeObj = reinterpret_cast<NativeCommConnection*>(aNativeCommConnectionHandle);
    //TBuf8<512> writeBuffer = reinterpret_cast<TBuf8<512>>(aBufferHandle);
    TDesC8* writeBuffer = reinterpret_cast<TDesC8*>(aBufferHandle);
    TBuf8<512> tmp;

    TRequestStatus status;
    nativeObj->mComm.Write(status,*writeBuffer);
    User::WaitForRequest(status);
    ILOG1(ESOCKET, "NativeCommConnection::doWrite(), status = %d",status.Int());
    return status.Int();
}

int NativeCommConnection::doRead(JNIEnv& aJni, jbyteArray aJavaBuffer)
{
    JELOG2(ESOCKET);

    if (mInternalBuf!=NULL)
    {
        TPtrC8 tmpPtr(mInternalBuf->Des());
        /* available was called before read, so copy the data already read, which is store in internal buffer */
        S60CommonUtils::CopyToJava(aJni, tmpPtr, aJavaBuffer, 0,
                                   mInternalBuflength);
        mInternalBuf = NULL;
        ILOG1(ESOCKET, "--NativeCommConnection::doRead(), copied from internal buffer ,len = %d",mInternalBuflength);
        return mInternalBuflength;
    }

    mReadInProgress = true;
    ILOG(ESOCKET, "NativeCommConnection::doRead(), continuing");
    TRequestStatus status;
    TBuf8<512> readBuffer;
    mComm.ReadOneOrMore(status, readBuffer); // block until at least 1 byte of data is read
    User::WaitForRequest(status);
    int length = readBuffer.Length();
    TPtrC8 receiveBufferPtr = readBuffer.Ptr();


    if (status.Int() == KErrNone)
    {

        S60CommonUtils::CopyToJava(aJni, readBuffer, aJavaBuffer, 0,
                                   length);
    }
    if (status.Int() != KErrNone)
    {
        return status.Int(); // error code
    }
    else
    {
        ILOG1(ESOCKET, "NativeCommConnection::doRead(), read successfully, length = %d",length);
        return length;
    }

}

int NativeCommConnection::readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer)
{
    JELOG2(ESOCKET);
    int ret = 0;
    CallMethod(ret,this,&NativeCommConnection::doRead,aJni,aJavaBuffer,iFuncServ);
    return ret;
}

void NativeCommConnection::initializePortSettings(int *aNumericOptions)
{
    JELOG2(ESOCKET);
    // mCommConfig holds the current configuration, update it with new values
    TParity parity;
    if (aNumericOptions[ECommParityIndex] == 1)
    {
        parity = EParityOdd;
        mCommConfig.iParity = parity;
    }
    else if (aNumericOptions[ECommParityIndex] == 2)
    {
        parity = EParityEven;
        mCommConfig.iParity = parity;
    }

    if (aNumericOptions[ECommAutoCtsIndex] == 1)
        mCommConfig.iHandshake |= KConfigObeyCTS; // option: autocts
    else
        mCommConfig.iHandshake &= ~KConfigObeyCTS;

    if (aNumericOptions[ECommAutoRtsIndex] == 1)
        mCommConfig.iHandshake |= KConfigFreeRTS; // option: autorts
    else
        mCommConfig.iHandshake &= ~KConfigFreeRTS;

    if (aNumericOptions[ECommBaudRateIndex] != -1)
        doSetBaudRate(aNumericOptions[ECommBaudRateIndex]); // option: baudrate

    if (aNumericOptions[ECommBitsPerCharIndex] == 7)
        mCommConfig.iDataBits = EData7; //  option: bitsperchar
    else
        mCommConfig.iDataBits = EData8;

    if (aNumericOptions[ECommStopBitsIndex] == 1)
        mCommConfig.iStopBits = EStop1; // option: stopbits
    else
        mCommConfig.iStopBits = EStop2;

    TCommConfig configBuf1;
    TCommConfigV01& configRef = configBuf1();
    configRef = mCommConfig;
    mComm.SetConfig(configBuf1); // set the new configuration

}

std::basic_string<char> NativeCommConnection::mapToSymbianPortName(
    const std::string aJavaPortName)
{
    JELOG2(ESOCKET);
    std::map<std::string, std::string>::iterator itr = mPortNames.find(
                aJavaPortName);
    if (itr != mPortNames.end())
    {
        return itr->second; // take the value string <name,value>
    }
    return NULL;
}

int NativeCommConnection::openPortConnection(std::string aJavaPortName, int aPortNumber, int *aNumericOptions)
{
    int ret = 0;
    CallMethod(ret,this,&NativeCommConnection::openCommPortConnection,aJavaPortName,aPortNumber,aNumericOptions,iFuncServ);
    return ret;
}

int NativeCommConnection::openCommPortConnection(
    const std::string aJavaPortName, int aPortNumber, int *aNumericOptions)
{
    JELOG2(ESOCKET);
    mNativePortName = mapToSymbianPortName(aJavaPortName);
    int ret = connectToCommServer();
    if (ret != KErrNone)
    {
        return ret; // error occurred
    }

    // RComm.Open() takes the argument port id in the form "IR::0" (or) "ACM::1" ,...
    // The java port number always starts from 1( MIDP spec).
    // So convert this java port number to actual native port number using the base index stored previously.

    std::basic_string<char> prtname;
    char portNumberString[50];
    sprintf(portNumberString, "%d", (aPortNumber + mPortLowIndex) - 1);
    prtname = mNativePortName + "::" + portNumberString;

    /* convert from string to TBuf16 */
    TBuf8<50> portNameUTF;
    TBuf16<50> portname;
    TPtrC8 portNamePtr((TText8*) prtname.c_str());
    portNameUTF.Copy(portNamePtr);
    portname.Copy(portNameUTF); // convert from TBuf8 to TBuf16, because RCommServer.open() accepts only TDesc16 type

    char* t5 = (char *)prtname.c_str();

    ret = mComm.Open(mCommServer, portname, ECommExclusive);
    if (ret == KErrNone)
    {
        ILOG(ESOCKET, "openCommPortConnection() success");
        mConnectionIsOpen = true;
        TCommConfig cBuf;
        TCommConfigV01& c = cBuf();
        mComm.Config(cBuf);
        mCommConfig = c;
        initializePortSettings(aNumericOptions);
    }
    ELOG1(ESOCKET, "--NativeCommConnection::openCommPortConnection()",2);
    return ret;
}

void NativeCommConnection::doOpenConnnection(int portNameHandle)
{
    JELOG2(ESOCKET);
    char *t2 = reinterpret_cast<char*>(portNameHandle);
    TBuf8<50> portNameUTF;
    TBuf16<50> portname;
    TPtrC8 portNamePtr((TText8*) t2);
    portNameUTF.Copy(portNamePtr);
    portname.Copy(portNameUTF);


    int rc = mComm.Open(mCommServer, portname, ECommExclusive);
    if (rc != KErrNone)
    {
        ELOG1(ESOCKET, "NativeCommConnection open failed: %d", rc);
    }
}

void NativeCommConnection::doStopReading()
{
    mComm.ReadCancel();     // cancel the read
    mReadInProgress = false;
    if (!mWriteInProgress)
    {
        ILOG(ESOCKET, "++NativeCommConnection::doStopReading(), releasing the connection");
        /* if there is no write going on, then release the comm port */
        mComm.Close();
        mCommServer.Close();
        mConnectionIsOpen = false;
    }
}

void NativeCommConnection::stopReading()
{
    JELOG2(ESOCKET);
    CallMethod(this, &NativeCommConnection::doStopReading,iFuncServ);
    // cancel any pending read operation
}

void NativeCommConnection::doStopWriting()
{
    mComm.WriteCancel();
    mWriteInProgress = false;
}

void NativeCommConnection::stopWriting()
{
    // cancel any pending write operation
    JELOG2(ESOCKET);
    CallMethod(this, &NativeCommConnection::doStopWriting,iFuncServ);
}

int NativeCommConnection::doAvailable()
{
    TRequestStatus status;
    TBuf8<512> readBuffer;
    mComm.ReadOneOrMore(status, readBuffer); // block until at least 1 byte of data is read
    User::WaitForRequest(status);
    int length = readBuffer.Length();
    TPtrC8 receiveBufferPtr = readBuffer.Ptr();
    if (status.Int() == KErrNone)
    {
        TRAPD(err,mInternalBuf = HBufC8::NewL(length));
        if (err < 0)
        {
            return err;
        }
        TPtr8 tmpPtr(mInternalBuf->Des());
        tmpPtr.Copy(readBuffer);
        mInternalBuflength = length;
        return length;
    }
    else
        return 0;

}

int NativeCommConnection::available()
{
    JELOG2(ESOCKET);
    int ret;
    CallMethod(ret,this, &NativeCommConnection::doAvailable,iFuncServ);
    ILOG1(ESOCKET, "NativeCommConnection::available(), returning %d",ret);
    return ret;
}

int NativeCommConnection::connectToCommServer()
{
    JELOG2(ESOCKET);
    int ret = 0;
    TInt err = User::LoadPhysicalDevice(PDD_NAME); // load physical device drivers
    if ( KErrNone != err )
	       {
 	       return err;
	       }

    err = User::LoadLogicalDevice(LDD_NAME); // load logical device drivers
    if ( KErrNone != err )
	       {
 	       return err;
	       }

    ret = mCommServer.Connect();
    if (ret != KErrNone)
        return -ret;

    /* convert from std::string * to TBuf16 */
    TBuf8<50> portNameUTF;
    TBuf16<50> portname;
    TPtrC8 portNamePtr((TText8*) mNativePortName.c_str()); // covert c-style char* to symbian specific TPtr8
    portNameUTF.Copy(portNamePtr);
    portname.Copy(portNameUTF);

    /* Load the comm module corresponding to the name */
    if (portname.Match(KSymbianIRComm1) != KErrNotFound)
        ret = mCommServer.LoadCommModule(KSymbianIRComm2);
    else if (portname.Match(KSymbianComm) != KErrNotFound)
        ret = mCommServer.LoadCommModule(KSymbianECUART);
    else if (portname.Match(KSymbianBTComm1) != KErrNotFound)
        ret = mCommServer.LoadCommModule(KSymbianBTComm2);
    else if (portname.Match(KSymbianAcm) != KErrNotFound)
        ret = mCommServer.LoadCommModule(KAcmCsyName);

    if (ret != KErrNone)
        return ret;

    // Get base offsets of specified port.
    TSerialInfo info;
    ret = mCommServer.GetPortInfo(portname, info);
    if (ret == KErrNone)
        mPortLowIndex = info.iLowUnit;

    ILOG1(ESOCKET, "NativeCommConnection::Lowunit  = %d", mPortLowIndex);

    return ret;
}

void NativeCommConnection::doClose()
{
    JELOG2(ESOCKET);
    if (mConnectionIsOpen != false)
    {
        mComm.Close();
        mCommServer.Close();
        mConnectionIsOpen = false;
    }
}

void NativeCommConnection::close()
{
    CallMethod(this, &NativeCommConnection::doClose,iFuncServ);

}

// convert a baud rate passed as a TBps into an integer,
int NativeCommConnection::TBpsToTInt(TBps aRate)
{
    JELOG2(ESOCKET);
    switch (aRate)
    {
    case EBps50:
        return 50;
    case EBps75:
        return 75;
    case EBps110:
        return 110;
    case EBps134:
        return 134;
    case EBps150:
        return 150;
    case EBps300:
        return 300;
    case EBps600:
        return 600;
    case EBps1200:
        return 1200;
    case EBps1800:
        return 1800;
    case EBps2000:
        return 2000;
    case EBps2400:
        return 2400;
    case EBps3600:
        return 3600;
    case EBps4800:
        return 4800;
    case EBps7200:
        return 7200;
    case EBps9600:
        return 9600;
    case EBps19200:
        return 19200;
    case EBps38400:
        return 38400;
    case EBps57600:
        return 57600;
    case EBps115200:
        return 115200;
    case EBps230400:
        return 230400;
    case EBps460800:
        return 460800;
    case EBps576000:
        return 576000;
    case EBps1152000:
        return 1152000;
    case EBps4000000:
        return 4000000;
    case EBps921600:
        return 921600;
    default:
        return -1;
    }
}

// convert a baud rate passed as an int into a TBps enum value,
// rates are rounded down to the next rate, anything less than 75 is set to 50
TBps NativeCommConnection::TIntToTBps(const TInt aRate)
{
    JELOG2(ESOCKET);
    if (aRate > 4000000)
        return EBpsSpecial;
    if (aRate == 4000000)
        return EBps4000000;
    if (aRate >= 1152000)
        return EBps1152000;
    if (aRate >= 921600)
        return EBps921600;
    if (aRate >= 576000)
        return EBps576000;
    if (aRate >= 460800)
        return EBps460800;
    if (aRate >= 230400)
        return EBps230400;
    if (aRate >= 115200)
        return EBps115200;
    if (aRate >= 57600)
        return EBps57600;
    if (aRate >= 38400)
        return EBps38400;
    if (aRate >= 19200)
        return EBps19200;
    if (aRate >= 9600)
        return EBps9600;
    if (aRate >= 7200)
        return EBps7200;
    if (aRate >= 4800)
        return EBps4800;
    if (aRate >= 3600)
        return EBps3600;
    if (aRate >= 2400)
        return EBps2400;
    if (aRate >= 2000)
        return EBps2000;
    if (aRate >= 1800)
        return EBps1800;
    if (aRate >= 1200)
        return EBps1200;
    if (aRate >= 600)
        return EBps600;
    if (aRate >= 300)
        return EBps300;
    if (aRate >= 150)
        return EBps150;
    if (aRate >= 134)
        return EBps134;
    if (aRate >= 110)
        return EBps110;
    if (aRate >= 75)
        return EBps75;
    return EBps50;
}

