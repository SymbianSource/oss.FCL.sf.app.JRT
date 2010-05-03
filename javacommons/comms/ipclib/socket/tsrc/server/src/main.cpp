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

#include "transport.h"
#include "logger.h"
#include "comms.h"



using namespace java::comms;

class MessageHandler : public IpcListener
{
public:
    MessageHandler()
    {
        mCon.reset(IpcConnectionFactory::createServerConnection(this));
    };
    // Listener methods
    virtual void processMessage(const ipcMessage_t* aMsg)
    {
        std::cout << "msg="
                  << std::string((const char*)&aMsg->data, aMsg->ipcHeader.length-sizeof(aMsg->ipcHeader))
                  << std::endl;

        mCon->send((ipcMessage_t*)aMsg);
    };
    virtual void onStart() {};
    virtual void onExit() {};

public:
    std::auto_ptr<IpcServerConnectionInterface> mCon;

};


int main()
{
    MessageHandler h;

    int rc = h.mCon->start(IPC_ADDRESS_JAVA_CAPTAIN_C);
    std::cout << "start rc=" << rc << std::endl;

    char* buf= new char[200];
    ipcMessage_t* pMsg = reinterpret_cast<ipcMessage_t*>(buf);

    std::cout << "main() thread loop. Press q to exit" << std::endl;
    int i = 0;
    while (!rc)
    {
        char inputChar;
        std::cin >> inputChar;

        if (inputChar == 'q')
        {
            break; // Exit the while loop
        }
        i++;
        sprintf(pMsg->data, "hello from socketserver %d", i);
        pMsg->ipcHeader.length = sizeof(ipcHeader_t) + strlen(pMsg->data);

        h.mCon->send(pMsg);
    }

    h.mCon->stop();
    delete[] buf;

    return 0;
}


