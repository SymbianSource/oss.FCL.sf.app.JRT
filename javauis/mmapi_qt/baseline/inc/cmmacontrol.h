/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is base interface for all controls.
*
*/


#ifndef CMMACONTROL_H
#define CMMACONTROL_H

// INCLUDES
// #include <mevents.h>
// #include <jutils.h>
#include "mmapiutils.h"
//  CLASS DECLARATION
/**
*   This is base interface for all controls.
*
*
*/
class CMMAControl : public CBase
{
public:
    /**
     * Static getter for Java class name. Java object will be created
     * according to this name. Derived classes defines the class names.
     *
     * @see ClassName
     * @param aControl Control to use.
     * @return Java control class name.
     */
    static const TDesC* ClassNameJni(CMMAControl* aControl);

    /**
     * Sets associated Java object.
     *
     * @param aControl Control to use.
     * @param aControlObject Java side control object.
     */
    static void StaticSetHandle(CMMAControl* aControl,
                                jobject aControlObject);
public:
    /**
     * @see ClassNameJni
     * @return Java control class name.
     */
    virtual const TDesC& ClassName() const = 0;

    /**
     * Return public class name.
     */
    IMPORT_C virtual const TDesC& PublicClassName() const;

    /**
     * Refresh this control.
     */
    virtual void RefreshControl()
    {
        // do nothing
    }

private:
    /**
     * @see StaticSetHandle
     * @param aControlObject Java side control object.
     */
    void SetHandle(jobject aControlObject);

protected:
    /**
     * Java side control object.
     */
    jobject iControlObject;
};

#endif // CMMACONTROL_H
