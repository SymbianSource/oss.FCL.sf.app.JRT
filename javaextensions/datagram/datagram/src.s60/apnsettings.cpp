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
* Description:  ApnSettings.app
*
*/



#include <string>
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include "logger.h"
#include "apnsettings.h"

using namespace java;

int ApnSettings::setDefaultApn(int aType, int aApn)
{
    struct ifreq  ifr;
    int def = 0;

#ifdef RD_JAVA_OPENC_BETA_PATCH
    if (aType == 2 || aType == 3)
    {
        if (aType == 3) // IAP ID
        {
            char * interfacename = new char[512];

            char * fret = if_indextoname(aApn,interfacename);
            strcpy(ifr.ifr_name, interfacename);
            ILOG1(ESOCKET, "interface name is %s",interfacename);
        }
        else
        {
            if (aType == 2) // SNAP ID
            {
                memset(&ifr, 0, sizeof(struct ifreq));
                ifr.ifr_ifru.snap_id = aApn;
            }
        }
        def = setdefaultif(&ifr);
        ILOG(ESOCKET, "sedefaultif called");
    }
    ILOG1(ESOCKET, "sedefaultif returned %d",def);
    // if(def == -1)
    //{
    //      ELOG(ESOCKET, "default access point interface is removed");
    //      setdefaultif(NULL);
    //}
#endif


    return def;
}

void ApnSettings::removeDefaultApn()
{

    int def = 0;

#ifdef RD_JAVA_OPENC_BETA_PATCH
    def = setdefaultif(NULL);
    ILOG(ESOCKET, "sedefaultif called");
#endif
    (void)def; //just to suppress a warning
    ILOG1(ESOCKET, "remove default apn returned %d",def);

}
