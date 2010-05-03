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

void _CreateMTouchFeedback(MTouchFeedback**  aHandle);

void _DoDirectFeedback(MTouchFeedback *aHandle, TTouchLogicalFeedback aStyle);
void _RegisterFeedbackArea(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex, TRect aFeedbackRect, TTouchLogicalFeedback aStyle);
void _UnregisterFeedbackArea(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex);
void _UnregisterFeedbackForControl(MTouchFeedback *aHandle, CCoeControl* aControl);
void _MoveAreaToFirstPriority(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex);



TBool _IsTouchFeedbackSupported(MTouchFeedback *aHandle);

void _RegisterFeedbackAreaForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex, TRect aFeedbackRect, TTouchLogicalFeedback aStyle);

void _UnregisterFeedbackAreaForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex);
void _UnregisterFeedbackForControlForComponent(MMIDTactileFeedbackComponent* aComponent);
void _MoveAreaToFirstPriorityForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex);
