/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/
#ifndef GFXUTILS_H_
#define GFXUTILS_H_

#include "graphics.h"

namespace Java { namespace GFX {

/**
 * Implementation for graphics utils
 */ 

class gfxUtils 
{
public:
	
    /**
     * Method for checking pixel level collision 
     */
    static bool detectCollision(Image* aImage1, int aTransform1, int aP1x, int aP1y, int aR1x1, int aR1y1, int aR1x2, int aR1y2,
                                Image* aImage2, int aTransform2, int aP2x, int aP2y, int aR2x1, int aR2y1, int aR2x2, int aR2y2);
private:
    static int getCgTransformValue(int aTransform);

};

} // namespace GFX
} // namespace Java


#endif /*GFXUTILS_H_*/
