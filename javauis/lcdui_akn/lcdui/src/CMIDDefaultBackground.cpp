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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "CMIDDefaultBackground.h"


//
// CMIDDefaultBackground
//

CMIDDefaultBackground* CMIDDefaultBackground::NewL(CCoeControl& aWindow)
{
    CMIDDefaultBackground* self = new(ELeave) CMIDDefaultBackground();
    CleanupStack::PushL(self);
    self->ConstructL(aWindow);
    CleanupStack::Pop(self);
    return self;
}

CMIDDefaultBackground::CMIDDefaultBackground()
{
}

void CMIDDefaultBackground::ConstructL(CCoeControl& aWindow)
{
    SetContainerWindowL(aWindow);
}

CMIDDefaultBackground::~CMIDDefaultBackground()
{
}

//
// MMIDComponent
//

void CMIDDefaultBackground::Dispose()
{
    delete this;
}

MMIDComponent::TType CMIDDefaultBackground::Type() const
{
    return MMIDComponent::EDefaultBackground;
}


