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
* Description: InstallOperation provides a way to launch and communicate
*              with Java Installer process
*
*/


#ifndef INSTALLOPERATION_H
#define INSTALLOPERATION_H

#include "javaosheaders.h"
#include "commslistener.h"
#include "commsserverendpoint.h"
using namespace java::comms;

namespace java
{

namespace debug
{

// Message ids and operations used in communication with Java Installer
const int GET_NEXT_OPERATION_MESSAGE_ID = 500;
const int OPERATION_MESSAGE_ID          = 501;
const int OPERATION_RESULT_MESSAGE_ID   = 502;

const int INSTALL_OPERATION     = 0;
const int UNINSTALL_OPERATION   = 1;
const int EXIT_OPERATION        = 2;

OS_NONSHARABLE_CLASS(InstallOperation) : public CommsListener
{
protected:
    InstallOperation();

public:
    virtual ~InstallOperation();
    virtual void processMessage(CommsMessage& aMessage);

protected:
    virtual int runJavaInstaller();

    virtual CommsMessage getNextOperation();
    virtual void handleOperationResult(CommsMessage& aMessage);

    virtual int startJavaInstaller();
    virtual void sendToJavaInstaller(CommsMessage& aMessage);

private:
    CommsServerEndpoint mInstaller;
};

} // end namespace debug
} // end namespace java


#endif // INSTALLOPERATION_H
