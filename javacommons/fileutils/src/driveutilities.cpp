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
* Description:  DriveUtilities - platform independent
*
*/


#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include <pls.h>
#endif

#include "logger.h"

#include "driveobserverclient.h"
#include "driveobserverserver.h"

#include "driveutilities.h"

namespace java
{
namespace fileutils
{

// Empty declaration for a pure virtual destructor
DriveObserverServerInterface::~DriveObserverServerInterface()
{}


OS_EXPORT void DriveUtilities::getAccesibleDrives(driveInfos& aDriveInfos)
{
    JELOG2(EJavaFile);

    getAllDrives(aDriveInfos);

    for (driveInfos::iterator iter = aDriveInfos.begin() ; iter != aDriveInfos.end() ;)
    {
        if (iter->iIsPresent)
        {
            iter++;
        }
        else
        {
            iter = aDriveInfos.erase(iter);
        }
    }
}

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
DriveObserverClient* getObserverInstance()
{
    // Access the PLS of this process
    DriveObserverClient* instance  = Pls<DriveObserverClient>(TUid::Uid(0x20022FB6));
    return instance;
}
void deleteObserverInstance()
{

}

#else
static DriveObserverClient* sDOinstance = 0;
DriveObserverClient* getObserverInstance()
{
    if (sDOinstance == 0)
    {
        sDOinstance = new DriveObserverClient();
    }
    return sDOinstance;
}
void deleteObserverInstance()
{
    if (sDOinstance)
    {
        delete sDOinstance;
        sDOinstance = 0;
    }
}
#endif

OS_EXPORT void DriveUtilities::registerListener(DriveListenerInterface* aListener,
        const int& aServerCommsAddress)
{
    getObserverInstance()->registerListener(aListener, aServerCommsAddress);
}

OS_EXPORT void DriveUtilities::unregisterListener(DriveListenerInterface* aListener,
        const int& aServerCommsAddress)
{
    if (0 == getObserverInstance()->unregisterListener(aListener,
            aServerCommsAddress))
    {
        deleteObserverInstance();
    }
}

OS_EXPORT DriveObserverServerInterface* DriveUtilities::getDriveObserverServer()
{
    JELOG2(EJavaFile);

    return new DriveObserverServer();
}

} // end of namespace fileutils
} // end of namespace java
