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


#ifndef CSWTCLIENT_H
#define CSWTCLIENT_H


#include <e32std.h>
#include "mswtclient.h"
#include "swtclient.h"
#include "eswt.h"


class RSwtClient;
class MSwtDisplay;


/**
 * CSwtClient
 * MSwtClient implementation.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtClient)
        : public CBase
        , public MSwtClient
{
public:
    /**
     * Creates new CSwtClient instance. Returned client is attached to
     * CSwtDisplay object existing in TLS. If display does not exists NULL is
     * returned, that can be used to checked if this function is called from
     * right thread with existing display.
     * @return new CSwtClient instance or NULL if not called eSWT main thread.
     */
    static CSwtClient* NewL();
    ~CSwtClient();

private:
    CSwtClient(MSwtDisplay* aDisplay);
    void ConstructL();

public: // From MSwtClient
    TInt Execute(const MSwtFunctor& aFunctor) const;
    MSwtDisplay* Display() const;

private:
    // session owned
    RSwtClient iClient;
    // not owned
    MSwtDisplay* iDisplay;
};

#endif // CSWTCLIENT_H
