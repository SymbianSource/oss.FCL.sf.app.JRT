/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for all localization data objects.
*
*/


#ifndef CPIMLOCALIZATIONDATA_H
#define CPIMLOCALIZATIONDATA_H

// INCLUDES
#include <e32base.h>
#include "pimtypes.h"
#include "mpimlocalizationdata.h"

class TResourceReader;
class RResourceFile;
class CPIMLabelProvider;

// CLASS DECLARATION
/**
 * Class for all localization data objects.
 * Provides common methods for accessing data in the localized resource files.
 */
NONSHARABLE_CLASS(CPIMLocalizationData) : public CBase,
        public MPIMLocalizationData
{
public: // Construction and destruction

    /**
     * NewL
     * Symbian two-phase constructor
     * @param aReader Resource reader associated to lists resources
     * @param aSubType Subtype of the list
     * @return New instance of this class
     */
    static CPIMLocalizationData* NewL(
        RResourceFile& aResourceFile,
        TResourceReader& aReader,
        TInt aSubType);

    /**
     * NewLC
     * Symbian two-phase constructor
     * @param aReader Resource reader associated to lists resources
     * @param aSubType Subtype of the list
     * @return New instance of this class. The item is left
     *         to the cleanup stack
     */
    static CPIMLocalizationData* NewLC(
        RResourceFile& aResourceFile,
        TResourceReader& aReader,
        TInt aSubType);

    /**
     * Destructor.
     */
    virtual ~CPIMLocalizationData();

public: // From MPIMLocalizationData

    /**
     * Provides a string label associated with the given field. The caller
     * takes ownership of the returned object.
     *
     * @param aField The field for which the label is being queried.
     *
     * @return String label for the field. The label is locale specific.
     */
    HBufC* GetFieldLabelL(TPIMField aField);

    /**
     * Provides a string label associated with the given attribute.
     * The caller takes ownership of the returned object.
     *
     * @param aAttribute The attribute for which the label is being queried.
     *
     * @return String label for the attribute. The label is locale specific.
     */
    HBufC* GetAttributeLabelL(TPIMAttribute aAttribute);

    /**
     * Provides a string label associated with the given array element.
     * The caller takes ownership of the returned object.
     *
     * @param aStringArrayField The field which has a EPIMFieldStringArray
     * data type.
     * @param aArrayElement The element in the array.
     *
     * @return String label for the array element
     */
    HBufC* GetArrayElementLabelL(TPIMField aStringArrayField,
                                 TPIMArrayElement aArrayElement);
    /**
     * Provides the name of the list.
     * The caller takes ownership of the returned object.
     *
     * @return the list name.
     */
    HBufC* GetListNameL();

private: // Constructors

    /**
     * Default C++ constructor
     */
    CPIMLocalizationData(TInt aSubType);

    /**
     * Second phase constructor.
     * Constructs this class from a resource structure
     * @param aReader Resource reader pointed to a resource file
     */
    void ConstructFromResourceL(
        RResourceFile& aResourceFile,
        TResourceReader& aReader);

private: // New methods

    /**
     * Reads list label information from a resource structure
     * @param aReader Resource reader pointed to a resource file
     */
    void ReadListInfoFromResourceL(
        RResourceFile& aResourceFile,
        TResourceReader& aReader);

private: // Data

    // Subtype of the list
    TInt iSubType;
    // List name. Owned
    HBufC* iListName;
    // Field labels. Owned
    RPointerArray< CPIMLabelProvider > iFields;
    // Attribute labels. Owned
    RPointerArray< CPIMLabelProvider > iAttributes;
};

#endif // CPIMLOCALIZATIONDATA_H

// End of file
