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
* Description:  Base class for all types of positioners
 *
*/


#ifndef CPOSITIONERBASE_H
#define CPOSITIONERBASE_H

//  INCLUDES
#include <lbs.h>
#include <jni.h>
#include "locationfunctionserver.h"

namespace java
{
namespace location
{

//  FORWARD DECLARATIONS
class LocationFunctionServer;

// CLASS DECLARATION
/**
 *  Base class for position request classes.
 */
class CPositionerBase: public CActive
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPositionerBase();

public:
    // New functions

    static void GetQualifiedCoordinates(CPositionerBase* aSelf, JNIEnv* aJni,
                                        jdouble* alatlon, jfloat* aAltHaccVacc, jlong* aTimestamp,
                                        jobjectArray aNmeaData);
    static TInt GetSpeedAndCourse(CPositionerBase* aSelf, jfloat* aSpeedCourse);
    static TInt GetAddressInfo(CPositionerBase* aSelf, JNIEnv* aJni,
                               jobjectArray aAddress);

protected:
    // Constructors

    /**
     * C++ default constructor.
     */
    CPositionerBase(LocationFunctionServer* aFunctionSource);

    /**
     * Symbian 2nd phase constructor.
     */
    void BaseConstructL(RPositionServer& aServer, TPositionModuleId aModuleId,
                        TPositionModuleInfo::TCapabilities aCapabilities);

protected:
    // New functions

    /**
     * UpdatePosition
     * Issues a new position request and set this object to active state
     */
    void UpdatePosition();

    /**
     * HandleBufferSizeErrorL
     * Increases the size of the HPositionGenericInfo if it is
     * not suitable for current location fix. The case might be
     * that the positioning PSY accepts multiple NMEA senteces
     *
     * @param aError Error code returned by S60 Location framework.
     *        Possible buffer size errors are KErrPositionBufferOverflow
     *        and KErrOverflow
     */
    void HandleBufferSizeErrorL(const TInt aError);

private:
    // New functions

    /**
     * CreatePositionInfoL
     * Creates a new HPositionGenericInfo with a requested buffer size
     *
     * @param aBufferSize Size of the buffer allocated to the created object
     * @param aNumFields Maximum number of fields in the buffer
     * @return New position info. Ownership is transferred to the caller
     */
    HPositionGenericInfo* CreatePositionInfoL(const TInt aBufferSize,
            const TInt aNumFields) const;

protected:
    // Data

    LocationFunctionServer* mFunctionServer;
    RPositioner iPositioner;
    HPositionGenericInfo* iPositionInfo;
    // Module capabilities. Used when position info sizeneeds to
    // be expanded
    TPositionModuleInfo::TCapabilities iCapabilities;

};

}
}
#endif // CPOSITIONERBASE_H
// End of File
