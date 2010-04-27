/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Access interface to list-specific localization data.
*
*/

#ifndef MPIMLOCALIZATIONDATA_H
#define MPIMLOCALIZATIONDATA_H

// INCLUDES
#include "pimtypes.h"

// CLASS DECLARATION

/**
*  Provides localized strings for the various labels used in the PIM API
*/
class MPIMLocalizationData
{
public: // destructor
    /**
     * Destructor is public virtual in order to allow deletion through
     * M-class
     */
    virtual ~MPIMLocalizationData() { }

public: // New functions

    /**
     * Provides a string label associated with the given field. The caller
     * takes ownership of the returned object.
     *
     * @param aField The field for which the label is being queried.
     *
     * @return String label for the field. The label is locale specific.
     */
    virtual HBufC* GetFieldLabelL(TPIMField aField) = 0;

    /**
     * Provides a string label associated with the given attribute.
     * The caller takes ownership of the returned object.
     *
     * @param aAttribute The attribute for which the label is being queried.
     *
     * @return String label for the attribute. The label is locale specific.
     */
    virtual HBufC* GetAttributeLabelL(TPIMAttribute aAttribute) = 0;

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
    virtual HBufC* GetArrayElementLabelL(TPIMField aStringArrayField,
                                         TPIMArrayElement aArrayElement) = 0;
    /**
     * Provides the name of the list.
     * The caller takes ownership of the returned object.
     *
     * @return the list name.
     */
    virtual HBufC* GetListNameL() = 0;

};



#endif // MPIMLOCALIZATIONDATA_H
