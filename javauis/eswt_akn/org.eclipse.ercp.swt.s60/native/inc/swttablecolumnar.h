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

#ifndef SWTTABLECOLUMNAR_H
#define SWTTABLECOLUMNAR_H


#include "eswt.h"


/**
 *  This interface collects the functions that are common to all the columnar
 *  drawable items in a Table. I.e. the header and the items.
 *  @lib eswt.dll
 */
class MSwtTableColumnar
{
    /**
     * Returns the preferred size of the whole item.
     * @return The preferred size
     */
    virtual TSize GetPreferredSize() const = 0;

    /**
     * Computes the preferred size of the cell specified by its column index.
     * @param aColumnIndex The column index to calculate for.
     * @return The preferred size
     */
    TSize GetPreferredSize(const TInt& aColumnIndex) const;

    /**
     * Insert a new column at the specified index.
     * The new column will use the default colors, fonts, etc.
     * @param aIndex The column index to insert at.
     */
    virtual void InsertNewColumnL(const TInt& aIndex) = 0;

    /**
     * Removes data (image, text, colors, ...) contained in the specified column
     * @param aIndex The column index to remove.
     */
    virtual void RemoveColumn(const TInt& aIndex) = 0;

    /**
     * Sets the text of the cell at the specified index.
     * @param aColumnIndex The column index to set.
     * @param aText        The text to set.
     */
    virtual void SetTextL(const TInt& aColumnIndex, const TDesC& aText) = 0;

    /**
     * Returns the text at the specified column index
     * @param aColumnIndex The column index to get.
     * @return             The text in the column.
     */
    virtual const TDesC& GetText(const TInt& aColumnIndex) const = 0;

    /**
     * Checks if there is at least one image in the specified column.
     * @param aColumnIndex The column index to get.
     * @return             True if there is one or more images in the column.
     */
    virtual TBool IsImageSet(const TInt& aColumnIndex) const = 0;

    /**
     * Sets the image of the cell at the specified index.
     * @param aColumnIndex  The column index to set.
     * @param aImage        The image to set.
     */
    virtual void SetImageByIndex(const TInt& aColumnIndex, const MSwtImage* aImage) = 0;
};


#endif // SWTTABLECOLUMNAR_H
