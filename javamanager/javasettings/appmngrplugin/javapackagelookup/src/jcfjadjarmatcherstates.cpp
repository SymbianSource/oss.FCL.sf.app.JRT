/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Matches JAD and JAR attributes.
*
*/


#include "jcfjadjarmatcherstates.h"
#include "logger.h"

// ---------------------------------------------------------------------------
//    CJcfJadJarMatcherState
// -----------------------------------------------------------------------------
CJcfJadJarMatcherState::CJcfJadJarMatcherState(
    MJcfJadJarMatcherObserver*  aObs) :
        CActive(EPriorityStandard)
{
    CActiveScheduler::Add(this);
    AddObserver(aObs);
    iState = EBegin;
}

// ---------------------------------------------------------------------------
//    ~CJcfJadJarMatcherState
// -----------------------------------------------------------------------------
CJcfJadJarMatcherState::~CJcfJadJarMatcherState()
{
}

// ---------------------------------------------------------------------------
//    To check whether JAD and JAR files match or not. Operation returns true
//    if following statements are all true:
//      1) The version of JAD is same as the version of JAR.
//      2) The vendor of JAD is same as the vendor of JAR.
//      3) The name of JAD is same as the name of JAR.
//      4) Suite contains at least one MIDlet.
// -----------------------------------------------------------------------------
TBool CJcfJadJarMatcherState::Match(
    const RPointerArray<Java::MJavaAttribute>& aJadAttributes,
    const RPointerArray<Java::MJavaAttribute>& aJarAttributes)
{
    JELOG2(EJavaStorage);
    TBool result = EFalse;

    _LIT(KMIDletName, "MIDlet-Name");
    _LIT(KMIDletVendor, "MIDlet-Vendor");
    _LIT(KMIDletVersion, "MIDlet-Version");
    _LIT(KFirstMIDlet, "MIDlet-1");

    if (AttributesEqual(aJadAttributes, aJarAttributes, KMIDletName)
            && AttributesEqual(aJadAttributes, aJarAttributes, KMIDletVendor)
            && AttributesEqual(aJadAttributes, aJarAttributes, KMIDletVersion))
    {
        // Check at least one MIDlet exists. Package has installable content
        // and can be shown to user.
        if (KErrNotFound != AttributeIndex(aJadAttributes, KFirstMIDlet)
                && KErrNotFound != AttributeIndex(aJarAttributes, KFirstMIDlet))
        {
            result = ETrue;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
//    RunError
// -----------------------------------------------------------------------------
TInt CJcfJadJarMatcherState::RunError(TInt aError)
{
    ELOG1(EJavaStorage, "CJcfJadJarMatcherState::RunError: %d", aError);
    NotifyObserver(aError);
    Cancel();
    Cleanup();
    return KErrNone;
}

// ---------------------------------------------------------------------------
// AttributesEqual
// ---------------------------------------------------------------------------
TBool CJcfJadJarMatcherState::AttributesEqual(
    const RPointerArray<Java::MJavaAttribute>& aJadAttributes,
    const RPointerArray<Java::MJavaAttribute>& aJarAttributes,
    const TDesC& aName) const
{
    TBool result = ETrue;

    TInt jadIndex = AttributeIndex(aJadAttributes, aName);
    TInt jarIndex = AttributeIndex(aJarAttributes, aName);

    if (jadIndex > 0 && jarIndex > 0)
    {
        if (aJadAttributes[jadIndex]->Value()
                != aJarAttributes[jarIndex]->Value())
        {
            result = EFalse;
        }
    }
    else
    {
        result = EFalse;
    }
    return result;
}

// ---------------------------------------------------------------------------
// AttributeIndex
// ---------------------------------------------------------------------------
TInt CJcfJadJarMatcherState::AttributeIndex(
    const RPointerArray<Java::MJavaAttribute>& aAttributes,
    const TDesC& aName) const
{
    TInt i = 0;
    while (i < aAttributes.Count())
    {
        if (aAttributes[i]->Name() == aName)
        {
            return i;
        }
        ++i;
    }
    return KErrNotFound;
}

