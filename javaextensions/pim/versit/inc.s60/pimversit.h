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
* Description:  PIM Versit API definitions
 *
*/


#ifndef PIMVERSIT_H
#define PIMVERSIT_H

// PIM Versit tokens for extended Contact fields. These fields
// do not belong to the standard vCard specification field set and
// do not have properties defined
_LIT8(KPIMVersitTokenNICKNAME, "X-NICKNAME");
_LIT8(KPIMVersitTokenDTMF, "X-DTMF");
_LIT8(KPIMVersitTokenWVID, "X-WV-ID");
_LIT8(KPIMVersitTokenASSTTEL, "X-ASSISTANT-TEL");
_LIT8(KPIMVersitTokenSIP, "X-SIP");

// SIP field has subtypes of VOIP, PoC (Push-to-talk) and share view
// these are all extended properties and need their own definitions
_LIT8(KPIMVersitTokenVoip, "VOIP");
_LIT8(KPIMVersitTokenPoc, "POC");
_LIT8(KPIMVersitTokenSwis, "SWIS");

// Strings for mapping CLASS values correctly to vCard/vCalendar and back
_LIT(KPIMClassStringPublic, "PUBLIC");
_LIT(KPIMClassStringPrivate, "PRIVATE");
_LIT(KPIMClassStringConfidential, "CONFIDENTIAL");

// Strings for mapping ToDo completed field to vCalendar and back
_LIT(KPIMStatusCompleted, "COMPLETED");
_LIT(KPIMStatusUncompleted, "NEEDS ACTION");

// String for separating name reading values in vCards
_LIT(KPIMSpaceString, " ");

#endif // PIMVERSIT_H
// End of file
