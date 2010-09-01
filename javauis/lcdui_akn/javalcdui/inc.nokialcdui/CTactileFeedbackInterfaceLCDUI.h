/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MTactileFeedbackInterface.h"
#include "flogger.h"

class CTactileFeedbackInterfaceLCDUI : public CBase, public MTactileFeedbackInterface
{
    CMIDToolkit* iLcduiToolkit;
    MTouchFeedback* iFeedback;

public:
    CTactileFeedbackInterfaceLCDUI(CMIDToolkit* aToolkit);

    virtual void DirectFeedback(TTouchLogicalFeedback aStyle);
    virtual void RegisterFeedbackArea(void* aControl,TInt aControlType,  TInt aAreaIndex, TInt aX, TInt aY, TInt aWidth, TInt aHeight, TTouchLogicalFeedback aStyle);
    virtual void UnregisterFeedbackArea(void* aControl, TInt aControlType, TInt aAreaIndex);
    virtual void UnregisterFeedbackForControl(void* aControl, TInt aControlType);
    virtual void MoveAreaToFirstPriority(void* aControl, TInt aControlType, TInt aAreaIndex);
    virtual TBool IsTouchFeedbackSupported();

};

