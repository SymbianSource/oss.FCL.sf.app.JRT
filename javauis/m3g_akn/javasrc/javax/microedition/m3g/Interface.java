/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import java.lang.ref.WeakReference;
import java.util.Hashtable;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.legacy.ToolkitObserverNGAExtension;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.legacy.MemoryUtil;

/**
 * M3G interface object. An interface is automatically created for
 * each MIDlet using the 3D API to keep track of Java-side object
 * lifetimes etc.
 */
class Interface implements ToolkitObserverNGAExtension
{
    //------------------------------------------------------------------
    // Static data
    //------------------------------------------------------------------

    // Common class enumeration for Java and native code

    private static final int ANIMATION_CONTROLLER = 0x01;
    private static final int ANIMATION_TRACK      = 0x02;
    private static final int APPEARANCE           = 0x03;
    private static final int BACKGROUND           = 0x04;
    private static final int CAMERA               = 0x05;
    private static final int COMPOSITING_MODE     = 0x06;
    private static final int FOG                  = 0x07;
    private static final int GROUP                = 0x08;
    private static final int IMAGE_2D             = 0x09;
    private static final int INDEX_BUFFER         = 0x0A;
    private static final int KEYFRAME_SEQUENCE    = 0x0B;
    private static final int LIGHT                = 0x0C;
    private static final int LOADER               = 0x0D;
    private static final int MATERIAL             = 0x0E;
    private static final int MESH                 = 0x0F;
    private static final int MORPHING_MESH        = 0x10;
    private static final int POLYGON_MODE         = 0x11;
    private static final int RENDER_CONTEXT       = 0x12;
    private static final int SKINNED_MESH         = 0x13;
    private static final int SPRITE_3D            = 0x14;
    private static final int TEXTURE_2D           = 0x15;
    private static final int VERTEX_ARRAY         = 0x16;
    private static final int VERTEX_BUFFER        = 0x17;
    private static final int WORLD                = 0x18;

    // Once created, the interface singleton currently remains in
    // memory until VM exit.  By using a WeakReference here, with hard
    // references stored in each object, it could be GC'd when no more
    // objects exist, but that probably isn't worth the extra memory
    // overhead.
    private static Hashtable s_instances = new Hashtable();

    //------------------------------------------------------------------
    // Instance data
    //------------------------------------------------------------------

    private int toolkitHandle;
    // Global handle-to-Object3D map used to both find the Java
    // counterparts of objects returned from the native methods, and
    // keep certain objects from being garbage collected.
    private final Hashtable liveObjects;
    // Indicates shutdown status
    private boolean shutdown;
    // Singleton Graphics3D instance
    Graphics3D graphics3D;
    // Event source native handle
    private int eventSrcHandle;
    // Reference count for the event source
    private int eventSrcHandleRefCount;
    // Handle of the native interface object.
    private int handle;
    private Finalizer mFinalizer;

