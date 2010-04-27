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
* Description:  Platform dependent implementations.
 *
*/


#ifndef PLATFORMIMPL_H
#define PLATFORMIMPL_H

#include <string>

/**
* Will decrement applicable stateful rights such as play counts where
* necessary.
* @param uri Uri for the file.
* @param drmContentId  A unique DRM content id.
* @param status Status string of the operation. Will be empty on success
*               cases, but will contain some understandable error text
*               in failure cases.
* @param startPhase true if starting MIDlet, false if closing.
* @param handle A handle to platform dependent object.
*/
void consumeRigthsImpl(const std::wstring& uri,
                       const std::wstring& drmContentId,
                       std::string& status,
                       bool startPhase,
                       int& handle);


#endif // PLATFORMIMPL_H
