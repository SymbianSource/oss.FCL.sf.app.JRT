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
* Description:  Valid to-do data info.
 *
*/


// INCLUDE FILES
#include  "cpimtodovalidator.h"
#include  "pimtodo.h"
#include "logger.h"

CPIMToDoValidator* CPIMToDoValidator::NewL()
{
    JELOG2(EPim);
    CPIMToDoValidator* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CPIMToDoValidator* CPIMToDoValidator::NewLC()
{
    JELOG2(EPim);
    CPIMToDoValidator* self = new(ELeave) CPIMToDoValidator();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CPIMToDoValidator::CPIMToDoValidator() :
        CPIMValidator(KPIMAttrNone) // ToDo defines no attributes
{
    JELOG2(EPim);
}

void CPIMToDoValidator::ConstructL()
{
    JELOG2(EPim);
    CPIMValidator::ConstructL();

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoClass, EPIMFieldInt));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoCompleted, EPIMFieldBoolean));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoCompletionDate,
                                           EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoDue, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoNote, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoPriority, EPIMFieldInt));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoRevision, EPIMFieldDate));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoSummary, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoUid, EPIMFieldString));

    iValidFields->AppendL(TPIMFieldAndType(EPIMToDoExtAlarm, EPIMFieldInt));
}

CPIMToDoValidator::~CPIMToDoValidator()
{
    JELOG2(EPim);
}

TBool CPIMToDoValidator::IsValidIntegerValue(const TPIMField& aField,
        const TInt& aValue) const
{
    JELOG2(EPim);
    TBool retVal = ETrue;

    if (aField == EPIMToDoClass)
    {
        // to-do class has predefined values
        if ((aValue != EPIMToDoClassConfidential) && (aValue
                != EPIMToDoClassPrivate) && (aValue != EPIMToDoClassPublic))
        {
            // not one of the predefined values
            retVal = EFalse;
        }
    }
    else if (aField == EPIMToDoPriority)
    {
        if ((aValue < EPIMToDoPriorityMinValue) || (aValue
                > EPIMToDoPriorityMaxValue))
        {
            retVal = EFalse;
        }
    }
    // else any integer is OK

    return retVal;
}

//  End of File
