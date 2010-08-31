/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for storing and parsing properties
*
*/


//  INCLUDE FILES
#include <logger.h>
#include "cmmaplayerproperties.h"

const TInt KPropertyNameEndChar = '=';
const TInt KPropertyEndChar = '&';
const TInt KDefaultGranularity = 8;

CMMAPlayerProperties* CMMAPlayerProperties::NewL(const TDesC& aProperties,
        MMMAParameterRule& aRule)
{
    CMMAPlayerProperties* props = new(ELeave) CMMAPlayerProperties(aProperties, aRule);
    return props;
}

CMMAPlayerProperties::~CMMAPlayerProperties()
{
}


CMMAPlayerProperties::CMMAPlayerProperties(const TDesC& aProperties, MMMAParameterRule& aRule) :
        iProperties(aProperties),
        iRule(aRule)
{
}

TBool CMMAPlayerProperties::GetProperty(const TDesC& aKey,
                                        TInt& aValue) const
{
    return iRule.FindProperty(aKey, aValue);
}

TBool CMMAPlayerProperties::GetProperty(const TDesC& aKey, TPtrC& aValue) const
{
    return iRule.FindProperty(aKey, aValue);
}

TBool CMMAPlayerProperties::Compare(const TDesC& aKey, const TDesC& aValue) const
{
    return iRule.Compare(aKey, aValue);
}

TBool CMMAPlayerProperties::Compare(const TDesC& aKey, const TInt& aValue) const
{
    return iRule.Compare(aKey, aValue);
}

// Parse property values: "key=value&key2=value2&..."
void CMMAPlayerProperties::ValidateL() const
{
    // Empty properties are valid
    if (iProperties.Length() == 0)
    {
        return;
    }
    TInt pos = 0;
    while (pos != KErrNotFound)
    {
        // Getting everything from 'pos' to end and finding '&'
        TPtrC property = iProperties.Mid(pos);
        TInt valueEndPos = property.Locate(KPropertyEndChar);

        if (valueEndPos != KErrNotFound)
        {
            // This is not last property, clipping remaining chars
            property.Set(iProperties.Mid(pos, valueEndPos));
        }

        TInt keyEndPos = property.Locate(KPropertyNameEndChar);
        // if we are getting KErrNotFound then properties is not valid
        if (keyEndPos == KErrNotFound)
        {
            User::Leave(KErrArgument);
        }

        // "key=value"
        TPtrC propertyKey(property.Left(keyEndPos));
        TPtrC propertyValue(property.Mid(keyEndPos + 1));

        // check is the pair valid, leave if not
        // check also that this key was expected, leave if not
        TPtrC tmp;
        TInt tmpInt;
        if (!iRule.ValidateAndStoreL(propertyKey, propertyValue) ||
                !(iRule.FindProperty(propertyKey, tmp) ||
                  iRule.FindProperty(propertyKey, tmpInt)))
        {
            User::Leave(KErrArgument);
        }

        // prepare for next token or mark end
        if (valueEndPos != KErrNotFound)
        {
            pos += valueEndPos + 1;
        }
        else
        {
            pos = valueEndPos;
        }
    }
}


