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
#include "javacommconnection.h"
#include "nativecommconnection.h"
#include "fs_methodcall.h"

using namespace java::comm;

JavaCommConnection::JavaCommConnection(): FunctionServer("MyCommConServer")
{
    JELOG2(ESOCKET);
    createServerToNewThread();

    int handle = reinterpret_cast<int>(this);
    CallMethod(this, &JavaCommConnection::createConnection,handle,this);
    //iNativeObj = new NativeCommConnection(this);

}

void JavaCommConnection::createConnection(int aHandle)
{
    JELOG2(ESOCKET);
    JavaCommConnection *obj = reinterpret_cast<JavaCommConnection*>(aHandle);
    obj->iNativeObj = new NativeCommConnection(this);
}


JavaCommConnection::~JavaCommConnection()
{


}
