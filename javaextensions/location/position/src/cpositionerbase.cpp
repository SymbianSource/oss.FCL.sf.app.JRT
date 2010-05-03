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
* Description:  Base class for position request classes
 *
*/


// INCLUDE FILES
#include "cpositionerbase.h"
#include "s60commonutils.h"
#include "logger.h"

using namespace java::location;

// UNNAMED LOCAL NAMESPACE
namespace
{
// Default size of the position info buffer if NMEA statements are supported
// In fact, this is four times the standard buffer size
const TInt KLocPositionGenericInfoNMEABufferSize = 0x1000; // Dec 4096

// Default maximum number of fields if NMEA statements are supported
// In fact, this is four times the default field count
const TInt KLocPositionGenericInfoNMEAFieldCount = 0x0080; // Dec 128

// Default buffer size increase. This is based on to BT GPS PSY standard
// buffer which is the size of 5K. Usually 1K increase should be enough
// since it is the standard size of HPositionGenericInfo when constructed
const TInt KLocBufferSizeIncrease = 0x0400; // 1024

// Default increase for accepted fields in position info buffer. This is based
// on to MLFW default field count which is 32. The increse is synchronized
// with the buffer size increase as these values are the same default values
// which are used when HPositionGenericInfo is generated without parameters
const TInt KLocFieldCountIncrease = 0x0020; // 32

const TInt KNumFields = 17;
const TUint16 positionField[KNumFields] =
    { EPositionFieldStreetExtension, EPositionFieldStreet,
      EPositionFieldPostalCode, EPositionFieldCity, EPositionFieldCounty,
      EPositionFieldState, EPositionFieldCountry, EPositionFieldCountryCode,
      EPositionFieldDistrict, EPositionFieldBuildingName,
      EPositionFieldBuildingFloor, EPositionFieldBuildingRoom,
      EPositionFieldBuildingZone, EPositionFieldCrossing1,
      EPositionFieldCrossing2, EPositionFieldMediaLinks,
      EPositionFieldBuildingTelephone
    };
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPositionerBase::CPositionerBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPositionerBase::CPositionerBase(LocationFunctionServer* aFunctionSource) :
        CActive(EPriorityNormal), mFunctionServer(aFunctionSource), iCapabilities(
            TPositionModuleInfo::ECapabilityNone)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CPositionerBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPositionerBase::BaseConstructL(RPositionServer& aServer,
                                     TPositionModuleId aModuleId,
                                     TPositionModuleInfo::TCapabilities aCapabilities)
{
    JELOG2(EJavaLocation);

    User::LeaveIfError(iPositioner.Open(aServer, aModuleId));

    _LIT(KService, "JavaVM");
    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication, KService);

    iCapabilities = aCapabilities;

    // By default, MLFW uses 1K buffer for location information. If the
    // PSY is capable of providing NMEA statements then the default buffer
    // runs out very quickly and the buffer size needs to adjusted. This may
    // cause delays when getting location fixes to Java-side. So if the
    // module supports NMEA statements then a larger buffer is created
    // by default. For example, BT GPS PSY receives all NMEA statements
    // that the module can provide so the default buffer size is not enough
    // Also the maximum number of fields needs to be changed in that case

    if (iCapabilities & TPositionModuleInfo::ECapabilityNmea)
    {
        iPositionInfo = CreatePositionInfoL(
                            KLocPositionGenericInfoNMEABufferSize,
                            KLocPositionGenericInfoNMEAFieldCount);
    }
    else
    {
        // Use default values defined in MLFW. Currently 1K for the buffer
        // size and the maximum number of 32 fields supported.
        iPositionInfo = CreatePositionInfoL(
                            KPositionGenericInfoDefaultBufferSize,
                            KPositionGenericInfoDefaultMaxFields);
    }
}

// -----------------------------------------------------------------------------
// CPositionerBase::~CPositionerBase
// -----------------------------------------------------------------------------
//
CPositionerBase::~CPositionerBase()
{
    JELOG2(EJavaLocation);
    delete iPositionInfo;
    iPositioner.Close();
}

