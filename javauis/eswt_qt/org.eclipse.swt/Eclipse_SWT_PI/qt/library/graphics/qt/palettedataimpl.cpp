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

#include "palettedataimpl.h"

namespace Java { namespace GFX {

IndexedPaletteDataWrapper::IndexedPaletteDataWrapper(QVector<QRgb>& aIndexedRgbData)
: mIndexedRgbData(aIndexedRgbData)
{
}

IndexedPaletteDataWrapper::~IndexedPaletteDataWrapper()
{
}

PaletteDataWrapper::TIndexedRgb IndexedPaletteDataWrapper::getIndexedRgb(int aIndex)
{
    QRgb rgb = mIndexedRgbData.at(aIndex);
    return TIndexedRgb(qRed(rgb), qGreen(rgb), qBlue(rgb));
}

DirectPaletteDataWrapper::DirectPaletteDataWrapper(
                PaletteDataWrapper::TDirectData& aDirectData)
: mDirectData(aDirectData)
{
}

DirectPaletteDataWrapper::~DirectPaletteDataWrapper()
{
}

} // namespace GFX
} // namespace Java

