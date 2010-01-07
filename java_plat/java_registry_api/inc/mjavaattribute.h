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


#ifndef MJAVAATTRIBUTE_H
#define MJAVAATTRIBUTE_H


#include <e32base.h>

/**
 * This interface is used to access Java application attributes.
 * In case of MIDlets, these attributes are defined in
 * Java Application Descriptor (JAD) and JAR manifest files.
 */
namespace Java
{
class MJavaAttribute
{
public:
    /**
    * This method returns name of attribute
    *
    * Since S60 v5.0
    */
    virtual const TDesC& Name() const = 0;
    /**
    * This method returns attribute's value
    *
    * Since S60 v5.0
    */
    virtual const TDesC& Value() const = 0;
    /**
    * This method returns if attribut is trusted or not (attribute is trusted if
    * JAR file is signed and attribute is present in JAR manifest)
    *
    * Since S60 v5.0
    */
    virtual TBool Trusted() const = 0;

    virtual ~MJavaAttribute() {};



};
} // namespace Registry

#endif // MJAVAATTRIBUTE_H


// End of File
