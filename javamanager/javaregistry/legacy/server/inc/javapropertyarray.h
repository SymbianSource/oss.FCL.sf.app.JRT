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
* Description:  javapropertyarray definition
*
*/


#ifndef JAVAPROPERTYARRAY_H
#define JAVAPROPERTYARRAY_H

#include <e32base.h>
#include <s32strm.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

class CJavaProperty;

/**
 * This class is a container of CJavaProperty objects references.
 * The class can externalize and internalize the contained CJavaProperty
 * objects.
 *
 * @since S60 v3.2
 */
class CJavaPropertyArray :
        public CBase
{

public:

    /**
     * Static method creates a new object of this class.
     *
     * @since S60 v3.2
     * @return a new object of this class.
     */
    static CJavaPropertyArray*  NewL();

    /**
     * Static method creates a new object of this class and
     * leaves it on cleanup stack.
     *
     * @since S60 v3.2
     * @return a new object of this class.
     */
    static CJavaPropertyArray*  NewLC();

    /**
     * Static method creates a new object of this class and initialize it
     * by creating JavaProperty objects. For initialization of this class
     * is used the buffer obtained by serialization of another object of
     * this class. This object will be a clone of the other object from
     * which the buffer is obtained. The created JavaProperty objects are
     * owned by the CJavaPropertyArray object.
     *
     * @since S60 v3.2
     * @see SerializedPropertiesL()
     * @param aBuffer buffer obtained on serialization of another object
     *          of this class,
     */
    static CJavaPropertyArray*  NewL(TDesC8* aBuffer);

    /**
     * Static method creates a new object of this class and initialize it
     * by creating JavaProperty objects. For initialization of this class
     * is used the buffer obtained by serialization of another object of
     * this class. This object will be a clone of the other object from
     * which the buffer is obtained. The created JavaProperty objects are
     * owned by the CJavaPropertyArray object.
     *
     * @since S60 v3.2
     * @see SerializedPropertiesL()
     * @param aBuffer buffer obtained on serialization of another object
     *          of this class,
     */
    static CJavaPropertyArray*  NewLC(TDesC8* aBuffer);

    /**
     * Appends a property object reference.
     * The appended property object are not owned by this object.
     *
     * @since S60 v3.2
     * @param aProperty property object reference to be appended to this
     *          container
     * @return error code of the operation
     */
    TInt Append(CJavaProperty* aProperty);

    /**
     * Returns the number of contained property object references.
     *
     * @since S60 v3.2
     * @return number of contained property references
     */
    TInt Count();

    /**
     * Returns the property object reference located at a specified position.
     *
     * @since S60 v3.2
     * @param aIndex the position of the property object reference
     * @return the reference of property object at position aIndex
     */
    CJavaProperty*  At(TInt aIndex);

    /**
     * Internalizes the property objects. It is used by ">>" operator.
     *
     * @since S60 v3.2
     * @param aStream internalization happens from this stream
     */
    virtual void InternalizeL(RReadStream& aStream);

    /**
     * Externalizes the property objects. It is used by "<<" operator.
     *
     * @since S60 v3.2
     * @param aStream externalization happens in this stream
     */
    virtual void ExternalizeL(RWriteStream& aStream) const;

    /**
     * Serializes the property objects in a descriptor buffer. This buffer
     * will be used to reconstruct a new JavaPropertArray object together
     * with its JavaProperty objects.
     *
     * @since S60 v3.2
     * @see NewL( TDesC8* aBuffer )
     * @return a buffer containing the property objects in a serialized form
     */
    HBufC8* SerializedPropertiesL() const;

    /**
     * Returns the size needed for serialization.
     *
     * @since S60 v3.2
     * @return size needed for serialization
     * @see SerializedPropertiesL
     */
    TInt Size() const;

    /**
     * Checks if the given property are contained by the property array.
     *
     * @since S60 v3.2
     * @return true if property array contains the given property
     */
    TBool Contains(CJavaProperty& aProperty) const;

    /**
     * Finds and returns the position of property with the given id.
     *
     * @since S60 v3.2
     * @return the position of the property with the given id or
     *              KErrNotFound if property does not exists
     */
    TInt Find(TInt32 aPropertyId);

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    ~CJavaPropertyArray();

    /**
       * Deletes the contained java property objects
       * and remove them from the container.
       *
     * @since S60 v3.2
     */
    void DeleteProperties();


private:

    void ConstructL(TDesC8* aBuffer);


private:    //members

    // contains the java property refernces
    RPointerArray<CJavaProperty> iProperties;

    // indicates if the properties are owned or not by the property array
    TBool iPropertiesOwned;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAPROPERTYARRAY_H