CMMAParameterRuleSet* CMMAParameterRuleSet::NewLC()
{
    CMMAParameterRuleSet* self = new(ELeave)CMMAParameterRuleSet();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMMAParameterRuleSet::~CMMAParameterRuleSet()
{
    if (iRules)
    {
        iRules->Reset();
        delete iRules;
    }
}

CMMAParameterRuleSet::CMMAParameterRuleSet()
{
}

void CMMAParameterRuleSet::ConstructL()
{
    iRules = new(ELeave)CArrayPtrSeg< MMMAParameterRule >(KDefaultGranularity);
}

#define LOOPUNTILRULE( endRule ) \
    TInt rules = iRules->Count(); \
    TInt i( 0 ); \
    while( ( i < rules ) && \
           endRule ) \
        { \
        i++; \
        }

TBool CMMAParameterRuleSet::ValidateAndStoreL(const TDesC& aKey, const TDesC& aValue)
{
    LOOPUNTILRULE(iRules->At(i)->ValidateAndStoreL(aKey, aValue));
    // if loop has ended before end then some validation has failed.
    return (i == rules);
}

TBool CMMAParameterRuleSet::Compare(const TDesC& aKey, const TDesC& aValue)
{
    LOOPUNTILRULE(!iRules->At(i)->Compare(aKey, aValue));
    // if loop has ended before end then key-value pair has been found
    return (i != rules);
}

TBool CMMAParameterRuleSet::Compare(const TDesC& aKey, const TInt aValue)
{
    LOOPUNTILRULE(!iRules->At(i)->Compare(aKey, aValue));
    // if loop has ended before end then key-value pair has been found
    return (i != rules);
}

TBool CMMAParameterRuleSet::FindProperty(const TDesC& aKey, TPtrC& aValue)
{
    LOOPUNTILRULE(!iRules->At(i)->FindProperty(aKey, aValue));
    // if loop has ended before end then key has been found
    return (i != rules);
}

TBool CMMAParameterRuleSet::FindProperty(const TDesC& aKey, TInt& aValue)
{
    LOOPUNTILRULE(!iRules->At(i)->FindProperty(aKey, aValue));
    // if loop has ended before end then key has been found
    return (i != rules);
}

void CMMAParameterRuleSet::AppendRuleL(MMMAParameterRule* aRule)
{
    iRules->AppendL(aRule);
}

TMMAParameterRuleBase::TMMAParameterRuleBase(const TDesC& aKey) :
        iKey(aKey),
        iAssigned(EFalse)
{}

TBool TMMAParameterRuleBase::ValidateAndStoreL(const TDesC& aKey, const TDesC& aValue)
{
    if ((aKey.Length() == 0) ||
            (aValue.Length() == 0))
    {
        // key or value length is zero, fail
        User::Leave(KErrArgument);
    }

    TBool isValid = ETrue;
    // this is same key as in rule
    if (iKey.Compare(aKey) == 0)
    {
        //if this key is already assigned then there is same key more than once
        if (iAssigned)
        {
            User::Leave(KErrArgument);
        }
        isValid = ValidateValueL(aValue);

        // if it was valid we mark this key as assigned
        if (isValid)
        {
            iAssigned = ETrue;
        }
    }
    return isValid;
}

TBool TMMAParameterRuleBase::Compare(const TDesC& /*aKey*/, const TDesC& /*aValue*/)
{
    // by default we do not found this key
    return EFalse;
}

TBool TMMAParameterRuleBase::Compare(const TDesC& /*aKey*/, const TInt /*aValue*/)
{
    // by default we do not found this key
    return EFalse;
}

TBool TMMAParameterRuleBase::FindProperty(const TDesC& /*aKey*/, TPtrC& /*aValue*/)
{
    // by default we do not found this key
    return EFalse;
}

TBool TMMAParameterRuleBase::FindProperty(const TDesC& /*aKey*/, TInt& /*aValue*/)
{
    // by default we do not found this key
    return EFalse;
}

TMMAParameterRuleInt::TMMAParameterRuleInt(const TDesC& aKey) :
        TMMAParameterRuleBase(aKey),
        iUpperLimit(KMaxTInt),
        iLowerLimit(KMinTInt)
{}

TMMAParameterRuleInt::TMMAParameterRuleInt(const TDesC& aKey,
        const TInt aLowerLimit) :
        TMMAParameterRuleBase(aKey),
        iUpperLimit(KMaxTInt),
        iLowerLimit(aLowerLimit)
{}

TMMAParameterRuleInt::TMMAParameterRuleInt(const TDesC& aKey,
        const TInt aLowerLimit,
        const TInt aUpperLimit) :
        TMMAParameterRuleBase(aKey),
        iUpperLimit(aUpperLimit),
        iLowerLimit(aLowerLimit)
{}

TBool TMMAParameterRuleInt::ValidateValueL(const TDesC& aValue)
{
    TLex lex(aValue);
    TInt valueInt = 0;
    if ((lex.Val(valueInt) != KErrNone) ||
            !lex.Eos())
    {
        User::Leave(KErrArgument);
    }
    if ((valueInt < iLowerLimit) ||
            (valueInt > iUpperLimit))
    {
        // value is not valid
        return EFalse;
    }
    else
    {
        // value is valid, store it
        iValue = valueInt;
        return ETrue;
    }
}

TBool TMMAParameterRuleInt::Compare(const TDesC& aKey, const TInt aValue)
{
    TBool match = EFalse;
    if (iAssigned &&
            (iKey.Compare(aKey) == 0) &&
            (aValue == iValue))
    {
        match = ETrue;
    }
    return match;
}

TBool TMMAParameterRuleInt::FindProperty(const TDesC& aKey, TInt& aValue)
{
    TBool match = EFalse;
    if (iAssigned &&
            iKey.Compare(aKey) == 0)
    {
        aValue = iValue;
        match = ETrue;
    }
    return match;
}

TMMAParameterRuleDes::TMMAParameterRuleDes(const TDesC& aKey) :
        TMMAParameterRuleBase(aKey),
        iValidValues(NULL),
        iArraySize(0)
{}

TMMAParameterRuleDes::TMMAParameterRuleDes(const TDesC& aKey,
        const TMMAStaticStrArray* aValidValues,
        const TInt aArraySize) :
        TMMAParameterRuleBase(aKey),
        iValidValues(aValidValues),
        iArraySize(aArraySize)
{}

TBool TMMAParameterRuleDes::ValidateValueL(const TDesC& aValue)
{
    TInt i = 0;
    while ((i < iArraySize) &&
            aValue.Compare(iValidValues[i]()) != 0)
    {
        i++;
    }
    // if there is not valid values then we treat every value as valid
    if (iValidValues &&
            (i == iArraySize))
    {
        // there was no hit
        return EFalse;
    }

    // find match or every value is valid, storing
    iValue.Set(aValue);
    return ETrue;
}

TBool TMMAParameterRuleDes::Compare(const TDesC& aKey, const TDesC& aValue)
{
    TBool match = EFalse;
    if (iAssigned &&
            (iKey.Compare(aKey) == 0) &&
            (iValue.Compare(aValue) == 0))
    {
        match = ETrue;
    }
    return match;
}

TBool TMMAParameterRuleDes::FindProperty(const TDesC& aKey, TPtrC& aValue)
{
    TBool match = EFalse;
    if (iAssigned &&
            iKey.Compare(aKey) == 0)
    {
        aValue.Set(iValue);
        match = ETrue;
    }
    return match;
}
//  END OF FILE
