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


#include <e32base.h>
#include "com_nokia_mj_impl_globalindicators_GlobalIndicatorsImpl.h"
#include "CGlobalIndicators.h"

/*
 * Class: com_nokia_mid_ui_GlobalIndicators
 * Method: _setGlobalIndicator
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_globalindicators_GlobalIndicatorsImpl__1setGlobalIndicator(
    JNIEnv*,
    jclass,
    jint aIndicator,
    jint aValue)
{
    return CGlobalIndicators::SetGlobalIndicator(
               (TInt) aIndicator,
               (TInt) aValue);
}

/*
 * Class: com_nokia_mid_ui_GlobalIndicators
 * Method: _getGlobalIndicator
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_globalindicators_GlobalIndicatorsImpl__1getGlobalIndicator(
    JNIEnv*,
    jclass,
    jint aIndicator)
{
    return CGlobalIndicators::GlobalIndicator((TInt) aIndicator);
}
//  End of File
