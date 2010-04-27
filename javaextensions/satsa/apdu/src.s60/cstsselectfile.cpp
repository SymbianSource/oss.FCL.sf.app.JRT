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


// INCLUDE FILES
#include  "cstsselectfile.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSINSSelectFile = 0xA4;
const TUint8 KSTSP1SelectAID = 0x04;

// Activation, Return FCP tmpl, First only occurence
const TUint8 KSTSP2SelectAIDActivation = 0x00;
// Termination, Return FCP tmpl, First only occurence
const TUint8 KSTSP2SelectAIDTermination = 0x44;

const TUint8 KSTSP1SelectByFileId = 0x00;
const TUint8 KSTSP1SelectByPathFromMF = 0x08;
const TUint8 KSTSP1SelectByPathFromCurrentDF = 0x09;

const TUint8 KSTSP2ReturnFCPTemplate = 0x04;

// ============================ MEMBER FUNCTIONS ===============================


CSTSSelectFile::CSTSSelectFile()
{

}

void CSTSSelectFile::ConstructL(const TDesC8& aData,
                                TSTSSelectFileApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{

    TUint8 p1;
    TUint8 p2;

    GenerateP1P2(p1, p2, aType);

    TInt aidLength = aData.Length();
    // Lc length=1 byte
    TInt maxLength = KSTSApduMandatoryHeaderLen + 1 + aidLength;

    iApduData = CSTSApdu::NewL(maxLength, aStandard);

    // fills header and Lc with zeros
    iApduData->ApduPtr().FillZ(KSTSApduMandatoryHeaderLen);

    SetHeader(KSTSCLABasicUMTS, KSTSINSSelectFile, p1, p2);

    AppendDatabytesL(aData);

}

CSTSSelectFile* CSTSSelectFile::NewLC(const TDesC8& aData,
                                      TSTSSelectFileApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSSelectFile* self = new(ELeave) CSTSSelectFile();
    CleanupStack::PushL(self);
    self->ConstructL(aData, aType, aStandard);
    return self;
}

CSTSSelectFile* CSTSSelectFile::NewL(const TDesC8& aData,
                                     TSTSSelectFileApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSSelectFile* self = CSTSSelectFile::NewLC(aData, aType, aStandard);
    CleanupStack::Pop(self);

    return self;
}

CSTSSelectFile::~CSTSSelectFile()
{
}

void CSTSSelectFile::SetApduType(TSTSSelectFileApduType aType)
{
    iType = aType;

    TUint8 p1;
    TUint8 p2;
    GenerateP1P2(p1, p2, aType);
    SetApduByte(p1, ESTSP1);
    SetApduByte(p2, ESTSP2);
}

void CSTSSelectFile::GenerateP1P2(TUint8& aP1, TUint8& aP2,
                                  TSTSSelectFileApduType aType)
{

    aP1 = KSTSP1SelectAID;
    aP2 = KSTSP2ReturnFCPTemplate;

    switch (aType)
    {
    case ESTSAIDActivation:
    {
        aP2 = KSTSP2SelectAIDActivation;
        break;
    }
    case ESTSAIDTermination:
    {
        aP2 = KSTSP2SelectAIDTermination;
        break;
    }
    case ESTSSelectByFileId:
    {
        aP1 = KSTSP1SelectByFileId;
        break;
    }
    case ESTSSelectByPathFromMF:
    {
        aP1 = KSTSP1SelectByPathFromMF;
        break;
    }
    case ESTSSelectByPathFromCurrentDF:
    {
        aP1 = KSTSP1SelectByPathFromCurrentDF;
        break;
    }
    default:
    {
        break;
    }
    }
}

} // namespace satsa
} // namespace java
//  End of File
