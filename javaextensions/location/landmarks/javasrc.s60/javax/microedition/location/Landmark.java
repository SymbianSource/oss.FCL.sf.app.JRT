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
* Description:  JSR-179 Location API version 1.0.1 Landmark class
 *
*/


// PACKAGE
package javax.microedition.location;

// IMPORTS
import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.location.LAPIManager;
import com.nokia.mj.impl.location.NativeError;

// CLASS DESCRIPTION
/**
 * JSR-179 Location API version 1.0.1 Landmark class
 *
 * Please refer JSR-179 spec
 */
public class Landmark
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javalocation");
        }
        catch (Exception e)
        {
            //Logger.LOG(Logger.EWMA, Logger.EError, e.toString());
        }

    }

    // Error messages
    // Error if the construction of the object did not succeed
    private static final String UNABLE_TO_CREATE_LANDMARK
    = "Unable to create new landmark";

    // The name is null
    private static final String LANDMARK_NAME_IS_NULL
    = "Landmark name is null";

    // Error occured when storing landmark data
    private static final String UNABLE_TO_STORE_LANDMARK_DATA
    = "Unable to store landmark data";

    // Handle to the native side peer object
    private int iLandmarkHandle = -1;

    // Handle to Location API event source
    private int iFunctionSourceHandle = -1;

    // The store to which this landmark belongs. Note that
    // this can be null if the landmark does not belong to any store
    private LandmarkStore iLandmarkStore;

    private Finalizer mFinalizer;

    // Private landmark data
    private String iLandmarkName;

    private String iLandmarkDescription;

    private QualifiedCoordinates iCoordinates; // Mutable

    private AddressInfo iAddressInfo; // Mutable

    /**
     * Please refer JSR-179 spec
     */
    public Landmark(String aName, String aDescription,
                    QualifiedCoordinates aCoordinates, AddressInfo aAddressInfo)
    {
        iFunctionSourceHandle = LAPIManager.getInstance().getFunctionSourceHandle();
        // Create the native side peer object
        iLandmarkHandle = _createNativePeer(iFunctionSourceHandle);
        // Check that the native side peer object was successfully constructed
        if (iLandmarkHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError(UNABLE_TO_CREATE_LANDMARK);
        }

        // Register this object for finalization
        mFinalizer = registerForFinalization();

        // Store the attributes
        setName(aName);
        setDescription(aDescription);
        setQualifiedCoordinates(aCoordinates);
        setAddressInfo(aAddressInfo);
    }

    Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

    }

    /**
     * Package private constructor for constuction Java side objects
     * from native side peer object handles
     *
     * @param aLandmarkHandle A handle to the native side peer object
     * @param aEventSourceHanlde A handle to the event source
     */
    Landmark(int aLandmarkHandle, int aFunctionSourceHandle)
    {
        iLandmarkHandle = aLandmarkHandle;
        iFunctionSourceHandle = aFunctionSourceHandle;
        // Register this object for finalization
        mFinalizer = registerForFinalization();
    }

    /**
     * Disposes the Landmark native peer objecct, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in
     * the first place.
     */
    final void doFinalize()
    {
        _dispose(iFunctionSourceHandle, iLandmarkHandle);
    }

    /**
     * Returns the name of the landmark. Note that a landmark must always
     * have a name and this function should never return null.
     *
     * The name is retrieved from the native side peer object if it has
     * not been added to the landmark yet (cases when landmarks are read
     * from the native database)
     */
    public String getName()
    {
        // Check that if the name is already known then return it. Otherwise
        // the name is read from the native side peer object.
        if (iLandmarkName == null)
        {
            iLandmarkName = _getName(iFunctionSourceHandle, iLandmarkHandle);
            // The name should never be null as the API specification states
            if (iLandmarkName == null)
            {
                // The most suitable way at this point is to return an empty string
                return "";
            }
        }
        return iLandmarkName;
    }

    /**
     * Returns the description of the landmark. If the description is not available
     * in the Java-side object, it is retrieved from the native landmark
     *
     * @return A description for this landmark, null is there is no
     *         description available
     */
    public String getDescription()
    {
        // Check that if the name is already known then return it. Otherwise
        // the name is read from the native side peer object.
        if (iLandmarkDescription == null)
        {
            // Note that the description can be null so it is ok the return null
            // even if an error occured from the native side
            iLandmarkDescription = _getDescription(iFunctionSourceHandle,
                                                   iLandmarkHandle);
        }
        return iLandmarkDescription;
    }

    /**
     * Returns the coordinates of the landmark. If the coordinates are not available
     * in the Java-side object, those are retrieved from the native landmark
     *
     * @return A coordinate information for this landmark, null is there is no
     *         coordinates available
     */
    public QualifiedCoordinates getQualifiedCoordinates()
    {
        if (iCoordinates == null)
        {
            // Coordinates (latitude and longitude) return values
            // are stored to this array
            double[] coordinates = new double[2];
            // Other coordinate data (altitude, horizontal accuracy and vertical
            // accuracy) return values are stored to this array
            float[] coordinateInfo = new float[3];
            // Note that the description can be null so it is ok the return null
            // even if an error occured from the native side
            int err = _getCoordinates(iFunctionSourceHandle, iLandmarkHandle,
                                      coordinates, coordinateInfo);
            // No errors occured when retrieving coordinates.
            // KErrNotFound indicates null coordinates
            if (err == NativeError.KErrNone)
            {
                iCoordinates = new QualifiedCoordinates(coordinates[0], // Latitude
                                                        coordinates[1], // Longitude
                                                        coordinateInfo[0], // Lltitude
                                                        coordinateInfo[1], // Horr.acc.
                                                        coordinateInfo[2]); // Ver.acc.
            }
        }
        return iCoordinates;
    }

    /**
     * Returns the address information of the landmark. If the address information
     * is not available in the Java-side object, it is retrieved from the native
     * landmark
     *
     * @return An address information for this landmark, null is there is no
     *         address information available
     */
    public AddressInfo getAddressInfo()
    {
        if (iAddressInfo == null)
        {
            String[] addressInfos = _getAddressInfo(iFunctionSourceHandle,
                                                    iLandmarkHandle);
            // null array indicates that the item does not have address information
            if (addressInfos != null && addressInfos.length > 0)
            {
                AddressInfo info = new AddressInfo();
                for (int i = 0; i < addressInfos.length; i++)
                {
                    // There is no need to set null fields to the address information
                    // since it already resets all elements in its constructor
                    if (addressInfos[i] != null)
                    {
                        info.setField(i + 1, addressInfos[i]);
                    }
                }
                // The object was successfully created so it can be
                // added to this class and returned to the caller
                iAddressInfo = info;
            }
        }
        return iAddressInfo;
    }

    /**
     * Sets the name of the landmark. The name must not be null or a
     * NullPointerException will be thrown
     *
     * @param aName The name of the landmark
     */
    public void setName(String aName)
    {
        // The name of the landmark must never be null
        if (aName == null)
        {
            throw new NullPointerException(LANDMARK_NAME_IS_NULL);
        }
        // Note that the actual storing of the name is done when the
        // landmark data is committed to the native side peer object
        // The storing does not have to be immediate because the name
        // cannot be set to null.
        iLandmarkName = aName;
    }

    /**
     * Sets the description of the landmark. The existing description
     * will be overwritten. Null argument is also applicable for description
     *
     * @param aDescription The description of the landmark, null indicates
     *        no description
     */
    public void setDescription(String aDescription)
    {
        int err = _setDescription(iFunctionSourceHandle, iLandmarkHandle,
                                  aDescription);

        checkSetError(err);
        iLandmarkDescription = aDescription;
    }

    /**
     * Sets the QualifiedCoordinates of the landmark.
     * @param New qualified coordinates of the landmark. Null indicates that
     *        the coordiantes will be removed from the landmark
     */
    public void setQualifiedCoordinates(QualifiedCoordinates aCoordinates)
    {
        double[] coordinates = null;
        float[] coordinateInfo = null;
        // null objects mark the this value has been cleared from the landmark
        if (aCoordinates != null)
        {
            // Create an array which holds the coordinates of the landmark
            coordinates = new double[2];
            coordinates[0] = aCoordinates.getLatitude();
            coordinates[1] = aCoordinates.getLongitude();

            // Create an array which holds additional position info
            coordinateInfo = new float[3];
            coordinateInfo[0] = aCoordinates.getAltitude();
            coordinateInfo[1] = aCoordinates.getHorizontalAccuracy();
            coordinateInfo[2] = aCoordinates.getVerticalAccuracy();
        }
        // Set values to the native landmark
        int err = _setCoordinates(iFunctionSourceHandle, iLandmarkHandle,
                                  coordinates, coordinateInfo);
        checkSetError(err);
        iCoordinates = aCoordinates;
    }

    /**
     * Sets the address information to this landmark
     * @param New address information. Null indicates that the address information
     *        will be removed from the landmark
     */
    public void setAddressInfo(AddressInfo aAddressInfo)
    {
        String[] addressInfo = null;

        if (aAddressInfo != null)
        {
            addressInfo = new String[AddressInfo.NUM_FIELDS];
            // Get all address info values from the object and create an array
            for (int i = 0; i < AddressInfo.NUM_FIELDS; i++)
            {
                // Field identifiers start from 1
                addressInfo[i] = aAddressInfo.getField(i + 1);
            }
        }
        // null indicates that the value has been cleared from the landmark
        int err = _setAddressInfo(iFunctionSourceHandle, iLandmarkHandle,
                                  addressInfo);
        checkSetError(err);
        iAddressInfo = aAddressInfo;
    }

    // Package private methods

    /**
     * Associates this landmark to the specified landmark store
     */
    synchronized void associateToStore(LandmarkStore aLandmarkStore)
    {
        iLandmarkStore = aLandmarkStore;
    }

    /**
     * Returns the associated store. null is returned if the landmark
     * does not belong to any store
     */
    synchronized LandmarkStore getAssociatedStore()
    {
        return iLandmarkStore;
    }

    /**
     * Returns the handle to the native side peer object
     */
    synchronized int getHandle()
    {
        return iLandmarkHandle;
    }

    /**
     * Prepares the landmark for saving. The QualifiedCoordinates
     * of this landmark is a mutable object so those must be stored
     * when the landmark is added or updated to the native database
     */
    synchronized void prepareForSave() throws IOException
    {
        // Coordinates are mutable so those must be stored
        // also before the item will be written to the store
        if (iCoordinates != null)
        {
            // Store coordinates once again
            setQualifiedCoordinates(iCoordinates);
        }

        // AddressInfo is a mutable object so those must be stored
        // also before the item will be written to the store
        if (iAddressInfo != null)
        {
            // Store address information once again
            setAddressInfo(iAddressInfo);
        }

        if (iLandmarkName != null)
        {
            // The name can be store at this point since it cannot be set to null
            int error = _setName(iFunctionSourceHandle, iLandmarkHandle,
                                 iLandmarkName);

            if (error != NativeError.KErrNone)
            {
                throw new IOException(UNABLE_TO_STORE_LANDMARK_DATA);
            }
        }
    }

    /**
     * Check landmark data setting error
     */
    private void checkSetError(int aError)
    {
        // Check that the operation was successful
        if (aError != NativeError.KErrNone)
        {
            throw new OutOfMemoryError(UNABLE_TO_STORE_LANDMARK_DATA);
        }
    }

    /**
     * Creates the native peer for this object
     * @param aFunctionSourceHandle Handle to event source
     * @retutn Handle to the native side peer object
     */
    private native int _createNativePeer(int aFunctionSourceHandle);

    /**
     * Returns the name of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @return Returns the name of the landmark. Null is returned if an error
     *         occured
     */
    private native String _getName(int aFunctionSourceHandle, int aLandmarkHandle);

    /**
     * Returns the description of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @return The description of the landmark. Null is returned if the landmark
     *         does not have a description
     */
    private native String _getDescription(int aFunctionSourceHandle,
                                          int aLandmarkHandle);

    /**
     * Returns the coordinates of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @param aCoordinates On return, contains values representing longitude and latitude
     * @param aCoordinateInfo On return, contains values representing altitude,
     *        horizontal accuracy and vertical accuracy
     * @return NativeError.KErrNone if the coordinates were successfully retrieved
     *         NativeError.KErrNotFound if the landmark does not have coordinates
     *         If an error occured, one of the system-wide error codes is returned
     */
    private native int _getCoordinates(int aFunctionSourceHandle,
                                       int aLandmarkHandle, double[] aCoordinates, float[] aCoordinateInfo);

    /**
     * Returns the address information of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @return Array holding the addressinfo of the item
     */
    private native String[] _getAddressInfo(int aFunctionSourceHandle,
                                            int aLandmarkHandle);

    /**
     * Stores the name of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @param aLandmarkName The name of the landmark
     */
    private native int _setName(int aFunctionSourceHandle, int aLandmarkHandle,
                                String aLandmarkName);

    /**
     * Stores the description of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @param aDescription The description of the landmark
     */
    private native int _setDescription(int aFunctionSourceHandle,
                                       int aLandmarkHandle, String aDescription);

    /**
     * Stores the coordinates of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @param aCoordinates The position of the landmark
     * @param aCoordinateInfo Additional information about the position
     */
    private native int _setCoordinates(int aFunctionSourceHandle,
                                       int aLandmarkHandle, double[] aCoordinates, float[] aCoordinateInfo);

    /**
     * Sets the address information of the landmark
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     * @param aAddressInfo The address information of the landmark
     */
    private native int _setAddressInfo(int aFunctionSourceHandle,
                                       int aLandmarkHandle, String[] aAddressInfo);

    /**
     * Disposes the native side peer object
     * @param aFunctionSourceHandle Handle to the native event source
     * @param aLandmarkHandle Handle to the native side peer object
     */
    private native void _dispose(int aFunctionSourceHandle, int aLandmarkHandle);
}

// End of file
