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


#ifndef SOCKETLOCALHOSTINFO_H
#define SOCKETLOCALHOSTINFO_H

class SocketLocalHostInfo
{

public:

    static int getLocalAddress(int aSd, char *aLocalAddr, int aMidletIapId, int aApType);


};

#endif // SOCKETLOCALHOSTINFO_H
