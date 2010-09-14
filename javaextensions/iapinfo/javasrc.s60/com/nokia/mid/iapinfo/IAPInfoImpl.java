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
* Description:  Implementation class of the APNInfo interface
 *
*/


package com.nokia.mid.iapinfo;

import com.nokia.mid.iapinfo.APNControl;
import com.nokia.mid.iapinfo.CommDBException;
import com.nokia.mid.iapinfo.ConnectionPreferencesTable;
import com.nokia.mid.iapinfo.IAPTable;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.*;

import com.nokia.mj.impl.gcf.utils.NativeError;

import java.util.Vector;

/**
 * Implementation class of the APNInfo interface
 */
class IAPInfoImpl extends IAPInfo
{

    private int iSessionHandle;
    static
    {
        try
        {
            Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "loading javaiapinfo");
            Jvm.loadSystemLibrary("javaiapinfo");
            Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "javaiapinfo loaded");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    /**
     * Default constructor. Creates a new IAPInfoImpl object.
     */
    public IAPInfoImpl() throws IAPInfoException
    {
        iSessionHandle = _createSession();
        if (iSessionHandle <0)
            throw new IAPInfoException("Error: symbian os error: "+iSessionHandle);

    }

    /**
     * Please refer IapInfo Spec.
     */
    public synchronized AccessPoint[] getAccessPoints() throws IAPInfoException
    {
        IAPTable iIAPTable = null;
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getAccessPoints()  - this = "+Thread.currentThread().toString() +"hascode= " +Thread.currentThread().hashCode());
        try
        {
            iIAPTable = new IAPTable();
        }
        catch (CommDBException e)
        {
            e.printStackTrace();
            throw new IAPInfoException(e.getMessage());
        }
        AccessPoint[] _ac = null;
        try
        {
            iIAPTable.open(iSessionHandle);
            int _nr_of_access_points = iIAPTable.getRecordCount();
            _ac = new AccessPoint[_nr_of_access_points];
            for (int i = 0; i < _nr_of_access_points; i++)
            {
                AccessPoint _c = new AccessPoint(iIAPTable.iRecordId,
                                                 iIAPTable.iRecordName, iIAPTable.iBearerType,
                                                 iIAPTable.iServiceType);
                _ac[i] = _c;
                if (i != _nr_of_access_points - 1)
                    iIAPTable.nextRecord();
            }
            iIAPTable.close();
        }
        catch (CommDBException _exception)
        {
            iIAPTable.close();
            throw new IAPInfoException(_exception.getMessage());
        }// end of catch
        return _ac;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public synchronized AccessPoint getAccessPoint(int aID) throws IAPInfoException
    {
        AccessPoint _ac = null;
        IAPTable iIAPTable = null;
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getAccessPoint() - aID = "+aID +" this = "+Thread.currentThread().toString() +"hascode= " +Thread.currentThread().hashCode());
        try
        {
            iIAPTable = new IAPTable();
        }
        catch (CommDBException e)
        {
            e.printStackTrace();
            throw new IAPInfoException(e.getMessage());
        }
        try
        {
            iIAPTable.open(iSessionHandle);
            int _ret = iIAPTable.findById(aID);
            if (_ret != IAPTable.RECORD_NOT_FOUND)
            {
                _ac = new AccessPoint(iIAPTable.iRecordId,
                                      iIAPTable.iRecordName, iIAPTable.iBearerType,
                                      iIAPTable.iServiceType);
            }
            iIAPTable.close();
        }
        catch (CommDBException _exception)
        {
            iIAPTable.close();
            throw new IAPInfoException(_exception.getMessage());
        }// end catch
        return _ac;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public synchronized AccessPoint getAccessPoint(String aName) throws IAPInfoException
    {
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getAccessPoint() - aName = "+aName +" this = "+Thread.currentThread().toString() +"hascode= " +Thread.currentThread().hashCode());
        IAPTable iIAPTable = null;
        try
        {
            iIAPTable = new IAPTable();
        }
        catch (CommDBException e)
        {
            e.printStackTrace();
            throw new IAPInfoException(e.getMessage());
        }
        if (aName == null)
        {
            throw new IAPInfoException("Parameter is NULL");
        }
        AccessPoint _ac = null;
        try
        {
            iIAPTable.open(iSessionHandle);
            Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getAccessPoint() - aName = after iaptable.open()");
            int _ret = iIAPTable.findByName(aName);
            if (_ret != IAPTable.RECORD_NOT_FOUND)
            {
                _ac = new AccessPoint(iIAPTable.iRecordId,
                                      iIAPTable.iRecordName, iIAPTable.iBearerType,
                                      iIAPTable.iServiceType);
            }
            iIAPTable.close();
        }
        catch (CommDBException _exception)
        {
            iIAPTable.close();
            throw new IAPInfoException(_exception.getMessage());
        }// end catch
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "-getAccessPoint() - aName = "+aName);
        return _ac;
//return null;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public synchronized AccessPoint getLastUsedAccessPoint() throws IAPInfoException
    {
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getLastUsedAccessPoint() " +" this = "+Thread.currentThread().toString() +"hascode= " +Thread.currentThread().hashCode());
        IAPTable iIAPTable = null;
        try
        {
            iIAPTable = new IAPTable();
        }
        catch (CommDBException e)
        {
            e.printStackTrace();
            throw new IAPInfoException(e.getMessage());
        }
        APNControl _control = new APNControl();
        int _lastIAP = _control.getLastIAP();
        AccessPoint _ac = null;
        int _nr_of_access_points = 0;
        try
        {
            iIAPTable.open(iSessionHandle);
            _nr_of_access_points = iIAPTable.getRecordCount();
            for (int i = 0; i < _nr_of_access_points; i++)
            {
                if (iIAPTable.iRecordId == _lastIAP)
                {
                    _ac = new AccessPoint(iIAPTable.iRecordId,
                                          iIAPTable.iRecordName,
                                          iIAPTable.iBearerType,
                                          iIAPTable.iServiceType);
                    break;
                }// end if
                if (i != _nr_of_access_points - 1)
                {
                    iIAPTable.nextRecord();
                }
            }// end for
            iIAPTable.close();
        }
        catch (CommDBException _exception)
        {
            iIAPTable.close();
            throw new IAPInfoException(_exception.getMessage());
        }
        return _ac;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public DestinationNetwork[] getDestinationNetworks()
    throws IAPInfoException
    {
        DestinationNetwork[] _array = null;
        try
        {
            String[] _obj = _getDestinationNetworks();
            if (_obj != null)
            {
                _array = new DestinationNetwork[_obj.length];
                for (int i = 0; i < _obj.length; i++)
                {
                    _array[i] = parse(_obj[i]);
                }// end for
            }// end if
        }
        catch (IAPInfoException exception)
        { // native code can throw it
            String message = exception.getMessage();
            int i = message.indexOf(':');
            throw new IAPInfoException(Integer.valueOf(
                                           message.substring(i + 2, message.length())).intValue(),
                                       exception.getMessage());
        }
        return _array;
    }

    /**
     * Please refer IapInfo Spec.
     */
    DestinationNetwork parse(String aString)
    {
        int index_1 = aString.indexOf(';');
        int dest_id = Integer.parseInt(aString.substring(0, index_1));
        // get the last ';'
        int index_2 = aString.lastIndexOf(';');
        String dest_name = aString.substring(index_1 + 1, index_2);
        String _iaps = aString.substring(index_2 + 1, aString.length());
        int[] _iap_ids = parseString(_iaps);
        DestinationNetwork _dn = new DestinationNetwork(dest_id, dest_name,
                _iap_ids);
        return _dn;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public DestinationNetwork getDestinationNetwork(int aID)
    throws IAPInfoException
    {
        DestinationNetwork _network = null;
        try
        {
            String _returnedString = _getDestinationNetworkById(aID);
            if (_returnedString != null)
            {
                _network = parse(_returnedString);
            }
        }
        catch (IAPInfoException exception)
        { // native code can throw it
            String message = exception.getMessage();
            int i = message.indexOf(':');
            throw new IAPInfoException(Integer.valueOf(
                                           message.substring(i + 2, message.length())).intValue(),
                                       exception.getMessage());
        }
        return _network;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public DestinationNetwork getDestinationNetwork(String aName)
    throws IAPInfoException
    {
        DestinationNetwork _network = null;
        try
        {
            String _returnedString = _getDestinationNetworkByName(aName);
            if (_returnedString != null)
            {
                _network = parse(_returnedString);
            }
        }
        catch (IAPInfoException exception)
        { // native code can throw it
            String message = exception.getMessage();
            int i = message.indexOf(':');
            throw new IAPInfoException(Integer.valueOf(
                                           message.substring(i + 2, message.length())).intValue(),
                                       exception.getMessage());
        }
        return _network;
    }

    /**
     * Please refer IapInfo Spec.
     */
    public synchronized AccessPoint[] getConnectionPreferences() throws IAPInfoException
    {

        ConnectionPreferencesTable _prefTable = null;
        AccessPoint[] _ac = null;
        IAPTable iIAPTable = null;
        Logger.LOG(Logger.EJavaIapInfo,Logger.EInfo, "+getConnectionPreferences() " +" this = "+Thread.currentThread().toString() +"hascode= " +Thread.currentThread().hashCode());
        try
        {
            iIAPTable = new IAPTable();
        }
        catch (CommDBException e)
        {
            e.printStackTrace();
            throw new IAPInfoException(e.getMessage());
        }

        try
        {
            _prefTable = new ConnectionPreferencesTable();
            _prefTable.open(iSessionHandle);
            int recNr = _prefTable.getRecordCount();

            HelperObject[] hp = new HelperObject[recNr];
            for (int i = 0; i < recNr; i++)
            {
                HelperObject _o = new HelperObject();
                _o.Id = _prefTable.iDefaultIAP;
                _o.Pref = _prefTable.iRanking;
                hp[i] = _o;
                if (i != recNr - 1)
                {
                    _prefTable.nextRecord();
                }
            }
            // sort it
            boolean stop = false;
            while (stop != true)
            {
                stop = true;
                for (int j = 0; j < hp.length - 1; j++)
                {
                    if (hp[j].Id > hp[j + 1].Id)
                    {
                        HelperObject tmp = hp[j];
                        hp[j] = hp[j + 1];
                        hp[j + 1] = tmp;
                        stop = false;
                    }// end if
                }// end for
            }// end while

            // get the AccesssPoints from IAPTable
            iIAPTable.open(iSessionHandle);

            Vector apv = new Vector();
            for (int i = 0; i < recNr; i++)
            {
                // int _ret = iIAPTable.findById( ids[ i ] );
                int _ret = iIAPTable.findById(hp[i].Id);
                if (_ret != IAPTable.RECORD_NOT_FOUND)
                {

                    apv.addElement(new AccessPoint(iIAPTable.iRecordId,
                                                   iIAPTable.iRecordName,
                                                   iIAPTable.iBearerType,
                                                   iIAPTable.iServiceType));
                }// end if
            }// end for

            _ac = new AccessPoint[apv.size()];
            for (int i = 0; i < apv.size(); i++)
            {
                _ac[i] = (AccessPoint) apv.elementAt(i);
            }

            iIAPTable.close();
            _prefTable.close();

        }
        catch (CommDBException e)
        {
            _prefTable.close();
            iIAPTable.close();
            throw new IAPInfoException(e.getMessage());
        }
        return _ac;
    }

    /*
     * Parses the string which contains the access point ids.
     *
     * @param aParameter String
     */
    int[] parseString(String aParameter)
    {
        int[] _returnArray = null;
        // search the separators
        int _index1 = aParameter.indexOf(",");
        if (_index1 == -1)
            return new int[0];
        int _elementCounter = 0;
        int _helper = 0;
        while (_helper != -1)
        {
            _helper = aParameter.indexOf(",", _index1 + 1);
            if (_helper != -1)
            {
                _elementCounter++;
                _index1 = _helper;
            }// end if
        }
        // create the _returnArray;
        _returnArray = new int[_elementCounter + 1];
        // get and add elements
        _index1 = 0;
        _helper = 0;
        int i = 0;
        while (_helper != -1)
        {
            _helper = aParameter.indexOf(",", _index1);
            if (_helper != -1)
            {
                String _substr = aParameter.substring(_index1, _helper);
                _returnArray[i] = Integer.parseInt(_substr);
                _index1 = _helper + 1;
                i++;
            }// end if
        }// end while
        return _returnArray;
    }

    class HelperObject
    {
        int Id;
        int Pref;
    }

    static native String[] _getDestinationNetworks() throws IAPInfoException;
    static native String _getDestinationNetworkById(int aID)
    throws IAPInfoException;
    static native String _getDestinationNetworkByName(String aName)
    throws IAPInfoException;

    private native int _createSession();

}
