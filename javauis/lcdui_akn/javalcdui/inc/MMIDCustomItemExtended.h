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
* Description:  Interface for observing custom item's position and visibility changes.
*
*/


#ifndef MMIDCUSTOMITEMEXTENDED_H
#define MMIDCUSTOMITEMEXTENDED_H

#include <lcdui.h>

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Interface for observing custom item's position and visibility changes

-----------------------------------------------------------------------------
*/

class MMIDCustomItemObserver
{
public:
    virtual void VisibilityChanged(TBool aVisible) = 0;
    virtual void PositionChanged(TPoint aPosition) = 0;
};

class MMIDCustomItemExtended : public MMIDCustomItem
{
public:
    virtual void SetItemObserver(MMIDCustomItemObserver* aItemObserver) = 0;
};

#endif // MMIDCUSTOMITEMEXTENDED_H
