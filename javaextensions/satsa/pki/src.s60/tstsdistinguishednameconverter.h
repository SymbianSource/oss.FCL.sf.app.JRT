/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef TSTSDISTINGUISHEDNAMECONVERTER_H
#define TSTSDISTINGUISHEDNAMECONVERTER_H

//  INCLUDES
#include <e32base.h>





// CONSTANTS
// Formatting of a generated distinguished name
_LIT(KSTSDistinguishedNameFormat, "PKI%Y%M%D%H%T%S%C");

// RFC2253 attribute type constants
_LIT(KSTSX500AttributeCN, "CN");
_LIT(KSTSX500AttributeL, "L");
_LIT(KSTSX500AttributeST, "ST");
_LIT(KSTSX500AttributeO, "O");
_LIT(KSTSX500AttributeOU, "OU");
_LIT(KSTSX500AttributeC, "C");
_LIT(KSTSX500AttributeSTREET, "STREET");
_LIT(KSTSX500AttributeDC, "DC");
_LIT(KSTSX500AttributeUID, "UID");

// FORWARD DECLARATION
class CX500DistinguishedName;
class CX520AttributeTypeAndValue;

// CLASS DECLARATION
/**
*  Provides functions for converting distinguished name formats.
*  This class can read RFC 2253 formatted distinguished names.
*
*/
NONSHARABLE_CLASS(TSTSDistinguishedNameConverter)
{

public: // New functions

    /**
        * C++ default constructor.
        */
    TSTSDistinguishedNameConverter();

    /**
        * Convert distinguished name from RFC2253 format to
        * CX500DistinguishedName object.
        * @param aNameInfo RFC2253 formatted distinguished name
        * @return The distinguished name in a CX500DistinguishedName object.
        */
    CX500DistinguishedName* CreateDNL(const TDesC& aNameInfo);

private: // New functions

    /**
        * Generates a pseudo random element for a distinguished name.
        * Current implementation uses current date and time for the value,
        * future implementation may use WIM serial number.
        * The return value is pushed to cleanup stack
        * @return the element int a CX520AttributeTypeAndValue.
        */
    CX520AttributeTypeAndValue* GenerateDNElementLC();

    /**
        * Parses a single element from given part of distinguished
        * name.
        * @param aSingleElement One element of RFC2253 formatted
        * distinguished name.
        * @return the element in a CX520AttributeTypeAndValue.
        */
    CX520AttributeTypeAndValue* ParseDNElementL(TDesC& aSingleElement);

    /**
        * Creates CX520AttributeTypeAndValue from the given arguments
        * @param aType type of the attribute
        * @param aValue value of the attribute
        */
    CX520AttributeTypeAndValue* CreateDNElementL(
        TDesC& aType, TDesC& aValue);

};


#endif // TSTSDISTINGUISHEDNAMECONVERTER_H

// End of File
