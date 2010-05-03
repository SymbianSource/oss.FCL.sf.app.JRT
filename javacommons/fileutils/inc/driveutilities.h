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
* Description:  DriveUtilities
*
*/

#ifndef DRIVEUTILITIES_H
#define DRIVEUTILITIES_H

#include "javaosheaders.h"
#include "comms.h"

#include <string>
#include <vector>

namespace java
{
namespace comms
{
class CommsEndpoint;
} // namespace comms

namespace fileutils
{

struct driveInfo
{
    driveInfo():iIsPresent(false), iIsRemovable(false),
            iIsLocal(false), iIsReadOnly(false), iId(0), iIsExternallyMountable(false)
    {}

    std::wstring  iRootPath;
    bool          iIsPresent;
    bool          iIsRemovable;
    bool          iIsLocal;
    bool          iIsReadOnly;
    unsigned int  iId;
    bool          iIsExternallyMountable;
};

typedef std::vector<driveInfo> driveInfos;

class DriveListenerInterface
{
public:
    enum
    {
        REMOVABLE_MEDIA_INSERTED_C     = 1,
        REMOVABLE_MEDIA_REMOVED_C,  // = 2
        // possibe future operations
        // for remote drives
        // DRIVE_MOUNTED_C,
        // DRIVE_UNMOUNTED_C
        // for local drives,
        // DRIVE_ADDDED_C
        // DRIVE_REMOVED_C
    };

    virtual void driveChanged(const int& aOperation, const driveInfo& aDriveInfo) = 0;
};

class DriveObserverServerInterface
{
public:
    virtual ~DriveObserverServerInterface() = 0;

    virtual void startServer(java::comms::CommsEndpoint* aComms) = 0;
    virtual void stopServer() = 0;

    // For local listeners
    virtual void registerListener(DriveListenerInterface* aListener) = 0;
    virtual void unregisterListener(DriveListenerInterface* aListener) = 0;
};

OS_NONSHARABLE_CLASS(DriveUtilities)
{
public:

    ////////////// Client API start //////////////////////////////////////////
    // These methods are intended for client users of drive observer API
    /**
     * Returns the list of all defined drives/roots within the system.
     *
     * @param[in/out] aDriveInfo: a reference to a collection which receives
     *                the all defined drives within the system
     */
    OS_IMPORT static void getAllDrives(driveInfos& aDriveInfos);

    /**
     * Returns the list of available drives/roots wich are present and accesible.
     *
     * @param[in/out] aDriveInfo: a reference to a collection which receives
     *                the existing drives with accesible media within the system
     */
    OS_IMPORT static void getAccesibleDrives(driveInfos& aDriveInfos);

    /**
     * Registers a listener for drive changed events
     *
     * @param[in] aListener: pointer to an interface implementing callback methods
     */
    OS_IMPORT static void registerListener(DriveListenerInterface* aListener,
                                           const int& aServerAddress = java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C);
    /**
     * Unregisters a listener for drive changed events
     *
     * @param[in] aListener: pointer to an interface implementing callback methods
     */
    OS_IMPORT static void unregisterListener(DriveListenerInterface* aListener,
            const int& aServerAddress = java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C);
    ////////////// Client API stop //////////////////////////////////////////

    ////////////// Server API start //////////////////////////////////////////
    // Following methods are to be used only by DriveObserver server instantianter
    // which is currently javacaptain. The interface to create a server side instance
    // is here in order to enable drive observer functionality also on platforms
    // and configurations which do not have javacaptain running
    ////////////// Server API start //////////////////////////////////////////
    OS_IMPORT static DriveObserverServerInterface* getDriveObserverServer();
};

} // end of namespace fileutils
} // end of namespace java
#endif // DRIVEUTILITIES_H

