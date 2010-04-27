/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java RemCon Observer Panic codes
*
*/


#ifndef PANICS_H
#define PANICS_H

/**
 * Java RemCon panic category. Used as the first parameter in User::Panic().
 */
_LIT(KJavaRemConPanicCategory, "JAVA-REMCON");


/**
 * Java RemCon panic numbers and descriptions.
 */
enum TJavaRemConPanic
{
    EJavaRemConPanicGeneral = 0,
    /**< 0 - General JAVA-REMCON panic; More descriptive panics should be used
       instead of this. */

    EJavaRemConPanicObserverNotRemoved = 1,
    /**< 1 - Indicates that a observer is not removed yet */

    EJavaRemConPanicObserverNotSet = 2,
    /**< 2 - Indicates that a observer is not removed yet */

    EJavaRemConPanicObserverNotFound = 3,
    /**< 3 - Indicates that the observer is not found during removing */
};

#endif // PANICS_H

// End of File
