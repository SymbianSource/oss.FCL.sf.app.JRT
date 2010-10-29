/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Namespace definitions
*
*/

#ifndef M2GNAMESPACE_H
#define M2GNAMESPACE_H
// INCLUDES

// CONSTANTS

// MACROS

#define M2G_NO_NAMESPACE_SWITCH
#ifndef M2G_NO_NAMESPACE_SWITCH
#define M2G_NS_ /**/ m2g_namespace
#define M2G_NS_START  /**/ namespace M2G_NS_ {
#define M2G_NS_END /**/
}
#define M2G_NS(X) /**/ ::M2G_NS_::X
#define USING_WHOLE_M2G_NS /**/ using namespace ::M2G_NS_;
#define USING_M2G_NS(X) /**/ using ::M2G_NS_::X;
#else
#define M2G_NS_ /**/
#define M2G_NS_START  /**/
#define M2G_NS_END /**/
#define M2G_NS(X) /**/
#define USING_WHOLE_M2G_NS /**/
#define USING_M2G_NS(X) /**/
#endif // !M2G_NO_NAMESPACE_SWITCH

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // M2GNAMESPACE_H
