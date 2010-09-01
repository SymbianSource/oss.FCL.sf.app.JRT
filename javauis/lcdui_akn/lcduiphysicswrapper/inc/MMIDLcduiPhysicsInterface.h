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
* Description:  Class to provide physics interface for LCDUI
*
*/


#ifndef MMIDLCDUIPHYSICSINTERFACE_H
#define MMIDLCDUIPHYSICSINTERFACE_H

// INCLUDES
#include <coecntrl.h>

#include "MMIDLcduiPhysicsObserver.h"

class MMIDLcduiPhysicsInterface
{
public:

    virtual TBool FeatureEnabled() = 0;

    virtual void InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize, const TBool aDirection) = 0;

    virtual void RegisterPanningPosition(const TPoint& aPosition) = 0;

    virtual TBool StartFlick(const TPoint& aLength, TTime aDuration) = 0;

    virtual TBool CanBeStopped() const = 0;

    virtual void StopPhysics() = 0;

    virtual void ResetFriction() = 0;

    virtual TInt DragThreshold() const = 0;

    virtual TInt HighlightTimeout() const = 0;

    virtual ~MMIDLcduiPhysicsInterface() {}
};

// Typedef for factory function
typedef MMIDLcduiPhysicsInterface*(*GetInstance)(CCoeControl*, MMIDLcduiPhysicsObserver*);

#endif // MMIDLCDUIPHYSICSINTERFACE_H