// -----------------------------------------------------------------------------
// CPositionerBase::GetQualifiedCoordinates
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositionerBase::GetQualifiedCoordinates(CPositionerBase* aSelf,
        JNIEnv* aJni, jdouble* alatlon, jfloat* aAltHaccVacc,
        jlong* aTimestamp, jobjectArray aNmeaData)
{
    JELOG2(EJavaLocation);

    HPositionGenericInfo* posInfo = aSelf->iPositionInfo;

    const TInt KMaxNmeaSentenceLength = 82;
    TUint8 numNmeaSentences(0);
    if (posInfo->GetValue(EPositionFieldNMEASentences, numNmeaSentences)
            == KErrNone)
    {
        HBufC* nmeaData = HBufC::New(KMaxNmeaSentenceLength * numNmeaSentences);
        if (nmeaData)
        {
            TPtr nmeaDataPtr = nmeaData->Des();
            TInt lastSentence = EPositionFieldNMEASentencesStart
                                + numNmeaSentences;
            for (TUint16 i = EPositionFieldNMEASentencesStart; i < lastSentence; ++i)
            {
                TBuf8<KMaxNmeaSentenceLength> sentence8;
                if (posInfo->GetValue(i, sentence8) == KErrNone)
                {
                    TBuf<KMaxNmeaSentenceLength> sentence;
                    sentence.Copy(sentence8);
                    nmeaDataPtr.Append(sentence);
                }
            }

            jstring jstr = java::util::S60CommonUtils::NativeToJavaString(
                               *aJni, *nmeaData);
            aJni->SetObjectArrayElement(aNmeaData, 0, jstr);

            delete nmeaData;
        }
    }

    TPosition pos;
    posInfo->GetPosition(pos);

    alatlon[0] = static_cast<jdouble>(pos.Latitude());
    alatlon[1] = static_cast<jdouble>(pos.Longitude());
    aAltHaccVacc[0] = static_cast<jfloat>(pos.Altitude());
    aAltHaccVacc[1] = static_cast<jfloat>(pos.HorizontalAccuracy());
    aAltHaccVacc[2] = static_cast<jfloat>(pos.VerticalAccuracy());

    *aTimestamp = java::util::S60CommonUtils::TTimeToJavaTime(pos.Time());
}

