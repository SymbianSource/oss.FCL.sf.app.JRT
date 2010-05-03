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
* Description: Implements MID edit indicator container.
*
*/


#ifndef CMIDEDITINGSTATEINDICATOR_H
#define CMIDEDITINGSTATEINDICATOR_H

// EXTERNAL INCLUDES
#include <AknIndicatorContainer.h>
#include <aknEditStateIndicator.h>

// FORWARD DECLARATIONS
class MMIDCustomComponentContainer;

// CLASS DESCRIPTION
/**
 * Implements MID edit indicator container.
 */
NONSHARABLE_CLASS(CMIDEditingStateIndicator) :
        public CAknIndicatorContainer,
        public MAknEditingStateIndicator
{
public: // Type definitions

    /**
     * Indicator's state
     */
    enum TIndicatorState
    {
        // Disabled. Should not be shown
        EIndicatorStateDisabled = 0,
        // Relative to a component.
        EIndicatorStateRelative,
        // Freely positioned.
        EIndicatorStateAbsolute
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @return New instance from this class
     */
    static CMIDEditingStateIndicator* NewL();

    /**
     * Destructor.
     */
    virtual ~CMIDEditingStateIndicator();

public: // From CCoeControl

    /**
     * Sets the container window for this UI control.
     *
     * @param aParent The container window for this control.
     * @since S60 5.0
     */
    void SetContainerWindowL(CCoeControl* aParent);

    /**
     * Sets the size of this indicator container.
     *
     * Note: Some parameters for the size can be constant.
     * Note: Calling this function results in a call to SizeChanged().
     * Note: Calling this function results in a call to PositionChanged().
     *
     * @param aWidth The width of the indicator in pixels.
     * @param aHeight The height of the indicator in pixels.
     * @since S60 5.0
     */
    void SetSize(TInt aWidth, TInt aHeight);

public: // From CCoeControl

    /**
     * Draws the background for this indicator container.
     *
     * @param aRect The rect to which to draw.
     * @since S60 5.0
     */
    void Draw(const TRect& aRect) const;

public: // From MAknEditingStateIndicator

    /**
     * Sets editing state indicator state.
     *
     * @param aState State of the editing indicator.
     * @since S60 5.0
     */
    void SetState(TAknEditingState aState);

    /**
     * Gets pointer to indicator container that contains
     * the editing state indicators.
     *
     * @return Indicator container of the editing indicators.
     * @since S60 5.0
     */
    CAknIndicatorContainer* IndicatorContainer();

public: // New methods

    /**
     * Gets an object whose type is encapsulated by the
     * specified <code>TTypeUid object</code>.
     *
     * @param aId Encapsulates the Uid that identifies the type
     *        of object required.
     * @return Encapsulates the pointer to the object provided.
     *         Note that the encapsulated pointer may be NULL.
     * @since S60 5.0
     */
    TTypeUid::Ptr SupplyIndicatorMopObject(TTypeUid aId);

    /**
     * Sets the position of this indicator component.
     *
     * Position is relative to the parent object.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    void SetPosition(TInt aX, TInt aY);

    /**
     * Sets this indicator component visible if it is hidden.
     *
     * Depending on the current status of the indicator, this operation
     * is no-op if there is nothing to do. (i.e. the indicator is set
     * hidden when it is already hidden).
     *
     * @param aVisible Indicates the visibility status of the indicator.
     * @since S60 5.0
     */
    void SetVisibleL(TBool aVisible);

    /**
     * Returns the state of this indicator.
     *
     * @return The state of this indicator.
     * @since S60 5.0
     */
    TIndicatorState EnabledState() const;

    /**
     * Sets a new state for this indicator.
     *
     * @param aState The new state.
     * @since S60 5.0
     */
    void SetEnabledState(TIndicatorState aState);

public: // New methods

    /**
     * Redraws the indicator's content correctly.
     *
     * Note that this method must be used to redraw the indicator correctly
     * in situations that the background color is transparent.
     * @since S60 5.0
     */
    void Redraw() const;

    /**
     * Used to override default colors.
     *
     * @see CCoeControl::OverrideColorL()
     *
     * @param aLogicalColor The logical colour. Indicates which part of a
     *        control the physical colour maps to. The set of logical
     *        colours for a standard application are defined in
     *        TLogicalColor.
     *
     * @param aColor The new physical colour to which the logical colour
     *        should be mapped.
     * @since S60 5.0
     */
    void OverrideColorL(TInt aLogicalColor, TRgb aColor);

private: // From CAknIndicatorContainer

    /**
     * Needs to be defined here for linking compatibility.
     * @since S60 5.0
     */
    void Reserved_1();

private: // Construtors

    /**
     * C++ constructor.
     */
    CMIDEditingStateIndicator();

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // New methods

    /**
     * Sets new indicator to container
     *
     * @param aIndicator Indicator id.
     * @param aState     State of the indicator.
     * @param aDrawNow   Should the indicator be drawn now.
     * @since S60 5.0
     */
    void SetIndicator(TInt aIndicator, TInt aState, TBool aDrawNow);

private: // Data

    // State of the indicators.
    TIndicatorState iState;
    // Editing state.
    TAknEditingState iEditingState;
    // Background color of the indicator.
    TRgb iBackgroundColor;
    // Top parent of this control. Not owned.
    CCoeControl* iParent;
};

#endif // CMIDEDITINGSTATEINDICATOR_H

// End of file
