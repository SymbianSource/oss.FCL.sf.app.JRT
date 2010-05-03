/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@publishedPartner
@released
*/

#ifndef MDOMAINPOLICYFILEPARSER_H
#define MDOMAINPOLICYFILEPARSER_H

#include <e32base.h>

class RFile;

namespace MIDP
{
class MProtectionDomainTable;

/**
 * Interface to a class used to parse a Domain Policy File, and
 * in doing so, create a list of protection domains contained within.
 */
class MDomainPolicyFileParser
{
public:

    /** Destructor */
    virtual ~MDomainPolicyFileParser() = 0;

    /**
     * Parse a Domain Policy File looking for Protection Domains.
     * @param   aFile
     *          Reference to an RFile object specifying the Domain Policy File.
     * @return  A pointer to a protection domains table, which the caller
     *          is responsible for deleting.
     */
    virtual MProtectionDomainTable* ParseL(RFile& aFile) = 0;
};

inline MDomainPolicyFileParser::~MDomainPolicyFileParser()
{
}

}

#endif // MDOMAINPOLICYFILEPARSER_H
