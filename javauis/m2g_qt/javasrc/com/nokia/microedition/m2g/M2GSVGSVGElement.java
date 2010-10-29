/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g;

import org.w3c.dom.*;
import org.w3c.dom.svg.*;
import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;

public class M2GSVGSVGElement extends M2GSVGLocatableElement implements SVGSVGElement
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------


    //------------------------------------------------------------------
    // VARIABLES
    //------------------------------------------------------------------
    // Exception text
    public static final String SCALE_BY_ZERO =
        "Cannot scale by 0.";

    //------------------------------------------------------------------
    // VARIABLES
    //------------------------------------------------------------------
    private Object iTimeGuard;
    private M2GSVGMatrix iMatrix;
    private M2GSVGPoint iCurrentTranslatePoint;
    private float iCurrentRotate;
    private float iCurrentScale;
    private float iCurrentTimeInSeconds;

    //------------------------------------------------------------------
    // METHODS
    //------------------------------------------------------------------
    /**
     * Constructor
     * @param aHandle -
     * @param aDocument -
     */
    protected M2GSVGSVGElement(int aHandle, M2GDocument aDocument)
    {
        super(aHandle, aDocument);
        iTimeGuard = new Object();
        iMatrix = (M2GSVGMatrix)createSVGMatrixComponents(1f, 0f, 0f, 1f, 0f, 0f);
        iCurrentTranslatePoint = new M2GSVGPoint(this);
        iCurrentRotate = 0.0f;
        iCurrentScale = 1.0f;
        iCurrentTimeInSeconds = 0.0f;
        refreshMatrix();
    }

    /**
     * @see org.w3c.dom.svg.SVGSVGElement#createSVGMatrixComponents()
     */
    public SVGMatrix createSVGMatrixComponents(
        float a, float b, float c, float d, float e, float f)
    {
        return new M2GSVGMatrix(a, b, c, d, e, f);
    }

    /**
       * @see org.w3c.dom.svg.SVGSVGElement#createSVGMatrixComponents()
     */
    public SVGPath createSVGPath()
    {
        return new M2GSVGPath(
                   M2GSVGPath._createPath(getNativeSVGProxyHandle()));
    }

    /**
       * @see org.w3c.dom.svg.SVGSVGElement#createSVGRect()
     */
    public SVGRect createSVGRect()
    {
        return new M2GSVGRect();
    }

    /**
       * @see org.w3c.dom.svg.SVGSVGElement#SVGRGBColor()
     */
    public SVGRGBColor createSVGRGBColor(int red, int green, int blue)
    {
        return new M2GSVGRGBColor(red, green, blue);
    }

    /**
       * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    protected void doCleanup()
    {
        // Note this method is not automatically called if
        // doConstruct() method is not first called
        super.doCleanup();
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#getCurrentRotate()
     */
    public float getCurrentRotate()
    {
        synchronized (this)
        {
            return iCurrentRotate;
        }
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#getCurrentScale()
     */
    public float getCurrentScale()
    {
        synchronized (this)
        {
            return iCurrentScale;
        }
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#getCurrentTime()
     */
    public float getCurrentTime()
    {
        synchronized (iTimeGuard)
        {
            return iCurrentTimeInSeconds;
        }
    }

    /**
       * @see com.nokia.microedition.m2g.M2GObject#getCurrentTranslate()
     */
    public SVGPoint getCurrentTranslate()
    {
        return iCurrentTranslatePoint;
    }

    /**
     * @org.w3c.dom.Node#getParentNode()
    */
    public Node getParentNode()
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "getParentNode() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return null;
    }

    /**
     * Check svg version and base profile
     * @throws IOException If version or profile is invalid
    */
    public void checkVersionAndProfile() throws IOException
    {
        // Check version
        String trait = _getStringTrait(
                           getNativeSVGProxyHandle(),
                           getHandle(),
                           M2GSVGConstants.AT_VERSION);
        if (trait != null && (!trait.equals(
                                  M2GSVGConstants.getInstance().iSvgBaseProfileVersion)))
        {
            Logger.ELOG(Logger.EJavaUI, "doCheckValidity() - excption: svg version");
            throw new IOException(/*SF*/"Invalid version."/*SF*/);
        }
        // Check profile
        trait = _getStringTrait(
                    getNativeSVGProxyHandle(),
                    getHandle(),
                    M2GSVGConstants.AT_BASEPROFILE);
        if (trait != null && (!trait.equals(
                                  M2GSVGConstants.getInstance().iSvgBaseProfileName)))
        {
            Logger.ELOG(Logger.EJavaUI, "doCheckValidity() - excption: svg base profile");
            throw new IOException(/*SF*/"Invalid base profile."/*SF*/);
        }
    }

    /**
     * @javax.microedition.m2g.SVGImage#incrementTime()
     */
    public void incrementTime(float seconds)
    {
        incrementTimeNoUpdate(seconds);
        // update the media time to SVG engine
        updateTime();
    }

    /**
     * Increments the animation or media timeline for this SVGImage (in seconds)
     * but it does not update it to the SVG engine
     *
     * @note the time is updated to the engine during M2GScalableGraphics.RenderFrame()
     * @param seconds the value of time to increment in seconds
     *
     * @since S60 3.1
     */
    public void incrementTimeNoUpdate(float seconds)
    {
        if (seconds < 0)
        {
            throw new IllegalArgumentException();
        }

        if (Float.isNaN(iCurrentTimeInSeconds + seconds))
        {
            throw new IllegalArgumentException();
        }

        synchronized (iTimeGuard)
        {
            iCurrentTimeInSeconds += seconds;
        }
    }

    /**
     * Sets matrix trait
     */
    protected void refreshMatrix()
    {
        float matrixComponents[] = new float[M2GSVGMatrix.ARRAY_SIZE];

        synchronized (this)
        {
            iMatrix.getComponentsToArray(matrixComponents);
        }

        M2GSVGElement._setMatrixTrait(
            getNativeSVGProxyHandle(), getHandle(),
            M2GSVGConstants.AT_TRANSFORM, matrixComponents);

        // inform observer about changes in DOM
        iDocument.notifyDOMChangeObserver();
    }

    /**
     * Updates the time to engine
     */
    private void updateTime()
    {
        float currentTimeInSeconds = 0.0f;

        synchronized (iTimeGuard)
        {
            currentTimeInSeconds = iCurrentTimeInSeconds;
        }

        _setMediaTime(getNativeSVGProxyHandle(),
                      getDocument().getHandle(), currentTimeInSeconds);

        // inform observer about changes in DOM
        iDocument.notifyDOMChangeObserver();
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#setCurrentRotate()
     */
    public void setCurrentRotate(float value)
    {
        String zoomAndPan = getTrait("zoomAndPan");
        if ((zoomAndPan != null) && zoomAndPan.equals("disable"))
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "setCurrentRotate() - zoomAndPan:" + zoomAndPan + ", fail");
            return;
        }
        synchronized (this)
        {
            iCurrentRotate = value;
            updateTransformMatrix();
        }
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#setCurrentScale()
     */
    public void setCurrentScale(float value)
    {
        String zoomAndPan = getTrait("zoomAndPan");
        if ((zoomAndPan != null) && zoomAndPan.equals("disable"))
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "setCurrentScale() - zoomAndPan:" + zoomAndPan + ", fail");
            return;
        }
        if (value == 0)
        {
            throw new DOMException(DOMException.INVALID_ACCESS_ERR, SCALE_BY_ZERO);
        }
        synchronized (this)
        {
            iCurrentScale = value;
            updateTransformMatrix();
        }
    }

    /**
       * @see org.w3c.dom.svg.SVGSVGElement#setCurrentTime()
       * @note no support for setting the time backwards except for setting it to zero
       * @note engine resets the animation if current time is zero
     */
    public void setCurrentTime(float newSeconds)
    {
        synchronized (iTimeGuard)
        {
            if (newSeconds <= iCurrentTimeInSeconds && newSeconds != 0)
            {
                // only forward time or zero time is accepted
                return;
            }

            iCurrentTimeInSeconds = newSeconds;
        }

        updateTime();
    }

    /**
     * <pre>
     *  [currentScale      0       currentTranslate.x]   [cos(currentRotate) -sin(currentRotate 0]
     *  [     0      currentScale  currentTranslate.y] x [sin(currentRotate) cos(currentRotate) 0]
     *  [     0            0               1         ]   [         0                  0         1]
     * </pre>
     */
    public void updateTransformMatrix()
    {
        synchronized (this)
        {
            M2GSVGMatrix a = new M2GSVGMatrix(
                iCurrentScale, 0f, 0f, iCurrentScale,
                iCurrentTranslatePoint.getX(), iCurrentTranslatePoint.getY());
            M2GSVGMatrix b = new M2GSVGMatrix(
                (float)Math.cos(iCurrentRotate *(float)Math.PI / 180.0f),
                (float)Math.sin(iCurrentRotate *(float)Math.PI / 180.0f),
                -(float)Math.sin(iCurrentRotate *(float)Math.PI / 180.0f),
                (float)Math.cos(iCurrentRotate *(float)Math.PI / 180.0f),
                0f, 0f);
            iMatrix = (M2GSVGMatrix)(a.mMultiply(b));
            refreshMatrix();
        }
    }

    //------------------------------------------------------------------
    // STATIC METHODS
    //------------------------------------------------------------------
    /**
     * Builds root element according to a document
     * @param aDocument
     * @return root element
     */
    public static M2GSVGSVGElement buildRootElement(M2GDocument aDocument)
    {
        // Get native handle
        int elementHandle = M2GSVGElement._getRootElement(
                                aDocument.getNativeSVGProxyHandle(),
                                aDocument.getHandle());
        // Check native handle
        if (!M2GObject.checkHandle(elementHandle))
        {
            return null;
        }
        // Create object
        M2GSVGSVGElement rootElement =
            new M2GSVGSVGElement(elementHandle, aDocument);
        if (rootElement != null)
        {
            // Register element
            aDocument.registerLiveElement(rootElement, new Integer(elementHandle));
        }
        return rootElement;
    }

    //------------------------------------------------------------------
    // NATIVE METHODS
    //------------------------------------------------------------------
    protected native static float _getMediaTime(
        int aSvgProxyHandle, int aDocumentHandle);
    private native static void _setMediaTime(
        int aSvgProxyHandle, int aDocumentHandle,
        float aSeconds);
}

