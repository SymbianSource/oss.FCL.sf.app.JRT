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

#ifndef CSERVICECENTRENUMRETRIEVERS60IMPL_H
#define CSERVICECENTRENUMRETRIEVERS60IMPL_H

#include <jni.h>
#include "javasmsattributes.h"
namespace java
{
namespace wma
{
/**
 * This class provides utilities to retrieve service centre number.
 * It retrives service centre number fron sms settings if defined
 * else from sms stack.
 */
class CServiceCentreNumRetrieverS60Impl : public CBase
{
public:
    /**
     * This function retrieves service centre number.
     * This functions retrieves SC number from sms settings
     * @param[in] aSmscNum the retrieved SC number is returned through it.
     * @exception leaves with the error value in case of SMSC not defined
     * in sms settings.
     */
    void static retrieveServiceCentreNumberL(TDes& aSmscNum);

};
} //namespace wma
} //namespace java
#endif // CSERVICECENTRENUMRETRIEVERS60IMPL_H

