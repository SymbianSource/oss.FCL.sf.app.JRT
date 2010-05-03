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
* Description:
 *
*/


#ifndef CSTSSLOTINFO_H
#define CSTSSLOTINFO_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION

/**
 *  Maintain information about slot.
 *  Information consists of slot number, type(coldswappable, hotswappable)
 *  and rawdata (contains all this information and more in one byte)
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSSlotInfo): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSSlotInfo* NewL(const TUint8& aRawData);

    /**
     * Destructor.
     */
    virtual ~CSTSSlotInfo();

public: // New functions

    /**
     * Getter for slot numger
     * @since 3.0
     * @return Slot number in integer format
     */
    TInt Slot();

    /**
     * Getter for slot type
     * @since 3.0
     * @return Type in char format (C=Cold swappable, H=Hot swappable)
     */
    TChar Type();

private:

    /**
     * C++ default constructor.
     */
    CSTSSlotInfo();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TUint8& aRawData);

private: // Data

    TUint8 iRawData;
    TInt iSlotNumber;
    TBool iRemovable;
    TBool iCardPresent;

};

} // namespace satsa
} // namespace java
#endif // CSTSSLOTINFO_H
// End of File
