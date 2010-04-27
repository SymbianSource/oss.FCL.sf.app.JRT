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
* Description:  Helper class to access CVersitParser protected methods
 *
*/


// CLASS HEADER
#include "cpimvcardparser.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMVCardParser::NewL
// ---------------------------------------------------------------------------
//
CPIMVCardParser* CPIMVCardParser::NewLC()
{
    JELOG2(EPim);
    CPIMVCardParser* self = new(ELeave) CPIMVCardParser();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CPIMVCardParser::~CPIMVCardParser
// Destructor
// ---------------------------------------------------------------------------
//
CPIMVCardParser::~CPIMVCardParser()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMVCardParser::MakePropertyValueCDesCArrayL
// ---------------------------------------------------------------------------
//
CDesCArray* CPIMVCardParser::MakePropertyValueCDesCArrayL(TPtr16 aStringValue)
{
    JELOG2(EPim);
    return CVersitParser::MakePropertyValueCDesCArrayL(aStringValue);
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPIMVCardParser::CPIMVCardParser() :
        CVersitParser(ESupportsVersion)
{
    JELOG2(EPim);
}

// End of file
