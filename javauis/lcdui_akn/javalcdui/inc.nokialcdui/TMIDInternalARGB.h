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
* Description:  Class contains one ARGB color value.
*
*/


#ifndef TMIDINTERNALARGB_H
#define TMIDINTERNALARGB_H

//  INCLUDES
#include <e32std.h>


//  CLASS DEFINITION
/**
 * Class contains one ARGB color value.
 *
 */

NONSHARABLE_CLASS(TMIDInternalARGB)
{
public: // constructors
    inline TMIDInternalARGB()
            : iA(0), iR(0), iG(0), iB(0) { };
    inline TMIDInternalARGB(TUint8 aA, TUint8 aR, TUint8 aG, TUint8 aB)
            : iA(aA), iR(aR), iG(aG), iB(aB) { };
public: // data
    TUint8 iA;
    TUint8 iR;
    TUint8 iG;
    TUint8 iB;
};

#endif // TMIDINTERNALARGB_H
