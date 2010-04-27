/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef METADATAFILEHANDLER_H
#define METADATAFILEHANDLER_H

#include "javaosheaders.h"
#include <string>

const int STATE_DELETED = 1;
const int STATE_DISABLED = 2;
const int STATE_ENABLED = 3;
const int STATE_UNDEFINED = -1;

namespace java
{
namespace security
{

OS_NONSHARABLE_CLASS(JavaCertStoreMetadataFileHandler)
{
public:
    static bool writeState(std::string aFilePath, int aState);
    static int readState(std::string aFilePath);
};

} //end namespace security
} //end namespace java

#endif // METADATAFILEHANDLER_H
