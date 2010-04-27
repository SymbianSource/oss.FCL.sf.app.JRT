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

#include <e32def.h>
#include <c32comm.h>
#include <e32std.h>
#include <usbman.h>
#include <acminterface.h>
#include "com_nokia_mj_impl_properties_comm_DynamicPropertyHandler.h"
#include "s60commonutils.h"
#include "logger.h"

_LIT(KIRCsyName, "IRCOMM"); // IR CSY
_LIT(KSerialCsyName, "ECUART"); // Serial CSY
_LIT(KBluetoothCsyName, "BTCOMM"); // BT CSY

_LIT(KSerialPort, "COM");
_LIT(KSymbianSerialPort, "COMM");

// USB
_LIT(KUSBPort, "USB");
_LIT(KSymbianUSBPort, "ACM");

_LIT(KIRPort, "IR");
_LIT(KSymbianIRPort, "IrCOMM");

// BT
_LIT(KBTPort, "BT");
_LIT(KSymbianBTPort, "BTCOMM");

using namespace java::util;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_comm_DynamicPropertyHandler__1getAvailCommPortInfo
(JNIEnv* aJni, jobject /*peer*/)
{
    JELOG2(ESOCKET);
    TBuf<700> names;
    TBuf<10> javaName;
    TSerialInfo info;
    bool validPort = true;
    RCommServ cserver;
    TInt r = cserver.Connect();

    if (r != KErrNone)
    {
        return NULL;
    }
    r = cserver.LoadCommModule(KSerialCsyName); // Serial
    r = cserver.LoadCommModule(KIRCsyName); // IR
    r = cserver.LoadCommModule(KAcmCsyName); // USB
    r = cserver.LoadCommModule(KBluetoothCsyName); // BT

    if (r != KErrNone)
    {
        return NULL;
    }
    TInt numPorts = 0;
    cserver.NumPorts(numPorts);
    TInt count;
    TBuf<10> name;
    TBuf<40> tempname;
    TBuf<10> buf;
    _LIT(KDefaultPort, "USB2");
    names.Append(KDefaultPort); // first value in the returned string , should be valid one (MIDP spec)
    for (count = 0; count < numPorts; count++)
    {
        r = cserver.GetPortInfo(count, buf, info);
        validPort = true;
        name = info.iName; // will be one of these COMM,IrCOMM,BTCOMM,ACM

        if (info.iName.MatchF(KSymbianSerialPort) != KErrNotFound)
        {
            //its a serial port
            javaName = KSerialPort;
        }
        else if (info.iName.MatchF(KSymbianIRPort) != KErrNotFound)
        {
            //infrared
            javaName = KIRPort;
        }
        else if (info.iName.MatchF(KSymbianBTPort) != KErrNotFound)
        {
            //bluetooth
            javaName = KBTPort;
        }
        else if (info.iName.MatchF(KSymbianUSBPort) != KErrNotFound)
        {
            //usb
            javaName = KUSBPort;
        }
        else
        {
            validPort = EFalse;
        }
        if (!validPort)
        {
            continue;
        }
        /* for each port, get the start index to end index number */
        for (int j = info.iLowUnit, k = 1; j <= info.iHighUnit; ++j, k++)
        {
            tempname.Format(_L("%S%u"), &javaName, k); // similar to sprintf() function
            if (tempname.Compare(KDefaultPort) == 0) // skip the default one as it's already added
            {
                continue;
            }
            if ((names.MaxLength() - names.Length()) > tempname.Length())
            { // enough space in the buffer
                names.Append(',');
                names.Append(tempname);
            }

        }
    }
    jstring javaString = NULL;
    javaString = S60CommonUtils::NativeToJavaString(*aJni, names);
    return javaString;
}
