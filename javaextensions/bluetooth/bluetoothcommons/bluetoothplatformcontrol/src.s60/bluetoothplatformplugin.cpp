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



/*
 N.B. at present this plugin is not designed to allow multiple operations to be
 carried out at once.  However it does not explicitly enforce this requirement.
 */

#include <bt_sock.h>
#include <e32std.h>

#include "logger.h"
#include "s60btdialog.h"
#include "bluetoothplatformplugin.h"
#include "bluetoothplatformconsts.h"

// Wait after changing BT power state.
#define MAX_WAIT_COUNT 20

/*
 * the following inline functions can be used as a standard Share()
 * interface when connecting to other subsystem's servers.
 */

// NB Always uses EAutoAttach
inline TInt ShareSession(RSessionBase& session,
                         RSessionBase::TAttachMode /*unused*/= RSessionBase::EAutoAttach)
{
    return session.ShareAuto();
}

using namespace java::bluetooth;

CBluetoothPlatformPlugin* CBluetoothPlatformPlugin::NewL()
{
    CBluetoothPlatformPlugin* self = new(ELeave) CBluetoothPlatformPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CBluetoothPlatformPlugin::CBluetoothPlatformPlugin()
{
}

void CBluetoothPlatformPlugin::ConstructL()
{
    LOG(EJavaBluetooth, EInfo, "+ CBluetoothPlatformPlugin::ConstructL()");

    mBTSettings = CBTEngSettings::NewL();

    InitBTL();
    LOG(EJavaBluetooth, EInfo, "- CBluetoothPlatformPlugin::ConstructL()");
}

CBluetoothPlatformPlugin::~CBluetoothPlatformPlugin()
{
    JELOG2(EJavaBluetooth);
    if (mBTSettings)
    {
        delete mBTSettings;
        mBTSettings = NULL;
    }
    mSocketServ.Close();
}

void CBluetoothPlatformPlugin::GetProperty(const TDesC& aName, TInt& aValue)
{
    JELOG2(EJavaBluetooth);
    if (aName == KPropertyPowerOn())
    {
        TBTPowerStateValue state;
        aValue = 0;
        TInt status = mBTSettings->GetPowerState(state);
        if (KErrNone == status)
        {
            state == EBTPowerOn ? aValue = 1 : aValue = 0;
        }
    }
    else if (aName == KPropertyDiscoverability())
    {
        TBTVisibilityMode mode;
        aValue = KNotDiscoverable;
        TInt status = mBTSettings->GetVisibilityMode(mode);
        if (KErrNone == status)
        {
            switch (mode)
            {
            case EBTVisibilityModeGeneral:
                aValue = KGIAC;
                break;
            case EBTVisibilityModeTemporary:
                aValue = KLIAC;
                break;
            case EBTVisibilityModeHidden:
                // fall through
            default:
                aValue = KNotDiscoverable;
                break;
            }
        }
    }
}

void CBluetoothPlatformPlugin::InitBTL()
{
    JELOG2(EJavaBluetooth);
    // make sure Bluetooth stack is initialised by starting the L2CAP protocol.
    User::LeaveIfError(mSocketServ.Connect());

    User::LeaveIfError(ShareSession(mSocketServ));
    TRequestStatus status;
    mSocketServ.StartProtocol(KBTAddrFamily, KSockSeqPacket, KL2CAP, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());

    LOG(EJavaBluetooth, EInfo, "- CBluetoothPlatformPlugin::InitBTL()");
}

void CBluetoothPlatformPlugin::AskPowerL(const TDesC& aAppName,
        const TBool aInstalling, TPtrC& aResult)
{
    LOG(EJavaBluetooth, EInfo, "+ CBluetoothPlatformPlugin::AskPowerL()");

    CS60BTDialog* dialog = CS60BTDialog::NewLC();
    if (dialog->ShowBTQueryDialog(aAppName, aInstalling, EBTIsOffJavaQuery))
    {
        aResult.Set(KAskPowerYes);
    }
    else
    {
        aResult.Set(KAskPowerNo);
    }
    CleanupStack::PopAndDestroy(dialog);
    LOG(EJavaBluetooth, EInfo, "- CBluetoothPlatformPlugin::AskPowerL()");
}

void CBluetoothPlatformPlugin::AskDiscoverabilityL(const TDesC& aAppName,
        const TBool aInstalling, TPtrC& aResult)
{
    LOG(EJavaBluetooth, EInfo,
        "+ CBluetoothPlatformPlugin::AskDiscoverabilityL()");
    CS60BTDialog* dialog = CS60BTDialog::NewLC();
    if (dialog->ShowBTQueryDialog(aAppName, aInstalling, EBTIsNotShownQuery))
    {
        aResult.Set(KAskDiscoverabilityYes);
    }
    else
    {
        aResult.Set(KAskDiscoverabilityNo);
    }
    CleanupStack::PopAndDestroy(dialog);
    LOG(EJavaBluetooth, EInfo,
        "- CBluetoothPlatformPlugin::AskDiscoverabilityL()");
}

TInt CBluetoothPlatformPlugin::SetPower(TBool aValue)
{
    LOG1(EJavaBluetooth, EInfo,
         "+ CBluetoothPlatformPlugin::SetPower aValue=%D", aValue);

    TBTPowerStateValue state = (aValue ? EBTPowerOn : EBTPowerOff);
    TInt status = mBTSettings->SetPowerState(state);
    if (KErrNone == status)
    {
        // Let's wait for a while for the BT power state change to take
        // effect before returning. If this method returns too early,
        // the next power on check returns incorrectly the old value.
        TBTPowerStateValue oldState = (aValue ? EBTPowerOff : EBTPowerOn);
        TBTPowerStateValue newState = oldState;
        for (TInt maxWait = 0; oldState == newState && maxWait < MAX_WAIT_COUNT; maxWait++)
        {
            User::After(100 * 1000); // 100 milliseconds
            TInt status2 = mBTSettings->GetPowerState(newState);
            if (KErrNone != status2)
            {
                break;
            }
        }
        status = (oldState == newState);
    }

    LOG1(EJavaBluetooth, EInfo,
         "- CBluetoothPlatformPlugin::SetPower status=%D", status);
    return status;
}

TInt CBluetoothPlatformPlugin::SetDiscoverability(TInt aValue)
{
    LOG1(EJavaBluetooth, EInfo,
         "+ CBluetoothPlatformPlugin::SetDiscoverability aValue=%D", aValue);

    ASSERT(aValue == KNotDiscoverable || (aValue >= 0x9E8B00 && aValue
                                          <= 0x9E8B3F));
    TInt err = KErrNone;

    TBTVisibilityMode mode = EBTVisibilityModeHidden;
    switch (aValue)
    {
    case KGIAC:
        mode = EBTVisibilityModeGeneral;
        LOG1(
            EJavaBluetooth,
            EInfo,
            "* CBluetoothPlatformPlugin::SetDiscoverability GIAC (mode=%D)",
            mode);
        break;
    case KLIAC:
        mode = EBTVisibilityModeTemporary;
        LOG1(
            EJavaBluetooth,
            EInfo,
            "* CBluetoothPlatformPlugin::SetDiscoverability LIAC (mode=%D)",
            mode);
        break;
    case KNotDiscoverable:
        mode = EBTVisibilityModeHidden;
        LOG1(
            EJavaBluetooth,
            EInfo,
            "* CBluetoothPlatformPlugin::SetDiscoverability Hidden (mode=%D)",
            mode);
        break;
    default:
        err = KErrNotSupported;
    }
    if (KErrNone == err)
    {
        // Time during which the phone remains in visible state is only
        // used with EBTDiscModeTemporary, otherwise it is ignored.
        // Value is given in minutes. According to JSR082 v1.1
        // specification the limited discoverability time must be 1 minute.
        // This is stated in LocalDevice.setDiscoverable() javadocs.
        err = mBTSettings->SetVisibilityMode(mode, 1);
    }
    LOG1(EJavaBluetooth, EInfo,
         "- CBluetoothPlatformPlugin::SetDiscoverability status=%D", err);

    return err;
}
