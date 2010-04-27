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


#ifndef JAVAOSHEADERS_H
#define JAVAOSHEADERS_H

#include <e32base.h>

#ifndef OS_EXPORT
#define OS_EXPORT EXPORT_C
#endif

#ifndef OS_IMPORT
#define OS_IMPORT IMPORT_C
#endif

#ifndef OS_NONSHARABLE_CLASS
#define OS_NONSHARABLE_CLASS(__x__) NONSHARABLE_CLASS(__x__)
#endif

#ifndef OS_NONSHARABLE_STRUCT
#define OS_NONSHARABLE_STRUCT(__x__) NONSHARABLE_STRUCT(__x__)
#endif

#endif // JAVAOSHEADERS_H
