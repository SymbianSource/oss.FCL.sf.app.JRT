/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of Scheme handler interface implementation for
*   "javaapp:" URI scheme
*
*/

#ifndef JAVAAPPHANDLER_H
#define JAVAAPPHANDLER_H

// INCLUDES
#include <schemehandler.h>
#include <e32base.h>

namespace java
{

namespace javaapphandler
{

/**
*  Scheme Handler IF implementation class for javaapp: uri scheme
*
*/
NONSHARABLE_CLASS(CJavaAppHandler) : public CSchemeHandler
{

public:     // Constructors and destructor

    /**
    * Two phased constructor. Leaves on failure.
    * @param - aUrl
    * @return The created object.
    */
    static CJavaAppHandler* NewL(const TDesC& aUrl);

    /**
     * Destructor.
     */
    virtual ~CJavaAppHandler();

private: // Constructors

    /**
    * Constructor.
    */
    CJavaAppHandler();

    /**
    * Second phase constructor. Leaves on failure.
    * @param - aUrl
    */
    void ConstructL(const TDesC& aUrl);

public:

    /**
    * Url Handler with embedding.
    * All applications are always started standalone.
    * @param -
    */
    void HandleUrlEmbeddedL();

    /**
    * Url Handler without embedding
    * @param -
    */
    void HandleUrlStandaloneL();

    /**
    * Observer is not supported. Does nothing.
    * @param - aSchemeDoc
    */
    void Observer(MAknServerAppExitObserver* aSchemeDoc);

protected: // Data

    // Results of parsing
    HBufC* mParsedUrl; ///< Owned.
};

} // javaapphandler

} // java

#endif // JAVAAPPHANDLER_H