    static
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javam3g");
    }

    //------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------

    private Interface(int toolkitHandle)
    {
        this.toolkitHandle = toolkitHandle;
        liveObjects = new Hashtable();
        graphics3D = null;
        shutdown = false;
        // initialize event source
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        invoker.addObserver(invoker.getToolkit(), this);
        int UIServerHandle = invoker.getUIEventServerHandle();
        eventSrcHandle = _initEventSource(UIServerHandle);
        eventSrcHandleRefCount = 0;
        // Check init status
        if (eventSrcHandle <= 0)
        {
            throw new OutOfMemoryError();
        }

        handle = _ctor(eventSrcHandle);
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    //------------------------------------------------------------------
    // Package methods
    //------------------------------------------------------------------

    /**
     * Returns the M3G interface instance for the current MIDlet.
     */
    synchronized static final Interface getInstance()
    {
        if (Interface.s_instances == null)
        {
            throw new RuntimeException("Graphics3D closed");
        }
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        int toolkitHandle = invoker.toolkitGetHandle(invoker.getToolkit());

        Object instance = s_instances.get(new Integer(toolkitHandle));
        if (instance == null)
        {
            instance = new Interface(toolkitHandle);
            s_instances.put(new Integer(toolkitHandle), instance);
        }
        return (Interface) instance;
    }

    /**
     * Returns the native handle of the current Interface instance.
     */
    static final int getHandle()
    {
        return getInstance().handle;
    }

    /**
     * Finds an Object3D in the global handle-to-object map. Also
     * removes dead objects (that is, null references) from the map
     * upon encountering them.
     */
    static final Object3D findObject(int handle)
    {
        Interface self = getInstance();
        Integer iHandle = new Integer(handle);
        Object ref = self.liveObjects.get(iHandle);

        if (ref != null)
        {
            Object3D obj = (Object3D)((WeakReference)ref).get();
            if (obj == null)
            {
                self.liveObjects.remove(iHandle);
            }
            return obj;
        }
        else
        {
            return null;
        }
    }

    /**
     * Returns the Java object representing a native object, or
     * creates a new proxy/peer if one doesn't exist yet.
     */
    static final Object3D getObjectInstance(int handle)
    {
        // A zero handle equals null
        if (handle == 0)
        {
            return null;
        }
        // Then try to find an existing Java representative for the
        // object
        Object3D obj = findObject(handle);
        if (obj != null)
        {
            return obj;
        }
        // Not found, create a new Java object. Note that only
        // non-abstract classes can possibly be returned.
        switch (_getClassID(handle))
        {
        case ANIMATION_CONTROLLER:
            return new AnimationController(handle);
        case ANIMATION_TRACK:
            return new AnimationTrack(handle);
        case APPEARANCE:
            return new Appearance(handle);
        case BACKGROUND:
            return new Background(handle);
        case CAMERA:
            return new Camera(handle);
        case COMPOSITING_MODE:
            return new CompositingMode(handle);
        case FOG:
            return new Fog(handle);
        case GROUP:
            return new Group(handle);
        case IMAGE_2D:
            return new Image2D(handle);
        case INDEX_BUFFER:
            return new TriangleStripArray(handle);
        case KEYFRAME_SEQUENCE:
            return new KeyframeSequence(handle);
        case LIGHT:
            return new Light(handle);
            //case LOADER:
        case MATERIAL:
            return new Material(handle);
        case MESH:
            return new Mesh(handle);
        case MORPHING_MESH:
            return new MorphingMesh(handle);
        case POLYGON_MODE:
            return new PolygonMode(handle);
            //case RENDER_CONTEXT:
        case SKINNED_MESH:
            return new SkinnedMesh(handle);
        case SPRITE_3D:
            return new Sprite3D(handle);
        case TEXTURE_2D:
            return new Texture2D(handle);
        case VERTEX_ARRAY:
            return new VertexArray(handle);
        case VERTEX_BUFFER:
            return new VertexBuffer(handle);
        case WORLD:
            return new World(handle);
        default:
            throw new Error();
        }
    }

    /**
     * Registers an object with this interface. The object is added
     * to the global handle-to-object map, and the native finalization
     * callback is set up. The handle of the object must already be
     * set at this point!
     */
    static final void register(Object obj, int handle)
    {
        if (obj == null || handle <= 0)
        {
            throw new RuntimeException("Invalid native handle");
        }
        Platform.registerFinalizer(obj);
        getInstance().liveObjects.put(new Integer(handle),
                                      new WeakReference(obj));
    }

    /**
     * Forces removal of an object from the handle-to-object map.
     */
    static final void deregister(Object obj, int handle)
    {
        Interface instance = null;
        try
        {
            instance = Interface.getInstance();
        }
        catch (Exception e)
        {
            return;
        }
        instance.liveObjects.remove(new Integer(handle));
        try
        {
            Platform.finalizeObject(Interface.bindEventSource(), handle);
        }
        finally
        {
            Interface.releaseEventSource();
        }
    }

    /**
     *  Bind native event source.
     *  @return Event source handle
     */
    static final int bindEventSource()
    {
        Interface instance = Interface.getInstance();
        synchronized (instance)
        {
            if (instance.isShutdownOngoing())
            {
                throw new RuntimeException("Graphics3D closed");
            }
            instance.eventSrcHandleRefCount++;
            return instance.getEventSourceHandle();
        }
    }

    /**
     *  Release native event source
     */
    static final void releaseEventSource()
    {
        Interface instance = Interface.getInstance();
        synchronized (instance)
        {
            instance.eventSrcHandleRefCount--;
            if (instance.eventSrcHandleRefCount <= 0)
            {
                instance.eventSrcHandleRefCount = 0;
                instance.notifyAll();
            }
        }
    }

    /**
     *  Sets shutdown indication flag. Actual native
     *  cleanup occurs when liveObjects count is zero
     */
    static void signalShutdown()
    {
        Interface instance = Interface.getInstance();
        synchronized (instance)
        {
            instance.shutdown = true;
        }
    }

    /**
     * Check if shtudown is ongoing
     * @return <code>true</code> if shutdown is ongoing.
     */
    static boolean isShutdownOngoing()
    {
        Interface instance = Interface.getInstance();
        synchronized (instance)
        {
            return instance.shutdown;
        }
    }

    /**
     * Returns handle for native event source stored
     * in given Interaface instance
     */
    final int getEventSourceHandle()
    {
        return eventSrcHandle;
    }


    //------------------------------------------------------------------
    // Private methods
    //------------------------------------------------------------------

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * @see ToolkitObserver#destroyNotify()
     */
    public void destroyNotify()
    {
        registeredFinalize();
    }

    // Native finalization hook, for Symbian only
    synchronized final private void registeredFinalize()
    {
        while (eventSrcHandleRefCount > 0)
        {
            try
            {
                wait();
            }
            catch (InterruptedException e)
            {
            }
        }
        Interface.signalShutdown();
        if (graphics3D != null)
        {
            graphics3D.doDestroyNotify();
            graphics3D = null;
        }
        if ((eventSrcHandle != 0) && (handle != 0))
        {
            // Finalize M3G interface
            Platform.finalizeInterface(handle, eventSrcHandle);

            // Finalize event source and remove singeleton reference
            _finalizeEventSource(eventSrcHandle);
            Interface.s_instances.remove(new Integer(toolkitHandle));
            Interface.s_instances = null;
            // reset handles
            handle = 0;
            eventSrcHandle = 0;
        }
        MemoryUtil.freeNativeMemory();
    }
    
    /*
     * @see ToolkitObserverNGAExtension#foregroundEvent(boolean)
     */
    public void foregroundEvent(boolean foreground)
    {
        if (graphics3D != null)
        {
            graphics3D.setForeground(foreground);
        }
    }

    // Native constructor
    private static native int _ctor(int eventSrcHandle);

    // Native class ID resolver
    private static native int _getClassID(int hObject);

    // Native event source constructor
    private static native int _initEventSource(int UIServerHandle);

    // Native event source finalization
    private static native void _finalizeEventSource(int eventSrcHandle);
}
