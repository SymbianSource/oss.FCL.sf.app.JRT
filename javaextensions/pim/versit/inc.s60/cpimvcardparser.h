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


#ifndef CPIMVCARDPARSER_H
#define CPIMVCARDPARSER_H

// EXTERNAL INCLUDES
#include <versit.h>

// CLASS DECLARATION

/**
 * Class deriving from CVersitParser to access its protected methods.
 * @since S60 3.1
 */
NONSHARABLE_CLASS(CPIMVCardParser) : public CVersitParser
{
public: // Constructor

    /**
     * Static two-phase constructor. Creates an instance
     * of this class
     */
    static CPIMVCardParser* NewLC();

    /**
     * Destructor
     */
    virtual ~CPIMVCardParser();

public: // Interface

    /**
     * MakePropertyValueCDesCArrayL
     * Creates an array of properties for a string buffer
     *
     * @param aStringValue Value from which the array is generated
     * @return Array of properties. Ownership is transferred to the caller
     */
    CDesCArray* MakePropertyValueCDesCArrayL(TPtr16 aStringValue);

private:

    /**
     * Default C++ constructor. Calls the constructor of the base class
     */
    CPIMVCardParser();
};

#endif // CPIMVCARDPARSER_H
// End of File
