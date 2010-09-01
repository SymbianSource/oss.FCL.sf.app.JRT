/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.m3g;

import javax.microedition.lcdui.Graphics;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.legacy.ToolkitObserver;


public class Graphics3D
{
    //------------------------------------------------------------------
    // Static data
    //------------------------------------------------------------------

    public static final int ANTIALIAS   = 2;
    public static final int DITHER      = 4;
    public static final int TRUE_COLOR  = 8;

    // M3G 1.1
    public static final int OVERWRITE   = 16;
    //------------------------------------------------------------------
    // Instance data
    //------------------------------------------------------------------

    int handle;

    private Camera camera = null;
    private Vector lights = new Vector();

    private java.lang.Object currentTarget = null;
    private int offsetX, offsetY, hints = 0;
    private boolean depthEnabled = true;

    // this flag is for identification of image target types
    // - True for mutable off-screen images
    // - False for canvas/GameCanvas framebuffer
    private boolean isImageTarget;

    // this flag is for identification if MBX HW accelerator is present
    // - True - MBX is NOT present
    // - False - MBX is present
    private boolean isProperRenderer;

    // NGA specific change.
    // Identifies if 2D content drawn prior M3G content to canvas
    // needs to be uploaded to canvas EGL surface before
    // M3G binds to EGL surface.
    private boolean eglContentValid = false;

    // NGA specific change.
    // Class for storing and comparing
    // clip and viewport rectangles
    private class Rect
    {
        public int x;
        public int y;
        public int width;
        public int height;

        public Rect()
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }

        public Rect(int x, int y, int width, int height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }

