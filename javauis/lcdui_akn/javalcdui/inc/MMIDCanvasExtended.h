/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for observing Canvas visibility changes.
*
*/


#ifndef MMIDCANVASEXTENDED_H
#define MMIDCANVASEXTENDED_H

#include <lcdui.h>

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Interface for observing canvas' visibility changes

-----------------------------------------------------------------------------
*/

class MMIDCanvasExtended : public MMIDCanvas
{
public:
    /**
     * Adds new observer. Observer will be informed when display gains or loses focus.
     * @param aDirectScreenAccess Interface which will be informed of visibility changes.
     */
    virtual void AddCanvasVisibilityObserver(MDirectScreenAccess* aDirectScreenAccess) = 0;

    /**
     * Removes observer.
     * @param aDirectScreenAccess Observer which will be removed.
     */
    virtual void RemoveCanvasVisibilityObserver(MDirectScreenAccess* aDirectScreenAccess) = 0;
};

#endif // MMIDCANVASEXTENDED_H
