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
* Description:  A base class for holding PIM field values
 *
*/


// CLASS HEADER
#include "cpimvaluebase.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "pimcommon.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMValueBase::~CPIMValueBase()
{
    JELOG2(EPim);
    delete iLabelArray;
    delete iInternalAttributes;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetIntValue(TInt /*aIntegerValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetBooleanValue(TBool /*aBooleanValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetDateValue(TPIMDate /*aDateValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetStringValue(const HBufC* /*aStringValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetStringArrayValue(const CDesCArray* /*aStringArrayValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetBinaryValue(const CPIMByteArray* /*aBinaryValue*/)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMValueBase::IntValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    return 0; // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMValueBase::BooleanValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    return EFalse; // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMDate CPIMValueBase::DateValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    return Time::NullTTime(); // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const HBufC* CPIMValueBase::StringValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    const HBufC* ret = NULL;
    return ret; // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CDesCArray* CPIMValueBase::StringArrayValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    CDesCArray* ret = NULL;
    return ret; // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CPIMByteArray* CPIMValueBase::BinaryValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    CPIMByteArray* ret = NULL;
    return ret; // For the compiler
}

// ---------------------------------------------------------------------------
// CPIMValueBase::NumElements
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMValueBase::NumElements() const
{
    JELOG2(EPim);
    return 1; // Default is one for other fields than string arrays
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetInternalAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetInternalAttributes(CArrayFix<TUid>* aInternalAttributes)
{
    JELOG2(EPim);
    delete iInternalAttributes;
    iInternalAttributes = aInternalAttributes;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetAttributes(TPIMAttribute aAttributes)
{
    JELOG2(EPim);
    iAttributes = aAttributes;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::InternalAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>& CPIMValueBase::InternalAttributes() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iInternalAttributes, User::Panic(KPIMPanicCategory,
                   EPIMPanicInvalidState));
    return *iInternalAttributes;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Attributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const TPIMAttribute& CPIMValueBase::Attributes() const
{
    JELOG2(EPim);
    return iAttributes;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::SetValueLabelsL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMValueBase::SetLabels(CDesCArray* aLabelArray)
{
    JELOG2(EPim);
    delete iLabelArray;
    iLabelArray = aLabelArray;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::Labels
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CDesCArray* CPIMValueBase::Labels() const
{
    JELOG2(EPim);
    return iLabelArray;
}

// ---------------------------------------------------------------------------
// CPIMValueBase::CPIMValueBase
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CPIMValueBase::CPIMValueBase(TPIMAttribute aAttributes) :
        iAttributes(aAttributes), iInternalAttributes(KPIMAttrNone)
{
    JELOG2(EPim);
}

// End of file