        public boolean contains(Rect r)
        {
            return x <= r.x && y <= r.y &&
                   (x + width)  >= (r.x + r.width) &&
                   (y + height) >= (r.y + r.height);
        }
    };

    // NGA specific change.
    // Viewport and clip are saved to Java side
    // for checking if Background is used for clearing
    // entire canvas area.
    private Rect viewport;
    private Rect clip;
    private boolean ngaEnabled = false;
    private boolean pendingGLESRelease = false;


    //------------------------------------------------------------------
    // Constructor(s)
    //------------------------------------------------------------------

    /**
     *
     */
    public static final Graphics3D getInstance()
    {
        Interface instance = Interface.getInstance();
        synchronized (instance)
        {
            if (instance.isShutdownOngoing())
            {
                throw new RuntimeException("Graphics3D closed");
            }
            if (instance.graphics3D == null)
            {
                instance.graphics3D = new Graphics3D();
            }
            return instance.graphics3D;
        }
    }

    /**
     *
     */
    private Graphics3D()
    {
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        this.handle = _ctor(Interface.getHandle());
        _addRef(handle);
        try
        {
            isProperRenderer = _isProperRenderer(Interface.bindEventSource());
        }
        finally
        {
            Interface.releaseEventSource();
        }
        // NGA specific change.
        // Initializes the viewport and clipping rectangles
        this.viewport = new Rect();
        this.clip = new Rect();
        // Initializes NGA status - enabled or disabled
        ngaEnabled = invoker.isNgaEnabled();
    }

    /**
     *
     */
    void doDestroyNotify()
    {
        this.camera = null;
        this.lights = null;
    }

    //------------------------------------------------------------------
    // Public methods
    //------------------------------------------------------------------

    /**
     */
    public void bindTarget(java.lang.Object target)
    {
        bindTarget(target, true, 0);
    }

    /**
     *
     */
    public void bindTarget(java.lang.Object target, boolean depth, int flags)
    {
        synchronized (Interface.getInstance())
        {
            integrityCheck();
            int eventSrcHandle = 0;
            if (currentTarget != null)
            {
                throw new IllegalStateException();
            }

            if (target == null)
            {
                throw new NullPointerException();
            }
            try
            {
                // Bind event source. This need to be released.
                eventSrcHandle = Interface.bindEventSource();
                if (target instanceof Graphics)
                {
                    Graphics g = (Graphics) target;
                    Platform.sync(g);

                    if (g.getClipWidth() > Defs.MAX_VIEWPORT_WIDTH ||
                            g.getClipHeight() > Defs.MAX_VIEWPORT_HEIGHT)
                    {
                        throw new IllegalArgumentException();
                    }

                    offsetX = g.getTranslateX();
                    offsetY = g.getTranslateY();

                    ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();

                    // NGA specific change.
                    if (ngaEnabled)
                    {
                        // If overwrite is set, there is no need
                        // to update EGL surface with 2D content
                        eglContentValid = (flags & OVERWRITE) > 0;

                        // Clip and viewport are stored for later
                        // checks regarding Background clear
                        clip.x = g.getClipX() + offsetX;
                        clip.y = g.getClipY() + offsetY;
                        clip.width = g.getClipWidth();
                        clip.height = g.getClipHeight();

                        viewport.x = clip.x;
                        viewport.y = clip.y;
                        viewport.width = clip.width;
                        viewport.height = clip.height;

                        isImageTarget = _bindGraphics(
                                            eventSrcHandle,
                                            handle,
                                            invoker.graphicsGetHandle(g),
                                            clip.x, clip.y,
                                            clip.width, clip.height,
                                            depth, flags,
                                            isProperRenderer);
                    }
                    else
                    {
                        isImageTarget = _bindGraphics(
                                            eventSrcHandle,
                                            handle,
                                            invoker.graphicsGetHandle(g),
                                            g.getClipX() + offsetX, g.getClipY() + offsetY,
                                            g.getClipWidth(), g.getClipHeight(),
                                            depth, flags,
                                            isProperRenderer);
                    }
                    currentTarget = g;
                }
                else if (target instanceof Image2D)
                {
                    Image2D img = (Image2D) target;

                    offsetX = offsetY = 0;

                    _bindImage(eventSrcHandle, handle, img.handle, depth, flags);
                    currentTarget = img;
                }
                else
                {
                    throw new IllegalArgumentException();
                }
            }
            finally
            {
                // Release event source
                Interface.releaseEventSource();
            }
            hints = flags;
            depthEnabled = depth;
        }
    }

    /**
     *
     */
    public void releaseTarget()
    {
        synchronized (Interface.getInstance())
        {
            integrityCheck();
            if (currentTarget == null)
            {
                return;
            }

            int eventSrcHandle = 0;

            try
            {
                // Bind event source
                eventSrcHandle = Interface.bindEventSource();
                if (currentTarget instanceof Graphics)
                {
                    Graphics g = (Graphics) currentTarget;

                    ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();

                    // NGA specific change.
                    if (ngaEnabled)
                    {
                        updateEglContent();
                    }
                    _releaseGraphics(eventSrcHandle, handle,
                                     invoker.graphicsGetHandle(g), isImageTarget, isProperRenderer);
                }
                else if (currentTarget instanceof Image2D)
                {
                    _releaseImage(eventSrcHandle, handle);
                }
                else
                {
                    throw new Error();
                }
            }
            finally
            {
                currentTarget = null;

                if (ngaEnabled  &&
                        pendingGLESRelease &&
                        eventSrcHandle != 0)
                {
                    pendingGLESRelease = false;
                    _freeGLESResources(eventSrcHandle, handle);
                }

                // Release event source
                Interface.releaseEventSource();
            }
        }
    }

    /**
     *
     */
    public void setViewport(int x, int y, int width, int height)
    {
        integrityCheck();
        if (width <= 0 || height <= 0
                || width  > Defs.MAX_VIEWPORT_DIMENSION
                || height > Defs.MAX_VIEWPORT_DIMENSION)
        {
            throw new IllegalArgumentException();
        }

        // NGA specific change.
        if (ngaEnabled)
        {
            // Update the viewport info.
            viewport.x = x + offsetX;
            viewport.y = y + offsetY;
            viewport.width = width;
            viewport.height = height;
            _setViewport(handle, viewport.x, viewport.y, viewport.width, viewport.height);
        }
        else
        {
            _setViewport(handle, x + offsetX, y + offsetY, width, height);
        }
    }

    /**
     *
     */
    public void clear(Background background)
    {
        integrityCheck();
        if (getTarget() == null)
        {
            throw new java.lang.IllegalStateException(
                "Graphics3D does not have a rendering target");
        }
        // NGA specific change.
        if (ngaEnabled)
        {
            checkBackgroundCleared(background);
        }
        try
        {
            _clear(Interface.bindEventSource(),
                   handle, background != null ? background.handle : 0);
        }
        finally
        {
            Interface.releaseEventSource();
        }
    }

    /**
     *
     */
    public void render(World world)
    {
        integrityCheck();
        // NGA specific change.
        if (ngaEnabled)
        {
            checkBackgroundCleared(world.getBackground());
        }
        try
        {
            _renderWorld(Interface.bindEventSource(), handle, world.handle);
        }
        finally
        {
            Interface.releaseEventSource();
        }
    }

    /**
     *
     */
    public void render(VertexBuffer vertices,
                       IndexBuffer primitives,
                       Appearance appearance,
                       Transform transform)
    {
        // Call rendering method with default visibility
        render(vertices, primitives, appearance, transform, -1);
    }

    /**
     *
     */
    public void render(VertexBuffer vertices,
                       IndexBuffer primitives,
                       Appearance appearance,
                       Transform transform,
                       int scope)
    {

        // null pointer exceptions thrown automatically below
        integrityCheck();

        // NGA specific change.
        if (ngaEnabled)
        {
            updateEglContent();
        }
        try
        {
            _render(Interface.bindEventSource(),
                    handle,
                    vertices.handle,
                    primitives.handle,
                    appearance.handle,
                    transform != null ? transform.matrix : null,
                    scope);
        }
        finally
        {
            Interface.releaseEventSource();
        }
    }

    /**
     *
     */
    public void render(Node node, Transform transform)
    {
        if (!(node instanceof Mesh
                || node instanceof Sprite3D
                || node instanceof Group)
                && node != null)
        {
            throw new IllegalArgumentException();
        }
        integrityCheck();

        // NGA specific change.
        if (ngaEnabled)
        {
            updateEglContent();
        }
        try
        {
            _renderNode(Interface.bindEventSource(),
                        handle,
                        node.handle,
                        transform != null ? transform.matrix : null);
        }
        finally
        {
            Interface.releaseEventSource();
        }
    }

    public void setCamera(Camera camera, Transform transform)
    {
        integrityCheck();
        _setCamera(handle,
                   camera != null ? camera.handle : 0,
                   transform != null ? transform.matrix : null);

        this.camera = camera;
    }

    /**
     */
    public int addLight(Light light, Transform transform)
    {
        integrityCheck();
        int index = _addLight(handle,
                              light.handle,
                              transform != null ? transform.matrix : null);
        if (lights.size() < index + 1)
        {
            lights.setSize(index + 1);
        }
        lights.setElementAt(light, index);
        return index;
    }

    /**
     *
     */
    public void setLight(int index, Light light, Transform transform)
    {
        integrityCheck();
        _setLight(handle,
                  index,
                  light != null ? light.handle : 0,
                  transform != null ? transform.matrix : null);
        lights.setElementAt(light, index);
    }

    /**
     */
    public void resetLights()
    {
        integrityCheck();
        _resetLights(handle);
        lights.removeAllElements();
    }

    /**
     *
     */
    public static final Hashtable getProperties()
    {
        Hashtable props = new Hashtable();

        props.put("supportAntialiasing",          new java.lang.Boolean(
                      _isAASupported(Interface.getHandle())));
        props.put("supportTrueColor",             new java.lang.Boolean(Defs.supportTrueColor));
        props.put("supportDithering",             new java.lang.Boolean(Defs.supportDithering));
        props.put("supportMipmapping",            new java.lang.Boolean(Defs.supportMipmapping));
        props.put("supportPerspectiveCorrection", new java.lang.Boolean(Defs.supportPerspectiveCorrection));
        props.put("supportLocalCameraLighting",   new java.lang.Boolean(Defs.supportLocalCameraLighting));
        props.put("maxLights",                    new java.lang.Integer(Defs.MAX_LIGHTS));
        props.put("maxViewportWidth",             new java.lang.Integer(Defs.MAX_VIEWPORT_WIDTH));
        props.put("maxViewportHeight",            new java.lang.Integer(Defs.MAX_VIEWPORT_HEIGHT));
        props.put("maxViewportDimension",         new java.lang.Integer(Defs.MAX_VIEWPORT_DIMENSION));
        props.put("maxTextureDimension",          new java.lang.Integer(Defs.MAX_TEXTURE_DIMENSION));
        props.put("maxSpriteCropDimension",       new java.lang.Integer(Defs.MAX_TEXTURE_DIMENSION));
        props.put("numTextureUnits",              new java.lang.Integer(Defs.NUM_TEXTURE_UNITS));
        props.put("maxTransformsPerVertex",       new java.lang.Integer(Defs.MAX_TRANSFORMS_PER_VERTEX));

        // Extra properties
        props.put("m3gRelease",                   new java.lang.String("04_wk49"));

        return props;
    }

    /**
     *
     */
    public void setDepthRange(float near, float far)
    {
        integrityCheck();
        _setDepthRange(handle, near, far);
    }

    // M3G 1.1

    public Camera getCamera(Transform transform)
    {
        integrityCheck();
        if (transform != null)
        {
            _getViewTransform(handle, transform.matrix);
        }

        return (Camera) Object3D.getInstance(_getCamera(handle));
    }

    public float getDepthRangeFar()
    {
        integrityCheck();
        return _getDepthRangeFar(handle);
    }

    public float getDepthRangeNear()
    {
        integrityCheck();
        return _getDepthRangeNear(handle);
    }

    public Light getLight(int index, Transform transform)
    {
        integrityCheck();
        if (index < 0 || index >= _getLightCount(handle))
        {
            throw new IndexOutOfBoundsException();
        }

        return (Light) Object3D.getInstance(_getLightTransform(handle,
                                            index,
                                            transform != null ? transform.matrix : null));
    }

    public int getLightCount()
    {
        integrityCheck();
        return _getLightCount(handle);
    }

    public java.lang.Object getTarget()
    {
        return currentTarget;
    }

    public int getViewportHeight()
    {
        integrityCheck();
        return _getViewportHeight(handle);
    }

    public int getViewportWidth()
    {
        integrityCheck();
        return _getViewportWidth(handle);
    }

    public int getViewportX()
    {
        integrityCheck();
        return _getViewportX(handle) - offsetX;
    }

    public int getViewportY()
    {
        integrityCheck();
        return _getViewportY(handle) - offsetY;
    }

    public int getHints()
    {
        return hints;
    }

    public boolean isDepthBufferEnabled()
    {
        return depthEnabled;
    }

    // M3G 1.1 getters END

    //------------------------------------------------------------------
    // Native implementation methods
    //------------------------------------------------------------------

    // Finalization method for Symbian
    final private void registeredFinalize()
    {
        // NOP -- handled via ToolkitObserver
    }

    private void integrityCheck()
    {
        if (Interface.isShutdownOngoing())
        {
            throw new RuntimeException("Graphics3D closed");
        }
    }

    /**
     * NGA specific change.
     * Checks if Background is used for clearing the full graphics target area.
     * If so, the 2D content prior to M3G content does not need to be
     * uploaded to EGL surface.
     */
    private void checkBackgroundCleared(Background bg)
    {
        if (!isCanvasTarget())
        {
            return;
        }
        if (bg != null && bg.isColorClearEnabled() && !eglContentValid)
        {
            Graphics g = (Graphics)currentTarget;
            int graphicsHandle = ToolkitInvoker.getToolkitInvoker().graphicsGetHandle(g);
            int targetWidth  = _getTargetWidth(graphicsHandle);
            int targetHeight = _getTargetHeight(graphicsHandle);
            Rect targetRect = new Rect(0, 0, targetWidth, targetHeight);

            // Checks that target is fully covered by graphics clip and viewport
            eglContentValid = viewport.contains(targetRect) && clip.contains(targetRect);
        }
        if (!eglContentValid)
        {
            updateEglContent();
        }
    }

    /**
     * NGA specific change.
     * Checks if the rendering target is a Graphics instance obtained from LCDUI Canvas.
     * @return true if the target is Canvas Graphics.
     */
    private boolean isCanvasTarget()
    {
        return (currentTarget != null) &&
               (currentTarget instanceof Graphics) &&
               !isImageTarget;
    }

    /**
     * NGA specific change.
     * Notifies the native side that egl content need to be updated
     * thanks to 2d mixture
     */
    private void updateEglContent()
    {
        if (isCanvasTarget() && !eglContentValid)
        {
            Graphics g = (Graphics)currentTarget;
            int graphicsHandle = ToolkitInvoker.getToolkitInvoker().graphicsGetHandle(g);
            try
            {
                _updateEglContent(Interface.bindEventSource(), graphicsHandle);
                eglContentValid = true;
            }
            finally
            {
                Interface.releaseEventSource();
            }
        }
    }

    void freeGraphicsMemory()
    {
        synchronized (Interface.getInstance())
        {
            if (!ngaEnabled)
            {
                return;
            }

            if (currentTarget == null)
            {
                pendingGLESRelease = false;
                try
                {
                    _freeGLESResources(Interface.bindEventSource(), handle);
                }
                finally
                {
                    Interface.releaseEventSource();
                }
            }
            else
            {
                pendingGLESRelease = true;
            }
        }
    }

    private native static int _ctor(int hInterface);
    private native static void _addRef(int hObject);

    private native static int _addLight(int handle,
                                        int hLight,
                                        byte[] transform);

    private native static boolean _bindGraphics(int eventSourceHandle,
            int handle,
            int graphicsHandle,
            int clipX, int clipY,
            int clipW, int clipH,
            boolean depth,
            int hintBits,
            boolean aIsProperRenderer);
    private native static void _bindImage(int eventSourceHandle, int handle, int imgHandle, boolean depth, int hintBits);

    private native static void _releaseGraphics(int eventSourceHandle,
            int handle,
            int graphicsHandle,
            boolean aIsImageTarget,
            boolean aIsProperRenderer);
    private native static void _releaseImage(int eventSourceHandle, int handle);
    private native static void _resetLights(int handle);

    private native static void _clear(int eventSourceHandle, int handle, int hBackground);

    private native static void _render(int eventSourceHandle,
                                       int handle,
                                       int hVtxBuffer,
                                       int hIdxBuffer,
                                       int hAppearance,
                                       byte[] transform,
                                       int scope);
    private native static void _renderNode(int eventSourceHandle, int handle, int hNode, byte[] transform);
    private native static void _renderWorld(int eventSourceHandle, int handle, int hWorld);

    private native static void _setCamera(int handle,
                                          int hCamera,
                                          byte[] transform);
    private native static void _setViewport(int handle,
                                            int x, int y,
                                            int width, int height);
    private native static void _setLight(int handle,
                                         int index,
                                         int hLight,
                                         byte[] transform);
    private native static void _setDepthRange(int handle,
            float near,
            float far);

    // M3G 1.1
    // Maintenance release getters

    private native static void _getViewTransform(int handle,
            byte[] transform);
    private native static int _getCamera(int handle);
    private native static int _getLightTransform(int handle,
            int index,
            byte[] transform);
    private native static int _getLightCount(int handle);
    private native static float _getDepthRangeNear(int handle);
    private native static float _getDepthRangeFar(int handle);
    private native static int _getViewportX(int handle);
    private native static int _getViewportY(int handle);
    private native static int _getViewportWidth(int handle);
    private native static int _getViewportHeight(int handle);

    /* Statistics support, MUST be disabled in official releases! */
    /*
        public native static int getStatistics(int[] statistics);
    */
    private native static boolean _isAASupported(int handle);

    private native static boolean _isProperRenderer(int handle);

    // NGA specific changes.
    private native static int _getTargetHeight(int graphicsHandle);
    private native static int _getTargetWidth(int graphicsHandle);
    private native static void _updateEglContent(int eventSourceHandle,
            int graphicsHandle);
    private native static void _freeGLESResources(int eventSourceHandle,
            int handle);
}
