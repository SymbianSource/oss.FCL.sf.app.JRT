/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java Registy API header file
*
*/

#ifndef APPVERSION_H
#define APPVERSION_H

/**
 * This class is used to represent the version of a Java application.
 * The standard TVersion provided in Symbian OS can't be used because it
 * doesn't allow for major and minor version numbers above 127.
 */
class TAppVersion
{
public:

    /**
     * This constructor creates a 0.0.0 version.
     */
    inline TAppVersion();

    /**
     * This is the copy constructor.
     * @param aAppVersion is the app version that will be copied
     */
    inline TAppVersion(const TAppVersion& aAppVersion);

    /**
     * This constructor creates a version according to the arguments.
     * @param aMajor is the major version number
     * @param aMinor is the minor version number
     * @param aBuild is the micro version number
     */
    inline TAppVersion(TInt aMajor, TInt aMinor, TInt aBuild);

    /**
     * Assignment operator.
     * @param aAppVersion is the app version to be assigned to the object
     */
    inline TAppVersion& operator =(const TAppVersion& aAppVersion);

    /**
     * Equality operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app versions are the same, EFalse otherwise
     */
    inline TBool operator ==(const TAppVersion& aAppVersion) const;

    /**
     * Inequality operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app versions are not the same, EFalse otherwise
     */
    inline TBool operator !=(const TAppVersion& aAppVersion) const;

    /**
     * Less than operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app version to the left of the operator is less
     * than the version to the right, EFalse otherwise
     */
    inline TBool operator <(const TAppVersion& aAppVersion) const;

    /**
     * Greater than operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app version to the left of the operator is
     * greater than the version to the right, EFalse otherwise
     */
    inline TBool operator >(const TAppVersion& aAppVersion) const;

    /**
     * Less or equal operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app version to the left of the operator is
     * less than or equal to the version to the right, EFalse otherwise
     */
    inline TBool operator <=(const TAppVersion& aAppVersion) const;

    /**
     * Greater or equal operator.
     * @param aAppVersion is the app version to be compared with
     * the object's app version
     * @return ETrue if the app version to the left of the operator is
     * greater than or equal to the version to the right, EFalse otherwise
     */
    inline TBool operator >=(const TAppVersion& aAppVersion) const;

public:
    TInt iMajor;
    TInt iMinor;
    TInt iBuild;
};

#include <appversion.inl>

#endif // APPVERSION_H

// End of File
