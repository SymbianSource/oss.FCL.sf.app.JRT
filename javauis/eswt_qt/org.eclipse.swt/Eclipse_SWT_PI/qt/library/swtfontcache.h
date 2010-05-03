/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef _SWT_FONTCACHE_H_
#define _SWT_FONTCACHE_H_

#include <QObject>

class QFont;

namespace Java { namespace eSWT {

/**
 * Cache of fonts owned by the SWT-API. 
 *
 * Fonts that are given out by the API but were not created by the client are 
 * stored in this cache. These fonts are owned by the SWT-API and kept here for
 * the life-time of the Display. This enables robust and efficient font 
 * resource management. 
 */
class SwtFontCache : public QObject {
    Q_OBJECT

public:
    /**
     * The constructor.
     */
    SwtFontCache();

    /**
     * The destructor.
     */
    virtual ~SwtFontCache();
    
    /**
     * Caches the given QFont. If equal QFont is already in the cache then the 
     * given QFont is deleted. 
     * @param aFont A pointer to the QFont to cache. Must be allocated on heap.
     * @return A pointer to the cached QFont. Can be the same as the given 
     * pointer if it was added to the cache, or different if an equal font was 
     * already found in the cache. 
     */
    QFont* cache(QFont* aFont);
    
    /**
     * Checks if the given QFont pointer is already in the cache. 
     * @param aFont A pointer to a QFont. Must be allocated on heap.
     * @return True if the pointer is in the cache. 
     */
    bool isCached(const QFont* const aFont);
        
private:
    /**
     * Gets a matching font from the cache. 
     * @param aFont A pointer to the font to search equal font for. 
     * @return A pointer to the cached QFont that is equal to the given font or 
     * NULL if matching font is not found in the cache. 
     */
    QFont* findEqual(QFont* aFont);
    
    /**
     * Add a QFont pointer to the cache. The QFont must be allocated on heap. 
     * @param aFont Pointer to add. 
     */
    void add(QFont* aFont);

    SwtFontCache(const SwtFontCache&);
    SwtFontCache& operator=(const SwtFontCache&);
    
    QFont** mFonts;
    int mSize;
    int mCount;
};

}}

#endif // _SWT_FONTCACHE_H_

