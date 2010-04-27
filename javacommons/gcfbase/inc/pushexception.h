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

#ifndef PUSHEXCEPTION_H
#define PUSHEXCEPTION_H

#include <sstream>
#include <exception>
#include "javaosheaders.h"
#include "exceptionbase.h"

namespace java
{
namespace push
{

/**
 * Exception class used in the Push framework.
 */
class OS_IMPORT PushException : public java::util::ExceptionBase
{

public:
    OS_IMPORT PushException(int aStatus,const std::string& aMessage, const std::string& aFile,
                            const std::string& aMethod, int aLine) throw();

    OS_IMPORT PushException(int aStatus,const std::wstring& aMessage, const std::string& aFile,
                            const std::string& aMethod, int aLine) throw();

    OS_IMPORT virtual ~PushException() throw();

    OS_IMPORT PushException(const PushException& x);

private:

    //Not implemented.
    //PushException(const PushException&);
    PushException& operator= (const PushException&);
};


}//end namespace push
}//end namespace java

#endif // PUSHEXCEPTION_H
