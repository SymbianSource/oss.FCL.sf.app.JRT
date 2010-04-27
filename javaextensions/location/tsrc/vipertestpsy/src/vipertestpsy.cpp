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
* Description:
*
*/
// INCLUDES
#include <e32base.h>
#include <Lbs.h>
#include <LbsSatellite.h>
#include <ImplementationProxy.h>
#include "ViperTestPsy.hrh"
#include "EPos_CPositioner.h"

// CONSTANTS
const TReal32 KHorizontalAcc = 40;
const TReal32 KVerticalAcc = 40;

const TReal64 KLatitude = 2;
const TReal64 KLongitude = 3;
const TReal32 KAltitude = 200;

_LIT(KPositionTime, "20801027:161600");//YYYYMMDD:HHMMSS.MMMMMM

const TReal32 KSpeed = 20;
const TReal32 KHeading = 30;
const TReal32 KSpeedAcc = 2;
const TReal32 KHeadingAcc = 3;

const TInt KSatelliteId = 30;
const TReal32 KAzimuth = 30;
const TReal32 KElevation = 30;
const TBool KIsUsed = ETrue;
const TInt KSignalStrength = 10;

const TReal32 KAzimuthOdd = 35;
const TReal32 KElevationOdd = 35;
const TBool KIsUsedOdd = EFalse;
const TInt KSignalStrengthOdd = 5;

const TUint KNumberOfSatellitesInView = 12;
const TUint KNumberOfSatellitesUsed = 6;
_LIT(KSatelliteTime, "20021027:161600");
const TReal32 KHorizontalDoPValue = 1;
const TReal32 KVerticalDoPValue = 2;
const TReal32 KTimeDoPValue = 20;
_LIT(KModuleName, "ViperTest PSY");

// CLASS DECLARATION

class CViperTestPsy: public CPositioner
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CViperTestPsy* NewL(TAny* aConstructionParameters);

    /**
     * Destructor.
     */
    ~CViperTestPsy();

protected:
    // Functions from base classes

    /**
     * From CPositioner. Requests position info asynchronously.
     *
     * @param aPosInfo A reference to a position info object. This object
     *                 must be in scope until the request has completed.
     * @param aStatus The request status
     */
    void NotifyPositionUpdate(
        /* IN/OUT */TPositionInfoBase& aPosInfo,
        /* OUT    */TRequestStatus& aStatus);

    /**
     * From CPositioner. Cancels position info request.
     */
    void CancelNotifyPositionUpdate();

private:

    /**
     * C++ default constructor.
     */
    CViperTestPsy();

    /**
     * By default EPOC constructor is private.
     */
    void ConstructL(TAny* aConstructionParameters);

    void SetTPositionInfo(TPositionInfo& aInfo);
    void SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo);
    void SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo);
    void SetHPositionGenericInfo(HPositionGenericInfo& aGenericInfo);

    // By default, prohibit copy constructor
    CViperTestPsy(const CViperTestPsy&);
    // Prohibit assigment operator
    CViperTestPsy& operator=(const CViperTestPsy&);

};

// ================= OTHER EXPORTED FUNCTIONS ==============

const TImplementationProxy KFactoryPtr = IMPLEMENTATION_PROXY_ENTRY(
            KPosImplementationUid, CViperTestPsy::NewL);

//
// ---------------------------------------------------------
// ImplementationGroupProxy
// Required PSY interface.
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
{
    aTableCount = 1;
    return &KFactoryPtr;
}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CViperTestPsy::CViperTestPsy()
{
}

// EPOC default constructor can leave.
void CViperTestPsy::ConstructL(TAny* aConstructionParameters)
{
    BaseConstructL(aConstructionParameters);
}

// Two-phased constructor.
CViperTestPsy* CViperTestPsy::NewL(TAny* aConstructionParameters)
{
    CViperTestPsy* self = new(ELeave) CViperTestPsy;
    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop();
    return self;
}

// Destructor
CViperTestPsy::~CViperTestPsy()
{
}

void CViperTestPsy::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
        TRequestStatus& aStatus)
{
    TRequestStatus* status = &aStatus;

    TUid implUid =
        { KPosImplementationUid };
    aPosInfo.SetModuleId(implUid);
    TUint32 classType = aPosInfo.PositionClassType();

    if (classType == (classType & EPositionInfoClass))
    {
        TPositionInfo* position = static_cast<TPositionInfo*>(&aPosInfo);
        SetTPositionInfo(*position);
    }
    else if (classType & EPositionGenericInfoClass)
    {
        HPositionGenericInfo& generic =
            static_cast<HPositionGenericInfo&>(aPosInfo);
        SetHPositionGenericInfo(generic);
    }
    else
    {
        User::RequestComplete(status, KErrGeneral);
        return;
    }

    User::RequestComplete(status, KErrNone);
}

void CViperTestPsy::CancelNotifyPositionUpdate()
{
}

void CViperTestPsy::SetTPositionInfo(TPositionInfo& aInfo)
{
    TCoordinate coor(KLatitude, KLongitude, KAltitude);
    TLocality loc(coor, KHorizontalAcc, KVerticalAcc);
    TTime now;
    now.UniversalTime();
    TPosition pos(loc, now);
    aInfo.SetPosition(pos);
}

void CViperTestPsy::SetHPositionGenericInfo(HPositionGenericInfo& aGenericInfo)
{
    SetTPositionInfo(aGenericInfo);
    if (aGenericInfo.IsRequestedField(EPositionFieldHorizontalSpeed))
    {
        aGenericInfo.SetValue(EPositionFieldHorizontalSpeed, KSpeed);
    }
    if (aGenericInfo.IsRequestedField(EPositionFieldHeading))
    {
        aGenericInfo.SetValue(EPositionFieldHeading, KHeading);
    }
}

//  End of File
