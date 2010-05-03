/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for holding label information
*
*/


#ifndef CPIMLABELPROVIDER_H
#define CPIMLABELPROVIDER_H

// EXTERNAL INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class TResourceReader;

/**
 * Class for holding label information
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CPIMLabelProvider) : public CBase
{
public: // Static constructors

    /**
     * Static two-phase constructor
     * Creates an instance of this class
     *
     * @param aReader Resource reader which is used in creation
     * @return An instance of this class
     */
    static CPIMLabelProvider* NewL(TResourceReader& aReader);

    /**
     * Static two-phase constructor
     * Creates an instance of this class
     *
     * @param aReader Resource reader which is used in creation
     * @return An instance of this class. The newly created instance
     *         is left to the cleanup stack
     */
    static CPIMLabelProvider* NewLC(TResourceReader& aReader);

    /**
     * Destructor
     */
    virtual ~CPIMLabelProvider();

public: // New methods

    /**
     * Returns the label of the requested identifier
     * The identifier can be field or attribute id
     *
     * @param aId Identifier of the requested label
     * @return Label. The caller takes ownership of the value. NULL
     *         is returned if the id doesn't match
     */
    HBufC* LabelL(const TInt aId) const;

    /**
     * Returns the label of the requested attribute
     *
     * @param aAttribute Attribute which label is to be retrieved
     * @return Label of the attribute. NULL is returned if the id
     *         or element doesn't match
     */
    HBufC* LabelL(const TInt aId, const TInt aElement) const;

private: // Constructors

    /**
     * Default C++ constructor
     */
    CPIMLabelProvider();

    /**
     * Second phase constructor which constructs this class from a
     * resource file. The orginal resource file is needed in the construction
     *
     * @param aResourceFile Resource file which countains resources for
     *        this provider object
     * @param aReader Resource reader from which the data is read
     */
    void ConstructFromResourceL(
        TResourceReader& aReader);

private: // Data

    // Identifier of the label
    TInt iId;
    // Label. Owned
    HBufC* iLabel;
    // Sub-element identifiers. Owned
    CArrayFix< TInt >* iSubElementIds;
    // Sub-element labels. Owned
    CDesCArray* iSubElementLabels;
};

#endif // CPIMLABELPROVIDER_H

// End of file
