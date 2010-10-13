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

#include <es_enum.h>
#include <es_sock.h>
#include <in_sock.h>
#include "socketlocalhostinfo.h"
#include "logger.h"
#include "connectionmanager.h"

int GetlocalIPAdressL(char *localaddr, int aMidletIapId, int aApType);

OS_EXPORT int SocketLocalHostInfo::getLocalAddress(int /* aSd */, char *aLocalAddr,
        int aMidletIapId, int aApType)
{
    TRAPD(err,GetlocalIPAdressL(aLocalAddr,aMidletIapId, aApType););
    return err;

}

int getIPAddressL(const int aIapId, char *ipAddress)
{
    RSocketServ socketServ;
    RSocket sock;
    User::LeaveIfError(socketServ.Connect());
    User::LeaveIfError(sock.Open(socketServ, KAfInet, KSockStream,
                                 KProtocolInetTcp));

    //  find the ip address of the active interface
    TSoInetInterfaceInfo ifInfo;
    TPckg<TSoInetInterfaceInfo> ifInfoPkg(ifInfo);
    TSoInetIfQuery ifQuery;
    TPckg<TSoInetIfQuery> ifQueryPkg(ifQuery);

    // To find out which interfaces are using our current IAP, we must
    // enumerate and go through all of them and make a query by name for each.
    ILOG(ESOCKET, "Looping thru all the destinations and ap \n\n");
    User::LeaveIfError(sock.SetOpt(KSoInetEnumInterfaces,
                                   KSolInetIfCtrl));

    while (sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, ifInfoPkg)
            == KErrNone)
    {
        ifQuery.iName = ifInfo.iName;
        TInt err = sock.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery,
                               ifQueryPkg);
        if ((err == KErrNone) && (ifQuery.iZone[1] == aIapId))
        {
            ILOG2(ESOCKET, "Network id = %d, IAP id = %d ",(TUint32)(ifQuery.iZone[15]),(TUint32)ifQuery.iZone[1]);

            // IAP ID is index 1 of iZone
            // We have found an interface using the IAP we are interested in.
            if (ifInfo.iAddress.Address() > 0)
            {
                if (!ifInfo.iAddress.IsUnspecified()
                        && !ifInfo.iAddress.IsLoopback()
                        && !ifInfo.iAddress.IsLinkLocal())
                {
                    // found a IPv4 address
                    TBuf8<20> aIP8;
                    TBuf<20> aIP;
                    TInetAddr aAddr;
                    aAddr = ifInfo.iAddress;
                    aAddr.ConvertToV4();
                    aAddr.Output(aIP);
                    aIP8.Copy(aIP);

                    strncpy(ipAddress, (char*) aIP8.Ptr(),
                            aIP8.Length());
                    ipAddress[aIP8.Length()] = '\0';
                    ILOG1(ESOCKET, "GetlocalIPAdressLLL , ip = %s",
                          ipAddress);
                    sock.Close();
                    socketServ.Close();
                    ILOG(ESOCKET, "returning from getIpAddr");
                    return KErrNone;
                }
            }  // end if addr > 0
            else if (err != KErrNone)
            {
                sock.Close();
                return err; // return with error
            }
        }
    }   // end while
    sock.Close();
    socketServ.Close();
    return KErrNone;

}

/**
 *In Symbian, when a server connection is opened, it is not bound to any interface
 *so the openC calls returns "0.0.0.0" as the local address.
 *To fetch the proper address (as in native apps), we check for any active connection(s)
 *on the device. If there is an active con and is same as midlet configured AP or device default AP
 *then the IP address of that interface is fetched and returned.
 */

int GetlocalIPAdressL(char *localaddr, int aMidletIapId, int aType)
{
    JELOG2(ESOCKET);
    int err = KErrNone;
    TUint32 activeIapId = 0;
    TConnectionInfoBuf connectionInfo;
    TUint count = 0;
    bool flg = false;
    RSocketServ socketServ;
    RSocket sock;
    RConnection conn;

    User::LeaveIfError(socketServ.Connect());
    User::LeaveIfError(sock.Open(socketServ, KAfInet, KSockStream,
                                 KProtocolInetTcp));
    User::LeaveIfError(conn.Open(socketServ));
    User::LeaveIfError(conn.EnumerateConnections(count));

    ILOG1(ESOCKET, "Active connections count = %d ", count);
    strcpy(localaddr, "0.0.0.0");
    if (count <= 0)
    {
        // no active connections on the device , return "0.0.0.0"
        return KErrNone;
    }
    else
    {
        // We have an active connection

        for (int j = 1; j <= count; j++)    // loop thru all active connection(s)
        {
            User::LeaveIfError(conn.GetConnectionInfo(j, connectionInfo));
            activeIapId = connectionInfo().iIapId;
            ILOG1(ESOCKET, "Active connection iap id = %d ", activeIapId);
            if (aType > 0)
            {
                switch (aType)
                {
                case 2:
                    flg = ConnectionManager::isIapDefault(activeIapId, aMidletIapId,false);
                    if (!flg)
                        continue;
                    break;
                case 3:
                    if (activeIapId != aMidletIapId)
                        continue; // go to the next active IAP
                    break;
                default:
                    break;
                }


            }
            else
            {
                // no access point configured for the midlet, check system default
                bool flag = ConnectionManager::isIapDefault(activeIapId, 0);
                ILOG1(ESOCKET, "Connection Manager isIapDefault returned %d ",
                      flag);
                if (!flag)
                    continue; // go to the next active IAP
            }

            // find the ip address of the active IAP
            err = getIPAddressL(activeIapId,localaddr);
            break;  // once we find the IP addr, return

        } // end for loop

    } // end else active connection
    ILOG(ESOCKET, "GetlocalIPAdressL , got ip ");
    conn.Close();
    sock.Close();
    socketServ.Close();

    return err; // return with KErrNotFound


}
