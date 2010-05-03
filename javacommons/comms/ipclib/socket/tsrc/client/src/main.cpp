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

#include <iostream>
#include <string>
#include <memory>

#include "logger.h"

#include "transport.h"
#include "comms.h"

using namespace java::comms;

class MessageHandler : public IpcListener
{
public:
    // Listener methods
    virtual void processMessage(const ipcMessage_t* aMsg)
    {
        std::cout << "msg="
                  << std::string((const char*)&aMsg->data, aMsg->ipcHeader.length-sizeof(aMsg->ipcHeader))
                  << std::endl;
    };
    virtual void onStart() {};
    virtual void onExit() {};
};


int main()
{
    MessageHandler h;
    std::auto_ptr<IpcConnectionInterface> con(IpcConnectionFactory::createConnection(&h));

    int rc = con->connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    std::cout << "connect rc=" << rc << std::endl;

    char* buf= new char[200];
    ipcMessage_t* pMsg = reinterpret_cast<ipcMessage_t*>(buf);
    sprintf(pMsg->data, "hello from socketclient");
    pMsg->ipcHeader.length = sizeof(ipcHeader_t) + strlen(pMsg->data);

    rc = con->send(pMsg);
    std::cout << "send rc=" << rc << std::endl;

    std::cout << "main() thread loop. Press q to exit" << std::endl;
    int i = 0;
    while (rc==0)
    {
        char inputChar;
        std::cin >> inputChar;

        if (inputChar == 'q')
        {
            break; // Exit the while loop
        }
        i++;
        sprintf(pMsg->data, "hello from socketclient %d", i);
        pMsg->ipcHeader.length = sizeof(ipcHeader_t) + strlen(pMsg->data);
        con->send(pMsg);
    }

    con->disconnect();
    delete[] buf;

    return 0;
};


