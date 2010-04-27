/*
* Copyright (c) 1997-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of various installation types.
*
*/


#ifndef INSTALLTYPES_H
#define INSTALLTYPES_H

/**
* Enumeration used for identifying the different kind of installations.
*/
enum TInstallType
{
    ESISApplication,
    ESISSystem,
    ESISOption,
    ESISConfiguration,
    ESISPatch,
    ESISPartialUpgrade,
    EMIDLETSuite
};

#endif // INSTALLTYPES_H
