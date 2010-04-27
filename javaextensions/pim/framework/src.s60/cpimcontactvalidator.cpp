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
* Description:  Valid contact data info.
 *
*/


// INCLUDE FILES
#include  "cpimcontactvalidator.h"
#include  "pimcontact.h"
#include "logger.h"

CPIMContactValidator* CPIMContactValidator::NewL()
{
    JELOG2(EPim);
    CPIMContactValidator* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CPIMContactValidator* CPIMContactValidator::NewLC()
{
    JELOG2(EPim);
    CPIMContactValidator* self = new(ELeave) CPIMContactValidator();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CPIMContactValidator::CPIMContactValidator() :
        CPIMValidator(EPIMContactAttrAsst | EPIMContactAttrAuto
                      | EPIMContactAttrFax | EPIMContactAttrHome | EPIMContactAttrMobile
                      | EPIMContactAttrOther | EPIMContactAttrPager
                      | EPIMContactAttrPreferred | EPIMContactAttrSms | EPIMContactAttrWork
                      | EPIMContactAttrExtVideoCall)
{
    JELOG2(EPim);
}

void CPIMContactValidator::ConstructL()
{
    JELOG2(EPim);
    CPIMValidator::ConstructL();

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactAddr,
                                           EPIMFieldStringArray));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactBirthday, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactClass, EPIMFieldInt));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactEmail, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactFormattedAddr,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactFormattedName,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactName,
                                           EPIMFieldStringArray));

    iValidFields->AppendL(
        TPIMFieldAndType(EPIMContactNickname, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactNote, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactOrg, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactPhoto, EPIMFieldBinary));

    iValidFields->AppendL(
        TPIMFieldAndType(EPIMContactPhotoUrl, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactPublicKey,
                                           EPIMFieldBinary));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactPublicKeyString,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactRevision, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactTel, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactTitle, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactUid, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactUrl, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtWvUserId,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtSip, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtDtmf, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtDepartment,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtAssistantName,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtSpouse,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtChildren,
                                           EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtAnniversary,
                                           EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtVoip, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtPTT, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMContactExtSWIS, EPIMFieldString));
}

CPIMContactValidator::~CPIMContactValidator()
{
    JELOG2(EPim);
}

TInt CPIMContactValidator::NumElementsL(const TPIMField& aStringArrayField) const
{
    JELOG2(EPim);
    TInt retVal = 0;

    if (aStringArrayField == EPIMContactAddr)
    {
        retVal = EPIMContactAddrNumElements;
    }
    else if (aStringArrayField == EPIMContactName)
    {
        retVal = EPIMContactNameNumElements;
    }
    else
    {
        User::Leave(KErrArgument);
    }

    return retVal;
}

TBool CPIMContactValidator::IsValidIntegerValue(const TPIMField& aField,
        const TInt& aValue) const
{
    JELOG2(EPim);
    TBool retVal = ETrue;

    if (aField == EPIMContactClass)
    {
        // contact class has predefined values
        if ((aValue != EPIMContactClassConfidential) && (aValue
                != EPIMContactClassPrivate) && (aValue != EPIMContactClassPublic))
        {
            // not one of predefined values
            retVal = EFalse;
        }
    }
    // else any integer is OK

    return retVal;
}

TBool CPIMContactValidator::IsValidStringArrayValue(const TPIMField& aField,
        const CDesCArray& aValue) const
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    if (aField == EPIMContactAddr)
    {
        retVal = (aValue.Count() == EPIMContactAddrNumElements);
    }
    else if (aField == EPIMContactName)
    {
        retVal = (aValue.Count() == EPIMContactNameNumElements);
    }
    // else leave retVal EFalse, because we don't recognize the
    // string array field

    return retVal;
}

//  End of File
