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
* Description:  Valid event data info.
 *
*/


// INCLUDE FILES
#include  "cpimeventvalidator.h"
#include  "pimevent.h"
#include "logger.h"

CPIMEventValidator* CPIMEventValidator::NewL()
{
    JELOG2(EPim);
    CPIMEventValidator* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CPIMEventValidator* CPIMEventValidator::NewLC()
{
    JELOG2(EPim);
    CPIMEventValidator* self = new(ELeave) CPIMEventValidator();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CPIMEventValidator::CPIMEventValidator() :
        CPIMValidator(KPIMAttrNone) // Event defines no attributes
{
    JELOG2(EPim);
}

void CPIMEventValidator::ConstructL()
{
    JELOG2(EPim);
    CPIMValidator::ConstructL();

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventAlarm, EPIMFieldInt));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventClass, EPIMFieldInt));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventEnd, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventLocation, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventNote, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventRevision, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventStart, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventSummary, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMEventUid, EPIMFieldString));
}

CPIMEventValidator::~CPIMEventValidator()
{
    JELOG2(EPim);
}

TBool CPIMEventValidator::IsValidIntegerValue(const TPIMField& aField,
        const TInt& aValue) const
{
    JELOG2(EPim);
    TBool retVal = ETrue;

    if (aField == EPIMEventClass)
    {
        // contact class has predefined values
        if ((aValue != EPIMEventClassConfidential) && (aValue
                != EPIMEventClassPrivate) && (aValue != EPIMEventClassPublic))
        {
            // not one of predefined values
            retVal = EFalse;
        }
    }
    // else any integer is OK

    return retVal;
}

//  End of File
