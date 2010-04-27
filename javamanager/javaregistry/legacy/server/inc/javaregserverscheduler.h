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
* Description:  javaregserverscheduler definition
*
*/


#ifndef JAVAREGSERVERSCHEDULER_H
#define JAVAREGSERVERSCHEDULER_H

// INCLUDES
#include <e32base.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATIONS
class CJavaRegServer;

// CLASS DECLARATION
NONSHARABLE_CLASS(CJavaRegServerScheduler) :
        public CActiveScheduler
{

public:

    static TInt ThreadStart();
    static void CreateAndRunServerL();
    ~CJavaRegServerScheduler();

private:

    CJavaRegServer* iServer;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGSERVERSCHEDULER_H

// End of File

