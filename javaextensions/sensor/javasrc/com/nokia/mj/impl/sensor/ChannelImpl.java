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
* Description:  ChannelImpl implementation
*
*/


package com.nokia.mj.impl.sensor;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.microedition.sensor.Channel;
import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.Condition;
import javax.microedition.sensor.ConditionListener;
import javax.microedition.sensor.Data;
import javax.microedition.sensor.LimitCondition;
import javax.microedition.sensor.ObjectCondition;
import javax.microedition.sensor.RangeCondition;
import javax.microedition.sensor.SensorConnection;

class ChannelImpl implements Channel
{

    private final ChannelInfo iChannelInfo;

    private final SensorConnectionImpl iSensor;

    private ConditionContainer iConditionContainer;

    private int iConnectionNativeHandle = 0;

    private int iChannelId;

    private static final Condition[] NO_CONDITIONS = new Condition[ 0 ];

    private static final String SCHEME_CHANNEL = "channel=";

    private static final String SCHEME_SEP2 = "&";

    // LimitCondition
    private static final String SCHEME_LIMIT = "limit=";

    private static final String SCHEME_OP = "op=";

    // RangeCondition
    private static final String SCHEME_LOWER = "lowerLimit=";

    private static final String SCHEME_LOWER_OP = "lowerOp=";

    private static final String SCHEME_UPPER = "upperLimit=";

    private static final String SCHEME_UPPER_OP = "upperOp=";

    // int values for Limit operators
    private static final int OP_EQUALS = 0;

    private static final int OP_GREATER_THAN = 1;

    private static final int OP_GREATER_THAN_OR_EQUALS = 2;

    private static final int OP_LESS_THAN = 3;

    private static final int OP_LESS_THAN_OR_EQUALS = 4;

    private static final int OP_DISABLED = -1;

    /**
     * Constructor
     * @param aConnection SensorConnection instance of this channel
     * @param aInfo ChannelInfo of this channel
     * @param aNativeConnectionHandle handle to native SensorConnection
     * @param aChannelId native identifier of this channel
     */
    public ChannelImpl(SensorConnectionImpl aConnection, ChannelInfo aInfo,
                       int aNativeConnectionHandle, int aChannelId)
    {
        iChannelInfo = aInfo;
        iSensor = aConnection;
        iConnectionNativeHandle = aNativeConnectionHandle;
        iChannelId = aChannelId;
        iConditionContainer = new ConditionContainer();
    }

