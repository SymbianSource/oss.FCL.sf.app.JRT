/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controls the native global indicators.
*
*/


#ifndef CGLOBALINDICATORS_H
#define CGLOBALINDICATORS_H

// CLASS DECLARATION

class CGlobalIndicators: public CBase
{
public:
    /**
     * Sets the value of the given global indicator.
     *
     * @param aIndicator id of the indicator
     *
     * @param aValue the value to set for the global indicator.
     *
     * @return the value that is currently set for the indicator,
     * or a native error value.
     */
    static TInt SetGlobalIndicator(TInt aIndicator, TInt aValue);

    /**
     * Gets the value of the given global indicator.
     *
     * @param aIndicator id of the indicator
     *
     * @return the value that is currently set for the indicator,
     * or a native error value.
     */
    static TInt GlobalIndicator(TInt aIndicator);
};
#endif // CGLOBALINDICATORS_H


