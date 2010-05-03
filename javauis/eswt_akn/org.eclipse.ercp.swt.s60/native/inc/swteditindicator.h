/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTEDITINDICATOR_H
#define SWTEDITINDICATOR_H


#include <aknEditStateIndicator.h>
#include <AknIndicatorContainer.h>


class CCoeControl;


const TInt KSWTEditIndicatorHeight = 25;


/**
 * CSwtEditIndicatorRedictor
 * An object provider participant to flag editors to direct their
 * FEP to SWTEDitIndicator. No real implementation for this class
 * used as a flag.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtEditIndicatorRedictor)
        : public CBase
{
public:
    DECLARE_TYPE_ID(0x0AED2D06)
    CSwtEditIndicatorRedictor();
    ~CSwtEditIndicatorRedictor();
};

/**
 * CSwtEditIndicatorRedictor
 * a FEP indicator to draw it inside the main area.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtEditIndicator)
        : public CBase
        , public MAknEditingStateIndicator
{
// Own methods
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param Parent control
     * @return Pointer to the created instance
     */
    static CSwtEditIndicator* NewL(CCoeControl& aControl);

    /**
     * C++ Destructor.
     */
    ~CSwtEditIndicator();

private:
    /**
     * Constructor
     */
    CSwtEditIndicator();

    /**
    * Second phase constructor
    * @param parent control
    */
    void ConstructL(CCoeControl& aControl);

// From MAknEditingStateIndicator
public:
    void SetState(TAknEditingState aState);
    CAknIndicatorContainer* IndicatorContainer();

// Own methods
private:
    /**
     * Sets new indicator to container
     * @param aIndicator Indicator id
     * @param aState     State of the indicator
     * @param aDrawNow   Should the indicator be drawn now
     */
    void SetIndicator(TInt aIndicator, TInt aState, TBool aDrawNow);

private: // Data
    /**
     * The Indicator control
     * Own
     */
    CAknIndicatorContainer* iIndicatorContainer;
};


#endif // SWTEDITINDICATOR_H
