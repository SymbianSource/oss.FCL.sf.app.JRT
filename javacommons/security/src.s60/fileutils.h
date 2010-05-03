/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef FILEUTILS_H
#define FILEUTILS_H

namespace java
{
namespace security
{

class FileUtils
{
public:
    static char * computeDigest(const char*);
    static char * computeDigest(const char*, bool);
};

} //end namespace security
} //end namespace java

#endif // FILEUTILS_H

