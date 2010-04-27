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


#ifndef STSAPDUCONSTANTS_H
#define STSAPDUCONSTANTS_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// CONSTANTS

// **** LEAVE CODES
// Leave codes maps to corresponding java expection.
// KSTSErr<java expection name without Expection word>
// Refer to JSR177 for correct meaning for expections.
// In java side these leave codes are defined in APDUError.java file.

// Base value for leaves to distinguish them from native error codes.
const TInt KSTSErrBase = -3000;

const TInt KSTSErrIllegalArgument = KSTSErrBase - 100;//incorrect uri
const TInt KSTSErrInterruptedIO = KSTSErrBase - 200;
//slot doesn't exist, card not inserted or powered on, application selection fails
const TInt KSTSErrConnectionNotFound = KSTSErrBase - 300;
//j2me application not allowed to access to selected application
const TInt KSTSErrSecurity = KSTSErrBase - 400;
const TInt KSTSErrIO = KSTSErrBase - 500;
const TInt KSTSErrCancel = KSTSErrBase - 600;

//illegal argument related detailed errors
//next is needed only in java side
const TInt KSTSErrIASelectApp = -12;
const TInt KSTSErrIAManageChannel = -13;
const TInt KSTSErrIAOtherCLA = -14;
const TInt KSTSErrIAMalformedAPDU = -15;
const TInt KSTSErrIAInvalidParam = -16;
const TInt KSTSErrIANotEnvelope = -17;

//Interrupted IO related detailed errors
const TInt KSTSErrIIConnClosedDuring = -21;
const TInt KSTSErrIICardRemoved = -22;
const TInt KSTSErrIIBTSapInUse = -23;

//Connection not found related detailed errors
const TInt KSTSErrCNTNoSlot = -31;
const TInt KSTSErrCNTNoCard = -32;
const TInt KSTSErrCNTNoApplication = -33;
const TInt KSTSErrCNTBtSapInUse = -34;

//Security related detailed errors
const TInt KSTSErrSECNoRightsPin = -41;
const TInt KSTSErrSECIllegalApdu = -42;
const TInt KSTSErrSECAccessNotAllowed = -43;
const TInt KSTSErrSECIllegalFileID = -44;

//IO related detailed errors
const TInt KSTSErrIOConnClosedBefore = -51;
const TInt KSTSErrIOCommProblems = -52;
const TInt KSTSErrIOErrorIO = -53;
const TInt KSTSErrIONoFreeChannels = -54;

//other constants

const TUint8 KMaxReaderAmount = 8;
const TInt KSTSApduMandatoryHeaderLen = 4;
//data part + sw1 and sw2
const TInt KSTSResponseApduMaxLength = 0xFF + 2;

const TInt KSTSLogicalChannelsMax = 3;

// DATA TYPES
enum TSTSAIDMode
{
    ESTSAIDActivation, ESTSAIDTermination
};

enum TSTSNetworkType
{
    ESTSTypeGSM, ESTSTypeUMTS
};

//must be values 0-4, because used also in array index
enum TPinApduType
{
    EVerifyPinAPDU = 0,
    EChangePinAPDU = 1,
    EDisablePinAPDU = 2,
    EEnablePinAPDU = 3,
    EUnblockPinAPDU = 4,
};

} // namespace satsa
} // namespace java
#endif // STSAPDUCONSTANTS_H
// End of File