// -----------------------------------------------------------------------------
// CPositionerBase::GetSpeedAndCourse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPositionerBase::GetSpeedAndCourse(CPositionerBase* aSelf,
                                        jfloat* aSpeedCourse)
{
    JELOG2(EJavaLocation);
    HPositionGenericInfo* posInfo = aSelf->iPositionInfo;

    if (posInfo->IsFieldAvailable(EPositionFieldHorizontalSpeed)
            && posInfo->IsFieldAvailable(EPositionFieldHeading))
    {
        TReal32 speed;
        TReal32 course;
        posInfo->GetValue(EPositionFieldHorizontalSpeed, speed);
        posInfo->GetValue(EPositionFieldHeading, course);

        aSpeedCourse[0] = static_cast<jfloat>(speed);
        aSpeedCourse[1] = static_cast<jfloat>(course);
        return KErrNone;
    }

    return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CPositionerBase::GetAddressInfo
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPositionerBase::GetAddressInfo(CPositionerBase* aSelf, JNIEnv* aJni,
                                     jobjectArray aAddress)
{
    JELOG2(EJavaLocation);
    HPositionGenericInfo* posInfo = aSelf->iPositionInfo;
    TInt availableFields(0);
    TPtrC ptr;

    for (TInt i = 0; i < (KNumFields - 2); ++i)
    {
        if (posInfo->GetValue(positionField[i], ptr) == KErrNone)
        {
            jstring jstr = java::util::S60CommonUtils::NativeToJavaString(
                               *aJni, ptr);
            aJni->SetObjectArrayElement(aAddress, i, jstr);
            aJni->DeleteLocalRef(jstr);
            availableFields |= 1 << i;
        }
    }

    TPtrC8 mediaLinkPtr;
    if (posInfo->GetValue(EPositionFieldMediaLinksStart, mediaLinkPtr)
            == KErrNone)
    {
        TInt numSlashFound(0);
        TInt length = mediaLinkPtr.Length();
        const TUint8* p = mediaLinkPtr.Ptr();
        while (numSlashFound < 2 && length--) // Find second slash
        {
            if (*p++ == '/')
            {
                ++numSlashFound;
            }
        }

        if (length > 0)
        {
            HBufC* mediaLink = HBufC::New(length);
            if (mediaLink)
            {
                mediaLink->Des().Copy(mediaLinkPtr.Right(length)); // Copy URL
                jstring jstr = java::util::S60CommonUtils::NativeToJavaString(
                                   *aJni, *mediaLink);
                aJni->SetObjectArrayElement(aAddress, KNumFields - 2, jstr);
                aJni->DeleteLocalRef(jstr);
                availableFields |= 1 << (KNumFields - 2);
                delete mediaLink;
            }
        }
    }

    if (posInfo->GetValue(EPositionFieldBuildingTelephone, ptr) == KErrNone)
    {
        jstring jstr = java::util::S60CommonUtils::NativeToJavaString(*aJni,
                       ptr);
        aJni->SetObjectArrayElement(aAddress, KNumFields - 1, jstr);
        aJni->DeleteLocalRef(jstr);
        availableFields |= 1 << (KNumFields - 1);
    }

    return availableFields;
}

// -----------------------------------------------------------------------------
// CPositionerBase::UpdatePosition
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositionerBase::UpdatePosition()
{
    JELOG2(EJavaLocation);

    iPositioner.NotifyPositionUpdate(*iPositionInfo, iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CPositionerBase::IncreaseBufferSizeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositionerBase::HandleBufferSizeErrorL(const TInt aError)
{
    JELOG2(EJavaLocation);

    // Increase the size of the buffer by default value. This gets called
    // repeatedly if the buffer is not big enough for example if the positioning
    // module is able to handle multiple NMEA sentences as BT GPS PSY does
    TInt bufferSize = iPositionInfo->BufferSize();
    TInt numFields = iPositionInfo->MaxFields();

    // Check that what was the overflow situation
    switch (aError)
    {
    case KErrOverflow:
    {
        // There are not enough supported fields.
        numFields += KLocFieldCountIncrease;
        break;
    }
    case KErrPositionBufferOverflow:
    {
        // The buffer size was not enough as the positioner provided
        // more information than expected. Try to increase buffer size
        bufferSize += KLocBufferSizeIncrease;
        break;
    }
    default:
    {
        // Unkown buffer size error. Should newer get here because MLFW
        // does not return any other buffer overflow situations and
        // base classes should take care about other situations
        __ASSERT_DEBUG(EFalse, User::Invariant());
        break;
    }
    }

    // Create new buffer. Capabilities needs to be the same as in the original
    HPositionGenericInfo* newInfo = CreatePositionInfoL(bufferSize, numFields);
    delete iPositionInfo;
    iPositionInfo = newInfo;
}

// -----------------------------------------------------------------------------
// CPositionerBase::CreatePositionInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HPositionGenericInfo* CPositionerBase::CreatePositionInfoL(
    const TInt aBufferSize, const TInt aNumFields) const
{
    JELOG2(EJavaLocation);
    HPositionGenericInfo* posInfo = HPositionGenericInfo::NewL(aBufferSize,
                                    aNumFields);

    // Request speed and direction if supported by the module
    if ((iCapabilities & TPositionModuleInfo::ECapabilitySpeed)
            && (iCapabilities & TPositionModuleInfo::ECapabilityDirection))
    {
        User::LeaveIfError(posInfo->SetRequestedField(
                               EPositionFieldHorizontalSpeed));
        User::LeaveIfError(posInfo->SetRequestedField(EPositionFieldHeading));
    }
    // Request NMEA statements if supported by the module
    if (iCapabilities & TPositionModuleInfo::ECapabilityNmea)
    {
        User::LeaveIfError(posInfo->SetRequestedField(
                               EPositionFieldNMEASentences));
    }
    if (iCapabilities & (TPositionModuleInfo::ECapabilityAddress
                         | TPositionModuleInfo::ECapabilityBuilding
                         | TPositionModuleInfo::ECapabilityMedia))
    {
        for (TInt i(0); i < KNumFields; i++)
        {
            User::LeaveIfError(posInfo->SetRequestedField(positionField[i]));
        }
    }
    return posInfo;
}

//  End of File
