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
* Description:  Java Connection Manager
*                Provides connection management for all Java Connections:
                 - Network Access Point selection
*
*/


#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <e32base.h>
#include <javauid.h>

using namespace java::util; // Uid
static const int KJavaNetworkAccessNotSpecified = -1; // returned when no IAP/SNAP is defined in java storage
static const int KAlwaysAskId = -2; // returned in case of AskAlways case
static const int KDefaultConnection = -3; // froe default connection case

/**
 *  Java Connection Manager
 *  Java The Connection Manager caches and handles Network Access Point selection.
 *  The component is central point for all Java Connection Implementations
 *  requiring Network Access Point selection.
 *
 *  Network Access Point selection
 *  Java Connection Manager component handles Network Access Point selection
 *  in following priority steps:
 *  1. Is SNAP ID parameter (nokia_netid) specified in the URI.
 *  2. Is IAP ID parameter (nokia_apnid) specified in the URI.
 *  3. Is Session default Network Access Point (SNAP/IAP) set?
 *  4. Is MIDlet suite default Network Access Point (SNAP/IAP) defined in CenRep?
 *  5. Prompt User to select SNAP/IAP. If Default Connection is set,
 *     it is used instead and User is not prompted at all.
 *
 *  User SNAP/IAP selection is set as Session default Network Access Point.
 *  The Session default Network Access Point exists until MIDlet is closed.
 *
 *  Usage:
 *
 *  @code
 *  TJavaNetworkAccessPoint access = ConnectionManager::SelectNetworkAccessPointL( appUid );
 *  if ( access.Type() == ESnap )
 *      // start connection with access.Id() as SNAP paramater
 *  else if ( access.Type() == EIap )
 *      // start connection with access.Id() as IAP paramater
 *  else
 *      // start connection without selected Network Access Point
 *  @endcode
 *
 *  @lib javaconnectionmanager.dll
 */
class ConnectionManager
{

    /**
     * Handles and caches the Network Access Point selection for
     * Java Connection Implementation.
     *
     *  Start
     *   |
     * Is Session default Network Access Point set? - Yes --------------------+
     *   |                                                                    |
     *   No                                                                   |
     *   |                                                                    |
     * MIDlet suite default Network Access Point defined - Yes ---------------+
     *   |                                                                    |
     *   No                                                                   |
     *   |                                                                    |
     * Prompt User to select SNAP/IAP ID                                      |
     *   |                                                                    |
     *   +-- Cache selection as Session default Network Access Point ---------+
     *                                                                        |
     *                                                                        |
     *                                                    Return Network Access Point
     *
     * @param aAppUid Java application UID
     * @return Selected Java Network Access Point
     */



public:

    /**
    * Prompt User to select Network Access Point
    * If Default Connection is defined in platform, the Default Connection is used instead.
    * @return Selected Network Access Point
    */

    static bool SelectNetworkAccessPoint(char * , int *);


    /* prompt the user for
    * access point
    */

    static void PromptUserL(char *);

    /* check if get the
    * device default access point
    */

    static void GetDeviceDefaultAccessPointL(char *);

    /**
    * Get the default IAP defined for the java application
    * from java storage
    * @param Application Suite UID
    * @return default IAP
    */

    IMPORT_C static unsigned int getApnIdL(Uid aAppSuiteUid);

    /**
        * Sets default IAP defined for the java application
        * defined by App UID into java storage
        * @param Application Suite UID
        * @param default IAP Id
        */

    IMPORT_C static void setApnIdL(Uid aAppSuiteUid, int aApnId);

    /**
       * Sets default SNAP defined for the java application
       * defined by App UID into java storage
       * @param Application Suite UID
       * @param default IAP Id
       */

    IMPORT_C static void setDestinationNetworkIdL(Uid aAppSuiteUid, int aApnId);

    /**
        * Get the default SNAP defined for the java application
        * from java storage
        * @param Application Suite UID
        * @return default SNAP ID
        */

    IMPORT_C static unsigned int getDestinationNetworkIdL(Uid aAppSuiteUid);

    /**
        * Checks if the given IAP id falls in the destination id.
        * @param aMatchIapId , iap id
        * @param aDestId,  destination id. If this param is -1 then it checks the iap in default SNAP
        * @return true if given ap is present in the SNAP , false otherwise
        */
    IMPORT_C static bool isIapDefault(TUint32 aMatchIapId, TUint32 aDestId, bool aDefault = true);

private:

    static HBufC8 * PromptUserSelectNetworkAccessPointL();
    static HBufC8 * CreateDescriptorL(const int aType, const int id);
    static bool checkIapDefaultL(TUint32 aMatchIapId, TUint32 aDestId, bool aDefault);



    static unsigned int ParseNetworkAccessPointL(const TDesC8& aDes);


};

#endif // CONNECTIONMANAGER_H
