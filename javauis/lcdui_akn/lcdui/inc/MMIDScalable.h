/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
*/


#ifndef MMIDSCALABLE_H
#define MMIDSCALABLE_H

// For TBool type
#include <e32def.h>

/**
 * Interface class for scalable components.
 *
 * @since S60 5.0
 */
class MMIDScalable
{
public:

    /**
     * Get information if current Displayble (like Canvas) is scaled.
      *
     * @return ETrue if scaling is on. EFalse otherwise.
     * @since S60 5.0
     */
    virtual TBool IsScalingOn() const = 0;
};

#endif // MMIDSCALEBLE_H

// End of File
