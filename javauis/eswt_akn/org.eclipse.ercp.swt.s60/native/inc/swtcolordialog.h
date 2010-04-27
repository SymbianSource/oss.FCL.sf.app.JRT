/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCOLORDIALOG_H
#define SWTCOLORDIALOG_H


#include <e32base.h>
#include <AknColourSelectionGrid.h>


/**
 * CSwtColorDialog
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtColorDialog)
        : public CBase
{
public:
    /**
     * Destructor
     */
    ~CSwtColorDialog();

    /**
     * Opens a ColorDialog
     * @return a TRgb located at Heap or NULL if none selected.
     */
    static TRgb* RunDlgL(TRgb* aDefaultColor);

private:
    /**
     * Constructor
     * @param default selected color
     */
    CSwtColorDialog(TRgb* aDefaultColor);

    /**
     * Opens a ColorDialog
     * @return a TRgb located at Heap or NULL if none selected.
     */
    TRgb* OpenLD();

private: // Data
    /**
     * Pointer to the default color if exists.
     */
    TRgb* iDefaultColor;
};

#endif // SWTCOLORDIALOG_H
