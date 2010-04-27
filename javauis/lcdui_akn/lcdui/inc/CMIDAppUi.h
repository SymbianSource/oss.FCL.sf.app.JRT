/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDAPPUI_H
#define CMIDAPPUI_H

// CAknAppUi inheritance in class declaration
#include <lcdui.h>
// MLcduiPlugin in class declaration
// MMIDObserver used as parameter in SetObserver function
#include <Lcdglue.h>

#include "coreuiavkonlcdui.h"
#include "coreuiappuichild.h"

class CMIDDisplayable;
class CMIDApplication;

NONSHARABLE_CLASS(CMIDAppUi) : public CBase, public java::ui::CoreAppUiChild, private MLcduiPlugin
{
public:
    CMIDAppUi();
    ~CMIDAppUi();

    void ConstructL();
    void SetCurrentDisplayable(CMIDDisplayable* aDisplayable);
    void HandleCommandL(TInt aCommand);
    void HandleSwitchOnEventL(CCoeControl* aDestination);
    void HandleForegroundEventL(TBool aForeground);
    void HandleResourceChangeL(TInt aType);
    void HandleApplicationSpecificEventL(TInt aType,const TWsEvent& aEvent);
    TBool HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    void AddToStackL(CCoeControl *aControl, TInt aPriority=ECoeStackPriorityDefault, TInt aStackingFlags=ECoeStackFlagStandard);
    TRect ApplicationRect() const;
    TRect ClientRect() const;
    CAknToolbar* CurrentFixedToolbar() const;
    void RemoveFromStack(CCoeControl *aControl);
    CEikStatusPane* StatusPane();

    RPointerArray<CMIDDisplayable>& Displayables();

    void SetEnv(MMIDEnv& aEnv);
    void UnSetEnv();


private:
    // MLcduiPlugin
    void SetObserver(MMIDObserver* aObserver);
    MMIDComponentFactory* CreateComponentFactoryL();


private: // data
    MMIDObserver* iObserver;
    RPointerArray<CMIDDisplayable> iDisplayables;
    java::ui::CoreUiAvkonAppUi*    mCoreAppUi;
    CAknAppUi*                     mJavaAppUi;

    MMIDEnv* iEnv;
    CMIDDisplayable* iCurrentDisplayable;
};

#endif // CMIDAPPUI_H

