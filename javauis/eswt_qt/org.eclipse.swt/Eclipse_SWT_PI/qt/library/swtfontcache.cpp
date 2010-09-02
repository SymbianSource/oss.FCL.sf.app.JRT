/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QFont>
#include "swtfontcache.h"
#include "swtlog.h"

#define SWTFONTCACHE_GRANULARITY 10

namespace Java { namespace eSWT {

SwtFontCache::SwtFontCache() : mFonts(NULL), mSize(0), mCount(0)
{
    SWT_LOG_FUNC_CALL();
}

SwtFontCache::~SwtFontCache()
{
    SWT_LOG_FUNC_CALL();
        
    for(int i = 0; i < mCount; ++i)
    {
        delete mFonts[i];
        mFonts[i] = NULL;
    }
    delete[] mFonts;
    mFonts = NULL;
}

QFont* SwtFontCache::cache(QFont* aFont)
{
    SWT_LOG_FUNC_CALL();
    
    QFont* cached = findEqual( aFont );
    if( cached )
    {
        delete aFont;
        aFont = NULL;
    }
    else
    {
        add( aFont );
        cached = aFont;
    }
    return cached;
}

bool SwtFontCache::isCached(const QFont* const aFont)
{
    SWT_LOG_FUNC_CALL();
    
    for(int i = 0; i < mCount; ++i)
    {
        if(mFonts[i] == aFont)
        {
            return true;
        }
    }
    return false;
}

QFont* SwtFontCache::findEqual(QFont* aFont)
{
    SWT_LOG_FUNC_CALL();

    if(aFont == NULL)
    {
        return NULL;
    }
    for(int i = 0; i < mCount; ++i)
    {
        if( *(mFonts[i]) == *aFont )
        {
            return mFonts[i];
        }
    }
    return NULL;
}

void SwtFontCache::add(QFont* aFont)
{
    SWT_LOG_FUNC_CALL();

    if(mSize == mCount)
    {
        const int newSize = mSize + SWTFONTCACHE_GRANULARITY;
        QFont** bigger = new QFont* [newSize];
        if(mFonts != NULL)
        {
            ::memcpy( bigger, mFonts, mSize * sizeof(QFont*) );
            delete[] mFonts;
        }
        ::memset( &bigger[mSize], 0, (newSize - mSize) * sizeof(QFont*) );
        mFonts = bigger;
        mSize = newSize;
    }
    mFonts[mCount++] = aFont;
}

}}

