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
* Description:  MJvmNativePortLayer
*
*/

#ifndef MJVMNATIVEPORTLAYER_H
#define MJVMNATIVEPORTLAYER_H


namespace javaruntime
{

class MJavaFile;

const TUint32 KDesLength = 40;

class MJvmNativePortLayer
{

public:

    /** Destructor */
    virtual ~MJvmNativePortLayer() {}

    /**
     * Gets the object that implements MJavaFile interface. The object
     * allows the vm to make file operations.
     * @return  A pointer to an object that implements MJavaFile interface.
     *          It is responsibility of the vm to delete the object.
     */
    virtual MJavaFile* GetJavaFileL() = 0;

    /**
     * Output the supplied data to the output stream.
     * @param   aData
     *          Descriptor reference containing the data.
     */
    virtual void WriteToStdoutL(const TDesC8& aData) = 0;

    /**
     * Output the supplied data to the output stream.
     * @param   aData
     *          Descriptor reference containing the data.
     */
    virtual void WriteToStderrL(const TDesC8& aData) = 0;

};

/**
 * Gets the object that implements MJvmNativePortLayer interface.
 * @return  A pointer to an object that implements MJvmNativePortLayer
 *          interface. It is responsibility of the vm to delete the object.
 */
IMPORT_C MJvmNativePortLayer* GetPortLayerL();
/**
 * Gets the object that implements MJvmNativePortLayer interface.
 * @return  A pointer to an object that implements MJvmNativePortLayer
 *          interface. It is responsibility of the vm to delete the object.
 */

/**
 * Return the native thread priority corresponding with the Java priority.
 *
 * @param aJavaPriority Java priority. Priority should be
 *          from 0 to 11. If not then the method adjusts it
 *          either to 0 (if <0 ) or to 11 (if > 11).
 * @return  TThreadPriority value
 */
IMPORT_C TThreadPriority GetSingleThreadPriority(const TInt& aJavaPriority);
} // end of namespace javaruntime

#endif // MJVMNATIVEPORTLAYER_H
