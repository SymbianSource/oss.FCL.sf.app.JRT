/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MTACTILEFEEDBACKINTERFACE_H
#define MTACTILEFEEDBACKINTERFACE_H


class MTactileFeedbackInterface
{
public:

    virtual void DirectFeedback(TTouchLogicalFeedback aStyle) = 0;
    virtual void RegisterFeedbackArea(void* aControl, TInt aControlType, TInt aAreaIndex, TInt aX, TInt aY, TInt aWidth, TInt aHeight, TTouchLogicalFeedback aStyle) = 0;
    virtual void UnregisterFeedbackArea(void* aControl, TInt aControlType, TInt aAreaIndex) = 0;
    virtual void UnregisterFeedbackForControl(void* aControl, TInt aControlType) = 0;
    virtual void MoveAreaToFirstPriority(void* aControl, TInt aControlType, TInt aAreaIndex) = 0;
    virtual TBool IsTouchFeedbackSupported() = 0;

};

#endif // MTACTILEFEEDBACKINTERFACE_H
