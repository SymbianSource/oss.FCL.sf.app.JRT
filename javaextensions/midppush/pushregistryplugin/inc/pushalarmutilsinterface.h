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
* Description:
*
*/


#ifndef PUSHALARMUTILSINTERFACE_H
#define PUSHALARMUTILSINTERFACE_H

#include "rtcinterface.h"
#include "javauid.h"

namespace java
{
namespace push
{

/**
 * This interface provides method to launch a MIDlet and get information from
 * push registrations.
 */
class PushAlarmUtilsInterface
{
public:

    PushAlarmUtilsInterface() {}

    /**
     * This method launches a MIDlet.
     * @param aMidletUid UID of the MIDlet.
     * @param aLaunchType Launching mode.
     * @returns true if drive, where MIDlet was installed, was available.
     *          Otherwise false.
     * @throws This operation does not throw exceptions.
     */
    virtual bool launchMidlet(const java::util::Uid& aMidletUid,
                              const int& aLaunchType) = 0;

    /**
     * This operation returns MIDlet's media uid.
     * @return MIDlet's media id. Returns -2 if MIDlet by uid is not found.
     *         Returns -1 if "is mmc available" check is not needed to do.
     */
    virtual int getMediaIdByMidletUid(const java::util::Uid& aMidletUid) = 0;

};


}//end namespace push
}//end namespace java

#endif // PUSHALARMUTILSINTERFACE_H

