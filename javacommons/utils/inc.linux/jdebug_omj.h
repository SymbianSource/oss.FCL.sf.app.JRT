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

#ifndef JDEBUG_OMJ_H
#define JDEBUG_OMJ_H

#ifdef LOGGING_ON
#include <stdio.h>

#define DEBUG( msg ) printf(msg "\n");
#define DEBUG_STR( msg, string ) printf( msg "\n", string);
#define DEBUG_STR_INT( msg, string, num ) printf(msg "\n", string, num);
#define DEBUG_INT( msg, num ) printf(msg, "\n", num);
#define DEBUG_INT2( msg, num, num2 ) printf(msg "\n", num, num2 );
#define DEBUG_INT3( msg, num, num2, num3 ) printf(msg "\n", num, num2, num3 );
#define DEBUG_INT4( msg, num, num2, num3, num4 ) printf(msg "\n", num, num2, num3, num4);
#define DEBUG_INT5( msg, num, num2, num3, num4, num5) printf(msg "\n", num, num2, num3, num4, num5);
#define DEBUG_INT6( msg, num, num2, num3, num4, num5, num6 ) printf(msg "\n", num, num2, num3, num4, num5, num6 );

#else // LOGGING_ON

#define DEBUG( msg )
#define DEBUG_STR( msg, string )
#define DEBUG_STR_INT( msg, string )
#define DEBUG_INT( msg, num )
#define DEBUG_INT2( msg, num, num2 )
#define DEBUG_INT3( msg, num, num2, num3 )
#define DEBUG_INT4( msg, num, num2, num3, num4 )
#define DEBUG_INT5( msg, num, num2, num3, num4, num5 )
#define DEBUG_INT6( msg, num, num2, num3, num4, num5, num6 )
#endif // LOGGING_ON

#define ERROR( msg ) printf(msg "\n");
#define ERROR_INT( msg, num ) printf(msg "\n", num);
#define ERROR_STR( msg, str ) printf(msg "\n", str);

#endif // JDEBUG_OMJ_H

