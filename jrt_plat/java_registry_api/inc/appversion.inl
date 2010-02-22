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

TAppVersion::TAppVersion()
        : iMajor(0), iMinor(0), iBuild(0)
{
}

TAppVersion::TAppVersion(const TAppVersion& aAppVersion)
        : iMajor(aAppVersion.iMajor), iMinor(aAppVersion.iMinor), iBuild(aAppVersion.iBuild)
{
}

TAppVersion::TAppVersion(TInt aMajor, TInt aMinor, TInt aBuild)
        : iMajor(aMajor), iMinor(aMinor), iBuild(aBuild)
{
}

TAppVersion& TAppVersion::operator =(const TAppVersion& aAppVersion)
{
    // Assignment operator using the copy constructor
    return *new(this) TAppVersion(aAppVersion);
}

TBool TAppVersion::operator ==(const TAppVersion& aAppVersion) const
{
    return ((iMajor == aAppVersion.iMajor) &&
            (iMinor == aAppVersion.iMinor) &&
            (iBuild == aAppVersion.iBuild));
}

TBool TAppVersion::operator !=(const TAppVersion& aAppVersion) const
{
    return !(*this == aAppVersion);
}

TBool TAppVersion::operator <(const TAppVersion& aAppVersion) const
{
    return ((iMajor < aAppVersion.iMajor) ||
            ((iMajor == aAppVersion.iMajor) && (iMinor < aAppVersion.iMinor)) ||
            ((iMajor == aAppVersion.iMajor) && (iMinor == aAppVersion.iMinor) &&
             (iBuild < aAppVersion.iBuild)));
}

TBool TAppVersion::operator >(const TAppVersion& aAppVersion) const
{
    return (aAppVersion < *this);
}

TBool TAppVersion::operator <=(const TAppVersion& aAppVersion) const
{
    return !(*this > aAppVersion);
}

TBool TAppVersion::operator >=(const TAppVersion& aAppVersion) const
{
    return !(*this < aAppVersion);
}
