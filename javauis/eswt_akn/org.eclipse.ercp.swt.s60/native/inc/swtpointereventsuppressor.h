/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifdef RD_SCALABLE_UI_V2
#ifndef SWTPOINTEREVENTSUPPRESSOR_H
#define SWTPOINTEREVENTSUPPRESSOR_H

#include <aknpointereventsuppressor.h>

class MSwtControl;

NONSHARABLE_CLASS(CSwtPointerEventSuppressor)
        : public CBase
{
// Methods
public:
    static CSwtPointerEventSuppressor* NewL(const MSwtControl* aControl);
    ~CSwtPointerEventSuppressor();

    /**
     * Returns true if suppressor is enabled; otherwise false.
     */
    TBool IsSuppressionEnabled() const;

    /**
     * Process pointer event and decides whether it should be suppressed.
     * @return true if event shoudl be suppressed; otherwise false.
     */
    TBool SuppressPointerEvent(const TPointerEvent& aPointerEvent);

    /**
     * Returns pointer event position for current event.
     * If pointer down event appeared and some consecutive events were
     * suppressed, then this method returns same position for pointer
     * up, as was for pointer down.
     */
    TPoint PointerEventPosition() const;

private:
    CSwtPointerEventSuppressor();
    void ConstructL(const MSwtControl* aControl);

// Data
private:
    /**
     * AvKon pointer event suppressor
     */
    CAknPointerEventSuppressor* iPointerEventSuppressor;

    /**
     * Stores either current evet position, or last ButtonDown position if
     * some event suppression is ongoing and current event is ButtonUp.
     * In case there were some Drag events which weren't suppressed,
     * proper (current) position is stored.
     */
    TPoint iPointerEventPosition;

    /**
     * Stores last ButtonDown position.
     */
    TPoint iLastButtonDownPointerEventPosition;

    /**
     * Indicates whether suppression is ongoing.
     */
    TBool iIsSuppressionOngoing;
};


#endif // SWTPALETTEDATA_H
#endif // SWTPOINTEREVENTSUPPRESSOR_H
