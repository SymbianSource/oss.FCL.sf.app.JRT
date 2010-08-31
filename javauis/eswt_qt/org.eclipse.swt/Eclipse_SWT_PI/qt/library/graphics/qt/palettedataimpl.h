/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/

#ifndef PALETTEDATAQT_H_
#define PALETTEDATAQT_H_

#include <QByteArray>
#include "graphics.h"

namespace Java { namespace GFX {

/**
 * Helper class for parsing <code>org.eclipse.swt.graphics.PaletteData</code>
 */
class IndexedPaletteDataWrapper : public PaletteDataWrapper
{

public:
    /**
     * Ctor
     */
    IndexedPaletteDataWrapper(QVector<QRgb>& aIndexedRgbData);

    /**
     * Dtor
     */
    virtual ~IndexedPaletteDataWrapper();

    /**
     * @see PaletteDataWrapper::isDirect
     */
    virtual inline bool isDirect()
    {
        return false;
    }

    /**
     * @see PaletteDataWrapper::getDirectData
     */
    virtual inline TDirectData* getDirectData()
    {
        return NULL;
    }

    /**
     * @see PaletteDataWrapper::getIndexedDataCount
     */
    virtual int getIndexedRgbCount()
    {
        return mIndexedRgbData.size();
    }

    virtual QVector<QRgb>* getIndexedPalette()
    {
        return &mIndexedRgbData;
    }

    /**
     * @see PaletteDataWrapper::getIndexedData
     */
    virtual TIndexedRgb getIndexedRgb(int aIndex);

protected:
    /**
     * Copy ctor. not implemented.
     */
    IndexedPaletteDataWrapper(const IndexedPaletteDataWrapper&);

    /**
     * Assignment operator, Not implemented.
     */
    IndexedPaletteDataWrapper& operator=(const IndexedPaletteDataWrapper&);

protected:
    QVector<QRgb> mIndexedRgbData;
};

/**
 * Helper class for parsing <code>org.eclipse.swt.graphics.PaletteData</code>
 */
class DirectPaletteDataWrapper : public PaletteDataWrapper
{

public:
    /**
     * Ctor
     */
    DirectPaletteDataWrapper(PaletteDataWrapper::TDirectData& aDirectData);

    /**
     * Dtor
     */
    virtual ~DirectPaletteDataWrapper();

    /**
     * @see PaletteDataWrapper::isDirect
     */
    virtual inline bool isDirect()
    {
        return true;
    }

    /**
     * @see PaletteDataWrapper::getDirectData
     */
    virtual inline TDirectData* getDirectData()
    {
        return &mDirectData;
    }

    /**
     * @see PaletteDataWrapper::getIndexedDataCount
     */
    virtual inline int getIndexedRgbCount()
    {
        return 0;
    }

    /**
     * @see PaletteDataWrapper::getIndexedData
     */
    virtual inline TIndexedRgb getIndexedRgb(int /*aIndex*/)
    {
        return TIndexedRgb();
    }

    virtual QVector<QRgb>* getIndexedPalette()
    {
        return 0;
    }

    /**
     * Shifts an integer by a certain number of bits.
     * @param value The integer whose bits are to be shifted
     * @param shift The number of bits to shift by; a positive value shifts
     * to the left, a negative value shifts to the right.
     */
    static inline int bitShift(int aValue, int aShift)
    {
        return (aShift >= 0) ? (aValue << aShift) : (aValue >> -aShift);
    }

protected:
    /**
     * Copy ctor. not implemented.
     */
    DirectPaletteDataWrapper(const DirectPaletteDataWrapper&);

    /**
     * Assignment operator, Not implemented.
     */
    DirectPaletteDataWrapper& operator=(const DirectPaletteDataWrapper&);

protected:
    TDirectData mDirectData;
};

} // namespace GFX
} // namespace Java

#endif /*PALETTEDATAQT_H_*/
