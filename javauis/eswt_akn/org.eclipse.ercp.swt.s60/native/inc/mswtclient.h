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


#ifndef MSWTCLIENT_H
#define MSWTCLIENT_H


#include "swtfunctor.h"


class MSwtDisplay;


/**
 * MSwtClient allows execution in eSWT user-interface thread and provides
 * a way to obtain MSwtDisplay instance.
 */
class MSwtClient
{
public:
    /**
     * Executes aFunctor in eSWT user-interface thread.
     * @param aFunctor functor to execute
     */
    virtual TInt Execute(const MSwtFunctor& aFunctor) const = 0;

    /**
     * @return associated display
     */
    virtual MSwtDisplay* Display() const = 0;

    /**
     * Allow delete through this interface.
     */
    virtual ~MSwtClient() {};
};


#endif // MSWTCLIENT_H
