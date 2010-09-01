/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphicsaccelerator.h>
#include <fbs.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <lcdgdrv.h>
#include "lcdgd.hrh"
#include "lcdgdrvif.h"
#include "lcdtransform.h"
#include "calctransform.h"
#include "collision.h"


#ifdef _DEBUG
TBool DbgValidateImageType(const TImageType& aType);
TBool DbgValidateRendererArray(const TImageRenderer aRendererArray[], TInt aCount);
#endif

/**
 * Factory.
 */
EXPORT_C CLcdGraphicsDriver* CLcdGraphicsDriver::NewL(TDisplayMode aDrawingMode)
{
    return CLcdGraphicsDriverImpl::NewL(aDrawingMode);
}

