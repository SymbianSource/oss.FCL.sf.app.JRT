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
* Description:  CJavaAttributes header file
*
*/


#ifndef JAVAATTRIBUTES_H
#define JAVAATTRIBUTES_H


#include <e32base.h>
#include <s32strm.h>

#include "javaattribute.h"

using namespace Java;

namespace Java
{
namespace Manager
{
namespace Registry
{
class CJavaAttributes: public CBase
{
public:
    /**
     * Construct a new attributes array object
     * @since S60 v5.0
     *
     *
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttributes* NewL();

    /**
     * Construct a new attributes array object and fill array form serialized
     * buffer
     * @since S60 v5.0
     *
     * @param aBuffer with serialized CJavaAttributes object
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttributes* NewL(TDesC8* aBuffer);

    /**
     * Construct a new attributes array object and leaves the object on
     * the cleanupstack
     * @since S60 v5.0
     *
     *
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttributes* NewLC();

    /**
     * Construct a new attributes array object, fill array form serialized
     * buffer and leaves the object on the cleanupstack
     * @since S60 v5.0
     *
     *
     * @param aBuffer with serialized CJavaAttributes object
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttributes* NewLC(TDesC8* aBuffer);

    /**
     * Destructor deletes all attributes stored in iAttributes
     * @since S60 v5.0
     *
     */
    virtual ~CJavaAttributes();

    /**
     * Returns number of stored attributes
     * @since S60 v5.0
     *
     *
     * @return number of attributes in array
     */
    IMPORT_C TInt Count() const;

    /**
     * Returns reference to object with requested Attribute.
     * @since S60 v5.0
     *
     *
     * @param aName: name of attribute
     * @return MJavaAttribute&; Ownership handed over
     */
    IMPORT_C const MJavaAttribute* Attribute(const TDesC& aName) const;

    /**
     * Returns copy of attributes storage
     * @since S60 v5.0
     *
     *
     * @return RPointerArray<MJavaAttributes>
     */
    IMPORT_C  const RPointerArray< MJavaAttribute >& Attributes() const;

    /**
     * Add attribute to array.
     * @since S60 v5.0
     *
     *
     * @param MJavaAttribute*, Ownership gotten.
     * @return position of added attribute in array
     */
    IMPORT_C void AddAttributeL(MJavaAttribute* aAttribute);

    /**
     * Store attribute to MJavaAttribute and add to array.
     * @since S60 v5.0
     *
     *
     * @param aName
     * @param aValue
     * @param aTrusted
     * @return position of added attribute in array
     */
    IMPORT_C void AddAttributeL(const TDesC& aName, const TDesC& aValue, TBool aTrusted);

    /**
    * Delete all attributes
    *
    * @since S60 v5.0
    */
    IMPORT_C void DeleteAttributes();

    /**
    * Returns the size needed for serialization.
    * @since S60 v5.0
    *
    * @return size needed for serialization
    * @see SerializedPropertiesL
    */
    IMPORT_C TInt Size() const;

    /**
     * Returns serialized CJavaAttributes object
     * @since S60 v5.0
     *
     *
     * @param aName
     * @param aValue
     * @param aTrusted
     * @return HBufC8* with serialized object, ownership handed over.
     */
    IMPORT_C HBufC8* SerializedAttributesL() const;

public:
    /**
    * Externalizes the attribete objects. It is used by "<<" operator.
     * @since S60 v5.0
     *
    *
    * @param aStream externalization happens in this stream
    */
    virtual void ExternalizeL(RWriteStream& aStream) const;

    /**
    * Internalizes the attribute objects. It is used by ">>" operator.
    * @since S60 v5.0
    *
    * @param aStream internalization happens from this stream
    */
    virtual void InternalizeL(RReadStream& aStream);

private:
    /**
     * ConstructL does deserialize attributes, if a buffer exists
     * @since S60 v5.0
     *
     *
     * @param aBuffer with serialized CJavaAttributes object
     */
    void ConstructL(TDesC8* aBuffer);

private:
    /**
     * Array of atributes objects
     * @since S60 v5.0
     *
     */
    RPointerArray< MJavaAttribute > iAttributes;

};
} // namespace Registry
} // namespace Manager
} // namespace Java

#endif // JAVAATTRIBUTES_H


// End of File
