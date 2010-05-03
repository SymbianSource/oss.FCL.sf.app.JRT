/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MTactileFeedbackInterface of eSWT midlets.
*
*/


#ifdef RD_TACTILE_FEEDBACK

#include "CMIDToolkit.h"
#include <touchfeedback.h>
#include <eswtwidgetscore.h>
#include "CTactileFeedbackInterfaceESWT.h"
#include "TactileFeedbackImpl.h"


class TactileFeedback_creator : public MSwtFunctor
{
public:
    void (*create)(MTouchFeedback**);


    mutable MTouchFeedback* retval;

    TactileFeedback_creator()
    {
        create = &_CreateMTouchFeedback;
    }

    void operator()() const
    {
        create(&retval);
    }
};

class TactileFeedback_directFeedback : public MSwtFunctor
{
public:
    void (*directFeedback)(MTouchFeedback*, TTouchLogicalFeedback);
    MTouchFeedback* iTactileFeeback;
    TTouchLogicalFeedback iStyle;

    TactileFeedback_directFeedback()
    {
        directFeedback = &_DoDirectFeedback;
    }

    void operator()() const
    {
        directFeedback(iTactileFeeback, iStyle);
    }
};


class TactileFeedback_isSupported : public MSwtFunctor
{
public:
    mutable TBool retval;
    TBool(*isSupported)(MTouchFeedback*);
    MTouchFeedback* iTactileFeeback;

    TactileFeedback_isSupported()
    {
        isSupported = &_IsTouchFeedbackSupported;
    }
    void operator()() const
    {
        retval = isSupported(iTactileFeeback);
    }
};




class TactileFeedback_registerArea : public MSwtFunctor
{
public:
    void (*registerArea)(MTouchFeedback*, CCoeControl*, TInt, TRect, TTouchLogicalFeedback);
    MTouchFeedback* iTactileFeeback;
    CCoeControl* iControl;
    TInt iAreaIndex;
    TRect iArea;
    TTouchLogicalFeedback iStyle;

    TactileFeedback_registerArea()
    {
        registerArea = &_RegisterFeedbackArea;
    }

    void operator()() const
    {
        registerArea(iTactileFeeback, iControl, iAreaIndex, iArea, iStyle);
    }
};


class TactileFeedback_removeArea : public MSwtFunctor
{
public:
    void (*removeArea)(MTouchFeedback*, CCoeControl*, TInt);
    MTouchFeedback* iTactileFeeback;
    CCoeControl* iControl;
    TInt iAreaIndex;

    TactileFeedback_removeArea()
    {
        removeArea = &_UnregisterFeedbackArea;
    }

    void operator()() const
    {
        removeArea(iTactileFeeback, iControl, iAreaIndex);
    }
};

class TactileFeedback_removeFeedbackForControl : public MSwtFunctor
{
public:
    void (*removeFeedback)(MTouchFeedback*, CCoeControl*);
    MTouchFeedback* iTactileFeeback;
    CCoeControl* iControl;

    TactileFeedback_removeFeedbackForControl()
    {
        removeFeedback = &_UnregisterFeedbackForControl;
    }

    void operator()() const
    {
        removeFeedback(iTactileFeeback, iControl);
    }
};


class TactileFeedback_moveAreaToFirstPriority : public MSwtFunctor
{
public:
    void (*moveArea)(MTouchFeedback*, CCoeControl*, TInt);
    MTouchFeedback* iTactileFeeback;
    CCoeControl* iControl;
    TInt iAreaIndex;

    TactileFeedback_moveAreaToFirstPriority()
    {
        moveArea = &_MoveAreaToFirstPriority;
    }

    void operator()() const
    {
        moveArea(iTactileFeeback, iControl, iAreaIndex);
    }
};


CTactileFeedbackInterfaceESWT::CTactileFeedbackInterfaceESWT(MSwtClient* aESWT_client)
{
    iESWT_client = aESWT_client;
    TactileFeedback_creator creator;
    iESWT_client->Execute(creator);
    iFeedback = creator.retval;

};


void CTactileFeedbackInterfaceESWT::DirectFeedback(TTouchLogicalFeedback aStyle)
{

    TactileFeedback_directFeedback directFeedback;
    directFeedback.iTactileFeeback = iFeedback;
    directFeedback.iStyle = aStyle;
    iESWT_client->Execute(directFeedback);
};

void CTactileFeedbackInterfaceESWT::RegisterFeedbackArea(void* aControl, TInt /*aControlType*/, TInt aAreaIndex, TInt aX, TInt aY, TInt aWidth, TInt aHeight, TTouchLogicalFeedback aStyle)
{
    MSwtControl* ctrl = (MSwtControl*)aControl;
    CCoeControl& cc = ctrl->CoeControl();


    TRect area(aX, aY, aX + aWidth, aY + aHeight);
    area.Move(cc.Position());
    TactileFeedback_registerArea registerArea;
    registerArea.iTactileFeeback = iFeedback;
    registerArea.iControl = &cc;
    registerArea.iAreaIndex = aAreaIndex;
    registerArea.iArea = area;
    registerArea.iStyle = aStyle;
    iESWT_client->Execute(registerArea);
};

void CTactileFeedbackInterfaceESWT::UnregisterFeedbackArea(void* aControl, TInt /*aControlType*/, TInt aAreaIndex)
{
    MSwtControl* ctrl = (MSwtControl*)aControl;
    CCoeControl& cc = ctrl->CoeControl();

    TactileFeedback_removeArea removeArea;
    removeArea.iTactileFeeback = iFeedback;
    removeArea.iControl = &cc;
    removeArea.iAreaIndex = aAreaIndex;
    iESWT_client->Execute(removeArea);
}

void CTactileFeedbackInterfaceESWT::UnregisterFeedbackForControl(void* aControl, TInt /*aControlType*/)
{
    MSwtControl* ctrl = (MSwtControl*)aControl;
    CCoeControl& cc = ctrl->CoeControl();

    TactileFeedback_removeFeedbackForControl removeFeedback;
    removeFeedback.iTactileFeeback = iFeedback;
    removeFeedback.iControl = &cc;
    iESWT_client->Execute(removeFeedback);
}

void CTactileFeedbackInterfaceESWT::MoveAreaToFirstPriority(void* aControl, TInt /*aControlType*/, TInt aAreaIndex)
{
    MSwtControl* ctrl = (MSwtControl*)aControl;
    CCoeControl& cc = ctrl->CoeControl();

    TactileFeedback_moveAreaToFirstPriority moveArea;
    moveArea.iTactileFeeback = iFeedback;
    moveArea.iControl = &cc;
    moveArea.iAreaIndex = aAreaIndex;
    iESWT_client->Execute(moveArea);

}

TBool CTactileFeedbackInterfaceESWT::IsTouchFeedbackSupported()
{
    TactileFeedback_isSupported isFeedbackSupported;
    isFeedbackSupported.iTactileFeeback = iFeedback;
    iESWT_client->Execute(isFeedbackSupported);

    return isFeedbackSupported.retval;
}

#endif
