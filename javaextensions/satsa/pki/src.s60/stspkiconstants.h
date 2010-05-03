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


#ifndef STSPKICONSTANTS_H
#define STSPKICONSTANTS_H



// Leave code which maps to the Java side as UserCredentialManagerException
// with reason SE_NOT_FOUND.
const TInt KSTSErrSeNotFound = -4001;

// Leave code which maps to the Java side as UserCredentialManagerException
// with reason SE_NO_KEYS
const TInt KSTSErrSeNoKeys = -4002;

// Leave code which maps to the Java side as IllegalArgumentException
// "already registered"
const TInt KSTSErrAlreadyRegistered = -4003;

// Leave code which maps to the Java side as IllegalArgumentException
// "Invalid CA name"
const TInt KSTSErrInvalidCAName = -4004;

// Leave code which maps to the Java side as IllegalArgumentException
// "Invalid characters in CAName"
const TInt KSTSErrInvalidCharactersInCAName = -4005;


#endif // STSPKICONSTANTS_H
