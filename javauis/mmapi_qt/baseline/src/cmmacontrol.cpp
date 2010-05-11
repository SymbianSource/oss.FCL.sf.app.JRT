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
* Description:  This is base interface for all controls.
*
*/


//  INCLUDE FILES
#include "cmmacontrol.h"

const TDesC* CMMAControl::ClassNameJni(CMMAControl* aControl)
{
    return &aControl->ClassName();
}

void CMMAControl::StaticSetHandle(CMMAControl* aControl,
                                  jobject aControlObject)
{
    aControl->SetHandle(aControlObject);
}

EXPORT_C const TDesC& CMMAControl::PublicClassName() const
{
    return ClassName();
}

inline void CMMAControl::SetHandle(jobject aControlObject)
{
    iControlObject = aControlObject;
}


//  END OF FILE