    /**
     * Sets a <code>Condition</code> object to be monitored. The
     * {@link ConditionListener#conditionMet} notification returns the
     * <code>Condition</code> object as one parameter, thus adding the same,
     * already met <code>Condition</code> for further monitoring is easy. If
     * the same <code>ConditionListener</code> and <code>Condition</code>
     * instance pair already exists, the method returns silently without making
     * addition.
     *
     * @param aListener the ConditionListener to which the
     *            {@link ConditionListener#conditionMet(SensorConnection, Data, Condition)
     *            ConditionListener#conditionMet} notifications are sent
     * @param aCondition the <code>Condition</code> object to be set
     * @throws NullPointerException if listener or condition is null
     * @throws IllegalStateException if state of the SensorConnection is
     *             STATE_CLOSED, or the SensorConnection is not yet opened
     * @throws IllegalArgumentException if the data type of the channel is
     *             TYPE_INT, or TYPE_DOUBLE and <code>ObjectCondition</code>
     *             is tried to be set, or if data type is TYPE_OBJECT and
     *             <code>LimitCondition</code>, or
     *             <code>RangeCondition</code> is tried to be set.
     */
    public synchronized void addCondition(ConditionListener aListener,
                                          Condition aCondition)
    {
        closeCheck("Adding condition ");
        if (aListener == null)
        {
            throw new NullPointerException("Adding condition failed: listener is null");
        }

        if (aCondition == null)
        {
            throw new NullPointerException(
                "Adding condition failed: condtion is null");
        }

        int dataType = iChannelInfo.getDataType();
        if (dataType == ChannelInfo.TYPE_OBJECT)
        {
            if (aCondition instanceof LimitCondition
                    || aCondition instanceof RangeCondition)
            {
                throw new IllegalArgumentException(
                    "Adding condition failed: numeric condition is not accepted for object type channel");
            }
        }
        else
        {
            if (aCondition instanceof ObjectCondition)
            {
                throw new IllegalArgumentException(
                    "Adding condition failed: ObjectCondition is not accepted for numeric value channel");
            }
        }

        ConditionWrapper wrapper = new ConditionWrapper(aCondition);
        int nativeId = 0;


        if (iConditionContainer.addCondition(aListener, wrapper))
        {
            if (aCondition instanceof LimitCondition)
            {
                LimitCondition lc = (LimitCondition) aCondition;
                nativeId = _addCondition(iConnectionNativeHandle, iChannelId, lc
                                         .getLimit(), -1, getIntOp(lc.getOperator()), OP_DISABLED);
            }
            else if (aCondition instanceof RangeCondition)
            {
                RangeCondition rc = (RangeCondition) aCondition;
                nativeId = _addCondition(iConnectionNativeHandle, iChannelId, rc
                                         .getLowerLimit(), rc.getUpperLimit(), getIntOp(rc
                                                 .getLowerOp()), getIntOp(rc.getUpperOp()));
            }

            if (nativeId < 0)
            {
                throw new Error(
                    "Adding condition failed: Symbian OS error "
                    + nativeId);
            }


            wrapper.setNativeId(nativeId);

            iConditionContainer.updateCustomConditionCount(nativeId);

            // start condition listening, will be ignored if already ongoing
            iSensor.startConditionListening(iConditionContainer.hasCustomConditions() ? 1 : 0);
        }
        // if condition was already present just ignore
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.sensor.Channel#getChannelInfo()
     */
    public ChannelInfo getChannelInfo()
    {
        return iChannelInfo;
    }

    /**
     * Returns Condition objects set for this Channel.
     *
     * @return conditions set for the channel as an array of Condition objects;
     *         a zero-length Condition array if there is none
     */
    public synchronized Condition[] getConditions(ConditionListener aListener)
    {
        if (aListener == null)
        {
            throw new NullPointerException(
                "Getting conditons failed: listener is null");
        }
        return iConditionContainer.getConditions(aListener);
    }

    /**
     * This method is offered for convenience to help registering of
     * SensorConnections to PushRegistry to be monitored. Conditions are set in
     * the query part of the sensor URL. By query part is meant the part coming
     * after question mark. Returned string contains limits and ranges set for
     * the Channel; the sensor URL scheme is defined in {@link SensorConnection}
     *
     * @return the piece of the sensor URL, which contains this Channel and
     *         Conditions, which are set for it. The sensor URL is
     *         order-sensitive, so Channel and its Conditions must be in
     *         sequence before next Channel - not to mix Conditions between
     *         Channels.
     */
    public synchronized String getChannelUrl()
    {
        StringBuffer urlBuf = new StringBuffer();
        Enumeration e = iConditionContainer.getAllConditions(true);
        if (e.hasMoreElements())
        {
            urlBuf.append(SCHEME_CHANNEL
                          + iChannelInfo.getName() + SCHEME_SEP2);
        }
        while (e.hasMoreElements())
        {
            urlBuf.append(getUrl(((ConditionWrapper)e.nextElement()).unwrap()));
            if (e.hasMoreElements())
            {
                urlBuf.append(SCHEME_SEP2);
            }
        }

        return urlBuf.toString();
    }

    /*
     * (non-Javadoc)
     * @see javax.microedition.sensor.Channel#removeAllConditions()
     */
    public synchronized void removeAllConditions()
    {
        closeCheck("Removing all conditons ");
        Enumeration conditions = iConditionContainer.getAllConditions(false);
        while (conditions.hasMoreElements())
        {
            removeCondition((ConditionWrapper)conditions.nextElement(), true);
        }
        iConditionContainer.clear();
        iSensor.stopConditionListening();
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.sensor.Channel#removeCondition(javax.microedition.
     *      sensor.ConditionListener, javax.microedition.sensor.Condition)
     */
    public synchronized void removeCondition(ConditionListener listener,
            Condition condition)
    {
        closeCheck("Removing Conditon ");

        if (listener == null)
        {
            throw new NullPointerException(
                "Removing condition failed: listener is null");
        }
        if (condition == null)
        {
            throw new NullPointerException(
                "Removing condition failed: condition is null");
        }

        ConditionWrapper wrapped =
            iConditionContainer.removeCondition(listener, condition);
        if (wrapped != null)
        {
            removeCondition(wrapped, true);
        }
    }

    /*
     * Helper method for Condition removal
     */
    private void removeCondition(ConditionWrapper aCondition, boolean aNativeRemove)
    {
        if (aCondition.getNativeId() > 0 && aNativeRemove)
        {
            _removeCondition(iConnectionNativeHandle, aCondition.getNativeId());
        }
        else if (!iConditionContainer.hasCustomConditions() && aCondition.getNativeId() <= 0)
        {
            // set mode to not having any conditions
            iSensor.startConditionListening(0);
        }

        if (iConditionContainer.iConditionCount == 0)
        {
            iSensor.stopConditionListening();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.sensor.Channel#removeConditionListener(javax.microedition
     *      .sensor.ConditionListener)
     */
    public synchronized void removeConditionListener(ConditionListener listener)
    {
        closeCheck("Removing ConditionListener ");
        if (listener == null)
        {
            throw new NullPointerException(
                "Removing ConditionListener failed: listener is null");
        }
        Condition[] conditions = iConditionContainer.getConditions(listener);
        for (int i = 0; i < conditions.length; i++)
        {
            removeCondition(listener, conditions[ i ]);
        }
    }

    /**
     * Callback function for native side to inform about met condition
     * @param aNativeId Native condition id
     * @param aValue Data value that meets the condition
     * @param aTimeStamp Timestamp of the value
     */
    protected void conditionMet(int aNativeId, double aValue, long aTimeStamp)
    {
        // Create data object
        Data data = null;
        long[] timeStamps = { aTimeStamp };
        boolean[] validities = { true };
        if (iChannelInfo.getDataType() == ChannelInfo.TYPE_INT)
        {
            int values[] = {(int) aValue };
            data = new DataImpl(iChannelInfo, values, timeStamps, validities);
        }
        else if (iChannelInfo.getDataType() == ChannelInfo.TYPE_DOUBLE)
        {
            double values[] = { aValue };
            data = new DataImpl(iChannelInfo, values, timeStamps, validities);
        }
        else
        {
            // This shouldn't happen
            return;
        }

        Vector metConditions = new Vector();
        synchronized (this)
        {
            Hashtable listenerTable = iConditionContainer.getListeners();
            Enumeration listeners = listenerTable.keys();
            while (listeners.hasMoreElements())
            {
                ConditionListener listener = (ConditionListener) listeners
                                             .nextElement();
                Vector conditionV = (Vector) listenerTable.get(listener);
                Enumeration e = conditionV.elements();
                while (e.hasMoreElements())
                {
                    ConditionWrapper wrapper = (ConditionWrapper) e
                                               .nextElement();
                    if (aNativeId <= 0)
                    {
                        // Non native condition, has to be evaluated
                        if (wrapper.isMet(aValue))
                        {
                            metConditions.addElement(new ConditionCallback(
                                                         listener, wrapper.unwrap()));
                            iConditionContainer.removeCondition(listener,
                                                                wrapper.unwrap());
                            removeCondition(wrapper, false);
                        }
                    }
                    else if (wrapper.getNativeId() == aNativeId)
                    {
                        // Native condition, pass directly to listener
                        ConditionCallback cb = new ConditionCallback(listener,
                                wrapper.unwrap());
                        metConditions.addElement(cb);
                        iConditionContainer.removeCondition(listener, wrapper
                                                            .unwrap());
                        removeCondition(wrapper, false);
                    }
                }
            }
        }
        // Call met condition listeners
        for (Enumeration e = metConditions.elements(); e.hasMoreElements();)
        {
            ConditionCallback cb = (ConditionCallback)e.nextElement();
            cb.call(iSensor, data);
        }
    }

    protected boolean hasConditions()
    {
        return iConditionContainer.iConditionCount > 0;
    }
    /**
     * Container for collecting met condition callbacks
     */
    class ConditionCallback
    {
        private Condition iCondition;
        private ConditionListener iListener;

        public ConditionCallback(ConditionListener aListener,
                                 Condition aCondition)
        {
            super();
            iCondition = aCondition;
            iListener = aListener;
        }

        public void call(SensorConnection aSensor, Data aData)
        {
            iListener.conditionMet(aSensor, aData, iCondition);
        }
    }

    /*
     * Throws exception if sensor connection is closed
     */
    private void closeCheck(String aReason)
    {
        if (iSensor.getState() == SensorConnection.STATE_CLOSED)
        {
            throw new IllegalStateException(
                aReason + "failed: SensorConnection is closed");
        }
    }

    /**
     * Method to get the URL string component for single condition. See the
     * &lt;condition&gt; part of the URL schema.
     *
     * @param aCondition Condition from which the string will be generated
     * @return string presentation of the condition
     */
    private static String getUrl(Condition aCondition)
    {
        if (aCondition instanceof LimitCondition)
        {
            LimitCondition c = (LimitCondition) aCondition;
            return SCHEME_LIMIT + c.getLimit() + SCHEME_SEP2 + SCHEME_OP
                   + c.getOperator();
        }

        if (aCondition instanceof RangeCondition)
        {
            RangeCondition c = (RangeCondition) aCondition;
            return SCHEME_LOWER + c.getLowerLimit() + SCHEME_SEP2
                   + SCHEME_LOWER_OP + c.getLowerOp() + SCHEME_SEP2
                   + SCHEME_UPPER + c.getUpperLimit() + SCHEME_SEP2
                   + SCHEME_UPPER_OP + c.getUpperOp();
        }

        return "";
    }

    private static int getIntOp(String aOp)
    {
        if (Condition.OP_EQUALS.equals(aOp))
            return OP_EQUALS;
        if (Condition.OP_GREATER_THAN.equals(aOp))
            return OP_GREATER_THAN;
        if (Condition.OP_GREATER_THAN_OR_EQUALS.equals(aOp))
            return OP_GREATER_THAN_OR_EQUALS;
        if (Condition.OP_LESS_THAN.equals(aOp))
            return OP_LESS_THAN;
        if (Condition.OP_LESS_THAN_OR_EQUALS.equals(aOp))
            return OP_LESS_THAN_OR_EQUALS;
        return OP_DISABLED;
    }

    /**
     * Adds condition to native side and starts condition listening
     *
     * @param aConnection native SensorConnection handle
     * @param aChannelId channel index
     * @param aLowerLimit limit or lower limit of range
     * @param aUpperLimit upper limit or discarded in case of LimitCondition
     * @param aLowerOp operator type for limit or lower limit in range
     * @param aUpperOp upper limit operator or discarded in case of LimitCondition
     * @return native condition handle or negative value on error
     */
    private native int _addCondition(int aConnection, int aChannelId,
                                     double aLowerLimit, double aUpperLimit, int aLowerOp, int aUpperOp);

    /**
     * Removes condition from native side
     * @param aConnection native SensorConnection handle
     * @param aCondition native condition handle
     */
    private native int _removeCondition(int aConnection, int aCondition);

    /**
     * Container class for conditions and listeners
     *
     */
    class ConditionContainer
    {
        private Hashtable iConditionListeners = new Hashtable();
        private int iCustomConditionCount = 0;
        int iConditionCount = 0;

        public boolean addCondition(ConditionListener aListener,
                                    ConditionWrapper aCondition)
        {
            Vector conditions = null;
            if (iConditionListeners.containsKey(aListener))
            {
                conditions = (Vector)iConditionListeners.get(aListener);
            }
            else
            {
                conditions = new Vector();
                iConditionListeners.put(aListener, conditions);
            }
            if (!conditions.contains(aCondition))
            {
                conditions.addElement(aCondition);
                iConditionCount++;
                return true;
            }
            else
            {
                return false;
            }
        }

        public void updateCustomConditionCount(int aNativeId)
        {
            if (aNativeId <= 0)
            {
                iCustomConditionCount++;
            }
        }


        public void clear()
        {
            iCustomConditionCount = 0;
            iConditionCount = 0;
            iConditionListeners.clear();
        }

        public Condition[] getConditions(ConditionListener aListener)
        {
            Condition[] retVal = null;
            if (iConditionListeners.containsKey(aListener))
            {
                Vector conditions = (Vector)iConditionListeners.get(aListener);
                retVal = new Condition[ conditions.size()];
                for (int i = 0; i < retVal.length; i++)
                {
                    retVal[ i ] = ((ConditionWrapper) conditions
                                   .elementAt(i)).unwrap();
                }
            }
            else
            {
                retVal = NO_CONDITIONS;
            }
            return retVal;
        }

        public Hashtable getListeners()
        {
            return iConditionListeners;
        }

        public Enumeration getAllConditions(boolean aUnique)
        {
            Vector retVal = new Vector();
            Enumeration conditionVectors = iConditionListeners.elements();
            while (conditionVectors.hasMoreElements())
            {
                Vector conditionVector =
                    (Vector)conditionVectors.nextElement();
                Enumeration conditions = conditionVector.elements();
                while (conditions.hasMoreElements())
                {
                    Condition condition = (Condition) conditions.nextElement();
                    if (!aUnique || !retVal.contains(condition))
                    {
                        retVal.addElement(condition);
                    }
                }
            }
            return retVal.elements();
        }


        public ConditionWrapper removeCondition(ConditionListener aListener,
                                                Condition aCondition)
        {
            ConditionWrapper condition = null;
            if (iConditionListeners.containsKey(aListener))
            {
                Vector conditions = (Vector)iConditionListeners.get(aListener);
                Enumeration e = conditions.elements();
                while (e.hasMoreElements())
                {
                    condition = (ConditionWrapper)e.nextElement();
                    if (condition.equals(aCondition))
                    {
                        conditions.removeElement(condition);
                        if (condition.getNativeId() <= 0)
                        {
                            iCustomConditionCount--;
                        }
                        iConditionCount--;
                        break;
                    }
                }
            }
            return condition;
        }

        public boolean hasCustomConditions()
        {
            return iCustomConditionCount > 0;
        }
    }
}
