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
* Description:  ITI enhanced methods to PtiEngine for supporting in java
*
*/


#ifndef JAVAPTIVARIATION_H
#define JAVAPTIVARIATION_H

#include <ptiengine.h>

/**
 * Factory
 */
IMPORT_C void JavaPtiVariationLibEntryL(CPtiEngine& aPtiEngine, TInt aType);

typedef void (*TJavaPtiVariationLibEntry)(CPtiEngine&, TInt);

#endif // JAVAPTIVARIATION_H
