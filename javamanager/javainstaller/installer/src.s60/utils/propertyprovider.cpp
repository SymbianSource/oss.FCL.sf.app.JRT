/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The JNI code for Java Installer class FileWriter.
*
*/

#include "com_nokia_mj_impl_installer_utils_PropertyProvider.h"
#include "logger.h"
#include "propertylistener.h"

// NAMESPACE DECLARATION
using namespace java::installer;
using namespace java::util;

/*
 * Class:     com_nokia_mj_impl_installer_utils_PropertyProvider
 * Method:    _subscribe
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_PropertyProvider__1subscribe
(JNIEnv */*aEnv*/, jclass, jint aCategory, jint aKey)
{
    TUid uid = TUid::Uid(aCategory);
    TUint key = (TUint)aKey;
    // Construct PropertyListener.
    CPropertyListener* propertyListener = 0;
    TRAPD(err, propertyListener = CPropertyListener::NewL(uid, key));
    if (KErrNone != err)
    {
        ELOG3(EJavaInstaller,
              "PropertyProvider__1subscribe: creating property listener "
              "failed, uid: 0x%x, key: 0x%x, err=%d", uid.iUid, key, err);
        return err;
    }

    // Return handle to session. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(propertyListener)>>2;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_PropertyProvider
 * Method:    _processEvents
 * Signature: (ILcom/nokia/mj/impl/installer/utils/PropertyProvider;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_PropertyProvider__1processEvents
  (JNIEnv *aEnv, jclass, jint aHandle, jobject aProvider)
{
    CPropertyListener* propertyListener =
        reinterpret_cast<CPropertyListener*>(aHandle<<2);
    // Start propertyListener for event sending.
    TRAPD(err, propertyListener->ProcessEventsL(aEnv, aProvider));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "PropertyProvider__1processEvents: initializing "
              "propertyListener failed, err=%d", err);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_PropertyProvider
 * Method:    _unsubscribe
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_PropertyProvider__1unsubscribe
  (JNIEnv *, jclass, jint aHandle)
{
    CPropertyListener* propertyListener =
        reinterpret_cast<CPropertyListener*>(aHandle<<2);

    // Delete PropertyListener.
    delete propertyListener;
    propertyListener = 0;

    return KErrNone;
}
