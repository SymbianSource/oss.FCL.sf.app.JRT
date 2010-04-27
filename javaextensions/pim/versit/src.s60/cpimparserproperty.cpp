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
* Description:  Provides access to versit parser propertys iArrayOfParams*
*/


// CLASS HEADER
#include "cpimparserproperty.h"

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "pimcontact.h"
#include "pimevent.h"
#include "pimtodo.h"
#include "pimversit.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <cntdef.h>

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CPIMParserProperty::ArrayOfParams
// ---------------------------------------------------------------------------
//
const CArrayPtr<CParserParam>* CPIMParserProperty::ArrayOfParams() const
{
    JELOG2(EPim);
    return iArrayOfParams;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::MatchAllAttributes
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMParserProperty::MatchAllAttributes() const
{
    JELOG2(EPim);
    TPIMAttribute attributes = KPIMAttrNone;
    if (!iArrayOfParams)
    {
        // No attributes
        return attributes;
    }
    // Iterate through all attributes
    TInt paramCount = iArrayOfParams->Count();
    for (TInt i = 0; i < paramCount; i++)
    {
        const CParserParam& param = *(iArrayOfParams->At(i));
        // The value may be stored as a name or a value in the parser
        if (DoCompareNameAndValue(param, KVersitParam8Pref))
        {
            attributes |= EPIMContactAttrPreferred;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Pager))
        {
            attributes |= EPIMContactAttrPager;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Cell))
        {
            attributes |= EPIMContactAttrMobile;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Fax))
        {
            attributes |= EPIMContactAttrFax;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Home))
        {
            attributes |= EPIMContactAttrHome;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Work))
        {
            attributes |= EPIMContactAttrWork;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Car))
        {
            attributes |= EPIMContactAttrAuto;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Video))
        {
            attributes |= EPIMContactAttrExtVideoCall;
        }
        if (DoCompareNameAndValue(param, KPIMVersitTokenASSTTEL))
        {
            attributes |= EPIMContactAttrAsst;
        }
    }

    return attributes;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::MatchHomeAndWorkAttributes
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMParserProperty::MatchHomeAndWorkAttributes() const
{
    JELOG2(EPim);
    TPIMAttribute attributes = KPIMAttrNone;
    if (!iArrayOfParams)
    {
        // No attributes
        return attributes;
    }
    // Iterate through all attributes
    TInt paramCount = iArrayOfParams->Count();
    for (TInt i = 0; i < paramCount; i++)
    {
        const CParserParam& param = *(iArrayOfParams->At(i));
        // The value may be stored as a name or a value in the parser
        if (DoCompareNameAndValue(param, KVersitParam8Home))
        {
            attributes |= EPIMContactAttrHome;
        }
        if (DoCompareNameAndValue(param, KVersitParam8Work))
        {
            attributes |= EPIMContactAttrWork;
        }
    }

    return attributes;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::ContainsParameter
// ---------------------------------------------------------------------------
//
TBool CPIMParserProperty::ContainsParameter(const TDesC8& aParam) const
{
    JELOG2(EPim);
    if (!iArrayOfParams)
    {
        return EFalse;
    }
    // Iterate through all attributes
    TInt paramCount = iArrayOfParams->Count();
    for (TInt i = 0; i < paramCount; i++)
    {
        const CParserParam& param = *(iArrayOfParams->At(i));
        // The value may be stored as a name or a value in the parser
        if (DoCompareNameAndValue(param, aParam))
        {
            return ETrue;
        }
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::MatchContactField
// ---------------------------------------------------------------------------
//
TPIMField CPIMParserProperty::MatchContactField() const
{
    JELOG2(EPim);
    TPIMField field = KErrNotFound;
    TPtrC8 name = Name();
    if (name.CompareF(KVersitTokenADR) == 0)
    {
        field = EPIMContactAddr;
    }
    else if (name.CompareF(KVersitTokenN) == 0)
    {
        field = EPIMContactName;
    }
    else if (name.CompareF(KVersitTokenORG) == 0)
    {
        field = EPIMContactOrg;
    }
    else if (name.CompareF(KVersitTokenChildren) == 0)
    {
        field = EPIMContactExtChildren;
    }
    else if (name.CompareF(KVersitTokenBDAY) == 0)
    {
        field = EPIMContactBirthday;
    }
    else if (name.CompareF(KVersitTokenREV) == 0)
    {
        field = EPIMContactRevision;
    }
    else if (name.CompareF(KVersitTokenAnniversary) == 0)
    {
        field = EPIMContactExtAnniversary;
    }
    else if (name.CompareF(KVersitTokenEMAIL) == 0)
    {
        field = EPIMContactEmail;
    }
    else if (name.CompareF(KVersitTokenNOTE) == 0)
    {
        field = EPIMContactNote;
    }
    else if (name.CompareF(KVersitTokenTEL) == 0 || name.CompareF(
                 KPIMVersitTokenASSTTEL) == 0)
    {
        field = EPIMContactTel;
    }
    else if (name.CompareF(KVersitTokenTITLE) == 0)
    {
        field = EPIMContactTitle;
    }
    else if (name.CompareF(KVersitTokenURL) == 0)
    {
        field = EPIMContactUrl;
    }
    else if (name.CompareF(KPIMVersitTokenNICKNAME) == 0)
    {
        field = EPIMContactNickname;
    }
    else if (name.CompareF(KPIMVersitTokenDTMF) == 0)
    {
        field = EPIMContactExtDtmf;
    }
    else if (name.CompareF(KPIMVersitTokenWVID) == 0)
    {
        field = EPIMContactExtWvUserId;
    }
    else if (name.CompareF(KVersitTokenAssistant) == 0)
    {
        field = EPIMContactExtAssistantName;
    }
    else if (name.CompareF(KVersitTokenChildren) == 0)
    {
        field = EPIMContactExtChildren;
    }
    else if (name.CompareF(KVersitTokenSpouse) == 0)
    {
        field = EPIMContactExtSpouse;
    }
    else if (name.CompareF(KPIMVersitTokenSIP) == 0)
    {
        // Check that if this is VOIP field
        if (ContainsParameter(KPIMVersitTokenVoip))
        {
            field = EPIMContactExtVoip;
        }
        // Check that if this is POC field
        else if (ContainsParameter(KPIMVersitTokenPoc))
        {
            field = EPIMContactExtPTT;
        }
        // Check that if this is SWIS field
        else if (ContainsParameter(KPIMVersitTokenSwis))
        {
            field = EPIMContactExtSWIS;
        }
        else
        {
            field = EPIMContactExtSip;
        }
    }
    else if (name.CompareF(KVersitTokenUID) == 0)
    {
        field = EPIMContactUid;
    }
    return field;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::MatchContactField
// ---------------------------------------------------------------------------
//
TPIMField CPIMParserProperty::MatchEventField() const
{
    JELOG2(EPim);
    TPIMField field = KErrNotFound;
    TPtrC8 name = Name();
    if (name.CompareF(KVersitTokenDTSTART) == 0)
    {
        field = EPIMEventStart;
    }
    else if (name.CompareF(KVersitTokenDTEND) == 0)
    {
        field = EPIMEventEnd;
    }
    else if (name.CompareF(KVersitTokenLASTMODIFIED) == 0)
    {
        field = EPIMEventRevision;
    }
    else if (name.CompareF(KVersitTokenSUMMARY) == 0)
    {
        field = EPIMEventSummary;
    }
    else if (name.CompareF(KVersitTokenDESCRIPTION) == 0)
    {
        field = EPIMEventNote;
    }
    else if (name.CompareF(KVersitTokenLOCATION) == 0)
    {
        field = EPIMEventLocation;
    }
    else if (name.CompareF(KVersitTokenDALARM) == 0)
    {
        field = EPIMEventAlarm;
    }
    else if (name.CompareF(KVersitTokenUID) == 0)
    {
        field = EPIMEventUid;
    }
    return field;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::MatchContactField
// ---------------------------------------------------------------------------
//
TPIMField CPIMParserProperty::MatchToDoField() const
{
    JELOG2(EPim);
    TPIMField field = KErrNotFound;
    TPtrC8 name = Name();

    if (name.CompareF(KVersitTokenDUE) == 0)
    {
        field = EPIMToDoDue;
    }
    else if (name.CompareF(KVersitTokenCOMPLETED) == 0)
    {
        field = EPIMToDoCompletionDate;
    }
    else if (name.CompareF(KVersitTokenLASTMODIFIED) == 0)
    {
        field = EPIMToDoRevision;
    }
    else if (name.CompareF(KVersitTokenSUMMARY) == 0)
    {
        field = EPIMToDoSummary;
    }
    else if (name.CompareF(KVersitTokenDESCRIPTION) == 0)
    {
        field = EPIMToDoNote;
    }
    else if (name.CompareF(KVersitTokenPRIORITY) == 0)
    {
        field = EPIMToDoPriority;
    }
    else if (name.CompareF(KVersitTokenDALARM) == 0)
    {
        field = EPIMToDoExtAlarm;
    }
    else if (name.CompareF(KVersitTokenSTATUS) == 0)
    {
        field = EPIMToDoCompleted;
    }
    else if (name.CompareF(KVersitTokenUID) == 0)
    {
        field = EPIMToDoUid;
    }
    return field;
}

// ---------------------------------------------------------------------------
// CPIMParserProperty::DoCompareNameAndValue
// ---------------------------------------------------------------------------
//
TBool CPIMParserProperty::DoCompareNameAndValue(const CParserParam& aParam,
        const TDesC8& aValue) const
{
    JELOG2(EPim);
    if ((aParam.Name().CompareF(aValue) == 0) || (aParam.Value().CompareF(
                aValue) == 0))
    {
        return ETrue;
    }

    return EFalse;
}

//  End of File
