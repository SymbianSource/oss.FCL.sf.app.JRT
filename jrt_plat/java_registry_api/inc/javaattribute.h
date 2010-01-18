/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java Registy API header file
*
*/


#ifndef JAVAATTRIBUTE_H
#define JAVAATTRIBUTE_H


#include <e32base.h>
#include "mjavaattribute.h"

/**
 * This class is used to access Java application attributes.
 * In case of MIDlets, these attributes are defined in
 * Java Application Descriptor (JAD) and JAR manifest files.
 */
namespace Java
{
class CJavaAttribute: public CBase, public MJavaAttribute
{
public:

    IMPORT_C virtual ~CJavaAttribute();

    /**
     * Construct a new attribute using the given name, value and trusted
     *
     * @since S60 v5.0
     * @param aName is attribute name
     * @param aValue is attribute value
     * @param aTrusted is true if attribute can be trusted.
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttribute* NewL(const TDesC& aName, const TDesC& aValue, TBool aTrusted);
    /**
     * Construct a new attribute using the given name, value and trusted
     * and leaves the object on the cleanup stack
     *
     * @since S60 v5.0
     * @param aName is attribute name
     * @param aValue is attribute value
     * @param aTrusted is true if attribute can be trusted
     * @return a new CJavaAttribute object
     */
    IMPORT_C static CJavaAttribute* NewLC(const TDesC& aName, const TDesC& aValue, TBool aTrusted);

    /**
     * from MJavaAttribute
     */
public:
    virtual const TDesC& Name() const;
    virtual const TDesC& Value() const;
    virtual TBool Trusted() const;


private:
    /**
     * ConstructL
     *
     * @since S60 v5.0
     * @param aName
     * @param aValue
     * @param aTrusted
     */
    void ConstructL(const TDesC& aName, const TDesC& aValue, TBool aTrusted);

private:
    /**
     * Attribute name
     */
    HBufC* iName;
    /**
     * Attribute value
     */
    HBufC* iValue;
    /**
     * this value indicate, if attribute is trusted (= signed JAR file +
     * attribute present in JAR manifest) or not
     */
    TBool iTrusted;
};
} // namespace Java

#endif // JAVAATTRIBUTE_H


// End of File
