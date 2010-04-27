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


#ifndef SWTCLIENT_H
#define SWTCLIENT_H


#include <e32std.h>


class MSwtFunctor;


/**
 * RSwtClient
 * @lib eswt
 */
NONSHARABLE_CLASS(RSwtClient)
        : public RSessionBase
{
public:
    TInt Connect();
    TInt Execute(const MSwtFunctor& aFunctor) const;
};

#endif // SWTCLIENT_H
