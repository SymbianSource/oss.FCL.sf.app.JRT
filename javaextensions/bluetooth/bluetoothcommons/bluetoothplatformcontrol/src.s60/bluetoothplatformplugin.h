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



#ifndef BLUETOOTHPLATFORMPLUGIN_H
#define BLUETOOTHPLATFORMPLUGIN_H

#include <e32base.h> // Can be removed after btengsettings.h includes it.
#include <btengsettings.h>
#include <in_sock.h>

namespace java
{
namespace bluetooth
{

class CBluetoothPlatformPlugin: public CBase
{
public:
    static CBluetoothPlatformPlugin* NewL();

    void GetProperty(
        const TDesC& aName, TInt& aValue);

    void AskPowerL(
        const TDesC& aAppName, const TBool aInstalling,
        TPtrC& aResult);

    void AskDiscoverabilityL(
        const TDesC& aAppName, const TBool aInstalling,
        TPtrC& aResult);

    TInt SetPower(TBool aValue);

    TInt SetDiscoverability(TInt aValue);

    ~CBluetoothPlatformPlugin();

private:
    CBluetoothPlatformPlugin();

    void ConstructL();

    void InitBTL();

private:
    RSocketServ mSocketServ;
    CBTEngSettings *mBTSettings;
};

} //end namespace java
} //end namespace bluetooth

#endif // BLUETOOTHPLATFORMPLUGIN_H
