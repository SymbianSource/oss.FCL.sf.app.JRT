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


#ifndef CMIDDEFAULTBACKGROUND_H
#define CMIDDEFAULTBACKGROUND_H


// INCLUDE FILES
// using CCoeControl inheritance in class declaration
#include <coecntrl.h>
#include <lcdui.h>

//
// CMIDDefaultBackground
//

NONSHARABLE_CLASS(CMIDDefaultBackground) : public CCoeControl, public MMIDComponent
{
public:
    static CMIDDefaultBackground* NewL(CCoeControl& aWindow);
    ~CMIDDefaultBackground();

private:
    CMIDDefaultBackground();
    void ConstructL(CCoeControl& aWindow);

//
// MMIDComponent
//

public:
    virtual void Dispose();
    virtual MMIDComponent::TType Type() const;

};

#endif // CMIDDEFAULTBACKGROUND_H
