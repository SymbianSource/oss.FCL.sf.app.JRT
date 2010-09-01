/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDTACTILEFEEDBACKEXTENSION_H
#define CMIDTACTILEFEEDBACKEXTENSION_H

#include <e32def.h>
#include <coecntrl.h>
#include <lcdui.h>
#include <touchfeedback.h>



/**
 * Utility class to handle tactile feedback areas.
 *
 * @since S60 5.0
 */


class CMIDTactileFeedbackExtension : public CBase
{
public:

    struct FeedbackArea
    {
        int id;
        TRect rect;
        TTouchLogicalFeedback style;
    };

    CMIDTactileFeedbackExtension(MMIDTactileFeedbackComponent* aParent, TInt aParentType);

    void RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle);
    void SetFeedbackArea(TInt aId, TRect aRect, TInt aStyle);
    void RemoveFeedbackArea(TInt aId);

    void UnregisterFeedbackArea(TInt aId);
    void UnregisterFeedbackForControl();
    void MoveAreaToFirstPriority(TInt aId);



    FeedbackArea* GetAreaByID(TInt aId, TInt* aIdx);
    FeedbackArea* GetArea(TInt idx);
    TInt GetAreasCount();

private:

    const TInt TypeCanvas;     // = 1;
    const TInt TypeCustomItem; // = 2;

    TInt iParentType;
    CCoeControl* GetParentControl();

    MMIDTactileFeedbackComponent* iParent;
    MTouchFeedback* iFeedbackInstance;
    RArray<FeedbackArea> iFeedbackAreasArray;
};

#endif // CMIDTACTILEFEEDBACKEXTENSION_H

// End of File
