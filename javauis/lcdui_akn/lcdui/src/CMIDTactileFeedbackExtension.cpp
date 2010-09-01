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
* Description:  Implementation of CMIDTactileFeedbackExtension to store feedback areas for an LCDUI component.
*
*/


#ifdef RD_TACTILE_FEEDBACK

#include "CMIDTactileFeedbackExtension.h"
#include "CMIDCanvas.h"
#include "CMIDCustomItem.h"

#include <j2me/jdebug.h>

CMIDTactileFeedbackExtension::CMIDTactileFeedbackExtension(MMIDTactileFeedbackComponent* aParent, TInt aParentType): TypeCanvas(1), TypeCustomItem(2)

{
    iParent = aParent;
    iParentType = aParentType;
    iFeedbackInstance = MTouchFeedback::Instance();
}


CCoeControl* CMIDTactileFeedbackExtension::GetParentControl()
{
    if (iParentType == TypeCanvas)
    {
        return  &(((CMIDCanvas*)iParent)->Control());
    }
    else if (iParentType == TypeCustomItem)
    {
        return ((CMIDCustomItem*)iParent)->ComponentControl(0);
    }
    return 0;
}


CMIDTactileFeedbackExtension::FeedbackArea* CMIDTactileFeedbackExtension::GetArea(TInt idx)
{
    return &iFeedbackAreasArray[idx];
}

CMIDTactileFeedbackExtension::FeedbackArea* CMIDTactileFeedbackExtension::GetAreaByID(TInt aId, TInt* aIdx)
{
    TInt index = 0;
    while (index < iFeedbackAreasArray.Count())
    {
        FeedbackArea* area;

        area = &iFeedbackAreasArray[index];

        if (area->id == aId)
        {
            if (aIdx) *aIdx = index;
            return area;
        }
        index++;
    }

    return NULL;

}

void CMIDTactileFeedbackExtension::RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle)
{
    FeedbackArea* area = GetAreaByID(aId, NULL);
    TInt err = KErrNone;
    if (area)
    {
        area->rect = aRect;
        area->style = (TTouchLogicalFeedback)aStyle;
    }
    else
    {
        FeedbackArea newArea;
        newArea.id  = aId;
        newArea.rect = aRect;
        newArea.style = (TTouchLogicalFeedback)aStyle;
        err = iFeedbackAreasArray.Append(newArea);
    }

    if (KErrNone == err )
    {
        iParent->UpdateTactileFeedback();
    }
    else
    {
        DEBUG_INT("CMIDTactileFeedbackExtension::RegisterFeedbackArea - RArray append error %d", err);
    }
}

void CMIDTactileFeedbackExtension::SetFeedbackArea(TInt aId, TRect aRect, TInt aStyle)
{
    if (iFeedbackInstance)
        iFeedbackInstance->SetFeedbackArea(GetParentControl(), aId, aRect, (TTouchLogicalFeedback)aStyle, ETouchEventStylusDown);
}


void CMIDTactileFeedbackExtension::RemoveFeedbackArea(TInt aId)
{
    if (iFeedbackInstance)
        iFeedbackInstance->RemoveFeedbackArea(GetParentControl(), aId);
}


TInt CMIDTactileFeedbackExtension::GetAreasCount()
{
    return iFeedbackAreasArray.Count();
}


void CMIDTactileFeedbackExtension::UnregisterFeedbackArea(TInt aId)
{
    TInt idx;
    FeedbackArea* area = GetAreaByID(aId, &idx);
    if (area)
    {
        RemoveFeedbackArea(aId);
        iFeedbackAreasArray.Remove(idx);
        iParent->UpdateTactileFeedback();
    }
}


void CMIDTactileFeedbackExtension::UnregisterFeedbackForControl()
{
    TInt index = 0;
    while (index < iFeedbackAreasArray.Count())
    {
        FeedbackArea* area;
        area = &iFeedbackAreasArray[index];
        RemoveFeedbackArea(area->id);
        index++;
    }

    iFeedbackAreasArray.Reset();
    iParent->UpdateTactileFeedback();
}

void CMIDTactileFeedbackExtension::MoveAreaToFirstPriority(TInt aId)
{
    if (iFeedbackInstance)
        iFeedbackInstance->MoveFeedbackAreaToFirstPriority(GetParentControl(), aId);
}

#endif
