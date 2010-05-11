/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions for Tuner component.
*
*/


TTunerCapabilities::TTunerCapabilities()
        : iTunerBands(0), iAdditionalFunctions(0)
{}

TTunerCapabilities::TTunerCapabilities(TUint32 aTunerBands, TUint32 aAdditionalFunctions)
        : iTunerBands(aTunerBands), iAdditionalFunctions(aAdditionalFunctions)
{}

TFrequency::TFrequency()
        : iFrequency(0)
{}

TFrequency::TFrequency(TInt aFrequency)
        : iFrequency(aFrequency)
{}

TInt TFrequency::operator==(const TFrequency& aFrequency) const
{
    return (iFrequency==aFrequency.iFrequency);
}

TInt TFrequency::operator!=(const TFrequency& aFrequency) const
{
    return (iFrequency!=aFrequency.iFrequency);
}

TInt TFrequency::operator> (const TFrequency& aFrequency) const
{
    return (iFrequency>aFrequency.iFrequency);
}

TInt TFrequency::operator>=(const TFrequency& aFrequency) const
{
    return (iFrequency>=aFrequency.iFrequency);
}

TInt TFrequency::operator< (const TFrequency& aFrequency) const
{
    return (iFrequency<aFrequency.iFrequency);
}

TInt TFrequency::operator<=(const TFrequency& aFrequency) const
{
    return (iFrequency<=aFrequency.iFrequency);
}

TRdsData::TRdsData()
{
}

TEonMappedFrequency::TEonMappedFrequency(TFrequency aTuningFrequency, TFrequency aMappedFrequency)
        : iTuningFrequency(aTuningFrequency), iMappedFrequency(aMappedFrequency)
{
}

TInt TRdsProgrammeItemNumber::operator==(const TRdsProgrammeItemNumber& aPin) const
{
    return ((iDayOfMonth==aPin.iDayOfMonth)&&(iHour==aPin.iHour)&&(iMinute==aPin.iMinute));
}

TInt TRdsProgrammeItemNumber::operator!=(const TRdsProgrammeItemNumber& aPin) const
{
    return (!((*this)==aPin));
}

// End of file
