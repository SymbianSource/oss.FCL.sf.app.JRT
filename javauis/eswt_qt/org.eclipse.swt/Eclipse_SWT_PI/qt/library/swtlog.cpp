/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
 
#include <QString>
#include <QByteArray>

#include "swtlog.h"

using namespace Java::eSWT;

SWTQT_EXPORT SwtScopeLog::SwtScopeLog( const char* aFunctionName, const SwtLogType& aEnterType, const SwtLogType& aExitType )
    : mExitLogType( aExitType )
    {
    mFunctionName = new QString( aFunctionName );
    SwtDataLog::LogData( "%s", mFunctionName->toLatin1().data(), aEnterType );
    }

SwtScopeLog::SwtScopeLog()
    {
    // Not used
    }

SWTQT_EXPORT SwtScopeLog::~SwtScopeLog()
    {
    if( mFunctionName )
        {
        SwtDataLog::LogData( "%s", mFunctionName->toLatin1().data(), mExitLogType );
        delete mFunctionName;
        mFunctionName = NULL;
        }
    }

