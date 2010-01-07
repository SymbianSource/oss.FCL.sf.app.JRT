/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregproperty definition
*
*/


#ifndef __JAVAREGPROPERTY_H
#define __JAVAREGPROPERTY_H

#include <e32base.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

/**
 * CJavaProperty class contains a property ID and a property
 * value and makes possible to set and get these fields.
 * The property value is represented as a 16 bit descriptor.
 *
 * @since S60 v3.2
 */
class CJavaProperty :
        public CBase
{

public:

    /**
     * Constructs a new property object using the given property
     * ID and value.
     *
     * @since S60 v3.2
     * @param aId java property ID
     * @param aValue java property value
     * @return a new java property object
     */
    IMPORT_C static CJavaProperty*  NewL(TInt32 aId,
                                         const TDesC& aValue);

    /**
     * Constructs a new property object using the given property
     * ID and value
     * and leaves the object on the cleanup stack.
     *
     * @since S60 v3.2
     * @param aId java property ID
     * @param aValue java property value
     * @return a new java property object
     */
    IMPORT_C static CJavaProperty*  NewLC(TInt32 aId,
                                          const TDesC& aValue);

    /**
     * Destructs property object.
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CJavaProperty();

    /**
     * Returns the property Id.
     *
     * @since S60 v3.2
     * @return the property ID
     */
    IMPORT_C TInt32 Id() const;

    /**
     * Returns the property value.
     *
     * @since S60 v3.2
     * @return the property value
     */
    IMPORT_C const TDesC& Value() const;

    /**
     * Compares two objects of this type and returns true if
     * they are identical.
     *
     * @since S60 v3.2
     * @return true if the two objects are identical
     */
    IMPORT_C TBool operator==(const CJavaProperty& aProp);

private:

    /**
     * ConstructL
     *
     * @since S60 v3.2
     * @param aId java property ID
     * @param aValue java property value
     */
    void ConstructL(TInt32 aId, const TDesC& aValue);

private:    //members

    // property ID
    TInt32 iId;

    // property value
    HBufC*  iValue;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif //__JAVAREGPROPERTY_H

