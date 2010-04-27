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
#include "javax_microedition_m3g_Graphics3D.h"

struct BindStruct
{
    int x, y, w, h;
    M3Gbool depth;
    M3Gbitmask hintBits;
};

static void IsProperRenderer(TBool* aIsProperRenderer)
{
    EGLContext ctx;
    EGLConfig config;
    EGLSurface surf;
    EGLint attrib[5];
    EGLint numConfigs;

    // initialize EGL and create a temporary surface & context for reading
    // the renderer string
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);

    attrib[0] = EGL_SURFACE_TYPE;
    attrib[1] = EGL_PBUFFER_BIT;
    attrib[2] = EGL_NONE;

    eglChooseConfig(eglGetDisplay(0), attrib, &config, 1, &numConfigs);

    ctx = eglCreateContext(eglGetDisplay(0), config, NULL, NULL);

    attrib[0] = EGL_WIDTH;
    attrib[1] = 2;
    attrib[2] = EGL_HEIGHT;
    attrib[3] = 2;
    attrib[4] = EGL_NONE;

    surf = eglCreatePbufferSurface(eglGetDisplay(0), config, attrib);
    eglMakeCurrent(eglGetDisplay(0), surf, surf, ctx);

    // We check if proper renderer is used and return value which is stored
    // on java side and passed to fuctions where is decided if m3g renders
    // into mutable off-screen image or into framebuffer (see
    // Java_javax_microedition_m3g_Graphics3D__1bindGraphics and
    // releaseGraphicsTarget).
    const GLubyte *info;
    info = glGetString(GL_RENDERER);   // get the renderer string

    // check if "MBX" substring is found
    if (strstr((const char *)info, "MBX"))
    {
        // HW renderer detected.
        // If "MBX" HW is detected we must reset alpha for mutable off-screen
        // images by hand (see releaseGraphicsTarget).
        *aIsProperRenderer = EFalse;
    }
    else
    {
        // Other renderers can use m3g core API m3gSetAlphaWrite without
        // performance drop.
        *aIsProperRenderer = ETrue;
    }

    // destroy the temporary surface & context
    eglMakeCurrent(eglGetDisplay(0), NULL, NULL, NULL);
    eglDestroySurface(eglGetDisplay(0), surf);
    eglDestroyContext(eglGetDisplay(0), ctx);
}

JNIEXPORT
jboolean JNICALL Java_javax_microedition_m3g_Graphics3D__1isProperRenderer
(JNIEnv* aEnv, jclass, jint aHandle)
{
    TBool isProperRenderer;

    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource =
        JavaUnhand<CJavaM3GEventSource>(aHandle);
    eventSource->ExecuteV(&IsProperRenderer, &isProperRenderer);
    M3G_DO_UNLOCK(aEnv)

    jboolean ret = static_cast<jboolean>(isProperRenderer);
    return ret;
}

#ifdef RD_JAVA_NGA_ENABLED
static void bindGraphicsBuffer(M3GRenderContext aCtx,
                               CMIDGraphics *aGraphics,
                               const BindStruct *aR)
{   
    if (m3gSetRenderBuffers(aCtx, aR->depth ? 
        M3G_COLOR_BUFFER_BIT|M3G_DEPTH_BUFFER_BIT : 
        M3G_COLOR_BUFFER_BIT) && m3gSetRenderHints(aCtx, aR->hintBits)) 
    {
        
        // Try to bind to EGLSurface if possible
        if (aGraphics->IsEglAvailable()) 
        {
            m3gBindEGLSurfaceTarget(aCtx, (M3GEGLSurface)aGraphics->BindEglSurface());
        }
        else
        {
            eglWaitNative(EGL_CORE_NATIVE_ENGINE);  
            m3gBindBitmapTarget(aCtx, (M3GNativeBitmap) aGraphics->Bitmap());
            // Get the physical screen size and pass it to m3gcore. This affects (improves) the performance
            // as the canvas frambuffer (rendering target) is larger than the physical screen size in 
            // devices that have more than one screen orientation, causing extra copying operations.
            // This will improve m3g performance and suppresses extra bitmap copying.
            TRect screenRect = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
            m3gSetDisplayArea(aCtx, screenRect.Width(), screenRect.Height());
        }            
        m3gSetClipRect(aCtx, aR->x, aR->y, aR->w, aR->h);
        m3gSetViewport(aCtx, aR->x, aR->y, aR->w, aR->h);
    }
      
}

#else // !RD_JAVA_NGA_ENABLED    
static void bindGraphicsBuffer(M3GRenderContext aCtx,
                               CMIDGraphics *aGraphics,
                               const BindStruct *aR)
{
    CFbsBitmap *bitmap = aGraphics->Bitmap();

    /*
    * Get the physical screen size and pass it to m3gcore. This affects (improves) the performance
    * as the canvas frambuffer (rendering target) is larger than the physical screen size in
    * devices that have more than one screen orientation, causing extra copying operations.
    *
    * This will improve m3g performance and suppresses extra bitmap copying.
    */

    TRect screenRect = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();

    eglWaitNative(EGL_CORE_NATIVE_ENGINE);

    if (m3gSetRenderBuffers(aCtx, aR->depth ?
                            M3G_COLOR_BUFFER_BIT|M3G_DEPTH_BUFFER_BIT :
                            M3G_COLOR_BUFFER_BIT) && m3gSetRenderHints(aCtx, aR->hintBits))
    {
        m3gBindBitmapTarget(aCtx, (M3GNativeBitmap) bitmap);

        // Pass the physical screen size to m3gcore
        m3gSetDisplayArea(aCtx, screenRect.Width(), screenRect.Height());

        m3gSetClipRect(aCtx, aR->x, aR->y, aR->w, aR->h);
        m3gSetViewport(aCtx, aR->x, aR->y, aR->w, aR->h);
    }

}
#endif // RD_JAVA_NGA_ENABLED    

JNIEXPORT jboolean JNICALL Java_javax_microedition_m3g_Graphics3D__1bindGraphics
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aCtx,
 jint aGraphicsHandle, jint aClipX, jint aClipY, jint aClipW, jint aClipH,
 jboolean aDepth, jint aHintBits, jboolean aIsProperRenderer)
{
    M3GRenderContext ctx = (M3GRenderContext)aCtx;

    // Fetch the native peer of our target Graphics object
    CMIDGraphics *cmidGraphics = MIDUnhandObject<CMIDGraphics>(aGraphicsHandle);

    BindStruct r;
    r.x = aClipX;
    r.y = aClipY;
    r.w = aClipW;
    r.h = aClipH;
    r.hintBits = aHintBits;
    r.depth = aDepth;

    jboolean isImageTarget = cmidGraphics->IsImageTarget();

    M3G_DO_LOCK

    // Call to the server side
    CJavaM3GEventSource* eventSource =
        JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&bindGraphicsBuffer,
                          ctx, cmidGraphics, (const BindStruct *)&r);

    M3G_DO_UNLOCK(aEnv)

    if (isImageTarget && aIsProperRenderer)
    {
        m3gSetAlphaWrite(ctx, M3G_FALSE);
    }

    return isImageTarget;
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1setViewport
(JNIEnv* aEnv, jclass, jint aHContext, jint aX, jint aY,
 jint aWidth, jint aHeight)
{
    M3G_DO_LOCK
    m3gSetViewport((M3GRenderContext)aHContext, aX, aY, aWidth, aHeight);
    M3G_DO_UNLOCK(aEnv)
}

#ifdef RD_JAVA_NGA_ENABLED
static void releaseTarget(M3GRenderContext aCtx, CMIDGraphics *aGraphics)
{
    // No need to wait GL when EGL surface is used
    if (!aGraphics || !aGraphics->IsEglAvailable())
    {
        eglWaitGL();
    }
    m3gReleaseTarget(aCtx);
}
#else
static void releaseTarget(M3GRenderContext aCtx)
{
    eglWaitGL();
    m3gReleaseTarget(aCtx);
}
#endif

static void releaseGraphicsTarget(M3GRenderContext aCtx, CMIDGraphics *aGraphics,
                                  TBool aIsImageTarget, TBool aIsProperRenderer)
{
#ifdef RD_JAVA_NGA_ENABLED
    releaseTarget(aCtx, aGraphics);
#else
    releaseTarget(aCtx);
#endif    

#ifdef RD_JAVA_NGA_ENABLED
    aGraphics->ReleaseEglSurface();
#endif // RD_JAVA_NGA_ENABLED     
    
    // clear alpha for only mutable off-screen images (not for canvas/GameCanvas
    // framebuffer) those images are indetified by aIsImageTarget argument
    if (aIsImageTarget)
    {
        if (aIsProperRenderer)
        {
            m3gSetAlphaWrite(aCtx, M3G_TRUE);
        }
        else
        {
            CFbsBitmap *bitmap = aGraphics->Bitmap();

            const TInt width = bitmap->SizeInPixels().iWidth;
            const TInt height = bitmap->SizeInPixels().iHeight;
            TInt stride = bitmap->ScanLineLength(width, bitmap->DisplayMode());

            bitmap->LockHeap();

            for (TInt i = 0; i < height; i++)
            {
                const void *srcAddr =
                    ((const char *) bitmap->DataAddress()) + i * stride;
                unsigned char *src = (unsigned char *) srcAddr;
                TInt count = width;
                while (count--)
                {
                    src += 3; //jump to last byte - alpha channel
                    //setting FF to alpha channel for non-canvas image targets
                    *src |= 0xff;
                    src++;
                }
            }

            bitmap->UnlockHeap();
        }
    }
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1releaseGraphics
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHandle,
 jint aGraphicsHandle, jboolean aIsImageTarget, jboolean aIsProperRenderer)
{
    M3G_DO_LOCK

    CMIDGraphics *cmidGraphics = MIDUnhandObject<CMIDGraphics>(aGraphicsHandle);

    CJavaM3GEventSource* eventSource =
        JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&releaseGraphicsTarget, ((M3GRenderContext) aHandle),
                          cmidGraphics, ((TBool) aIsImageTarget), ((TBool) aIsProperRenderer));
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1setCamera
(JNIEnv* aEnv, jclass, jint aHContext, jint aHCamera, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (Matrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }

    M3G_DO_LOCK
    m3gSetCamera((M3GRenderContext) aHContext, (M3GCamera) aHCamera, transform);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, JNI_ABORT);
    }
}

static void renderWorld(M3GRenderContext aHContext,
                        M3GWorld aHWorld)
{
    m3gRenderWorld(aHContext, aHWorld);
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1renderWorld
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHContext, jint aHWorld)
{
    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&renderWorld,
                          (M3GRenderContext) aHContext,
                          (M3GWorld) aHWorld);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1ctor
(JNIEnv* aEnv, jclass, jint aM3g)
{
    M3G_DO_LOCK
    M3GRenderContext ctx = m3gCreateContext((M3GInterface)aM3g);
    M3G_DO_UNLOCK(aEnv)

    return (jint)ctx;
}

struct RenderStruct
{
    M3GVertexBuffer hVertices;
    M3GIndexBuffer hIndices;
    M3GAppearance hAppearance;
    const M3GMatrix *transform;
};

static void renderImmediate(M3GRenderContext aHContext, RenderStruct *aR, jint aScope)
{
    m3gRender(aHContext,
              aR->hVertices,
              aR->hIndices,
              aR->hAppearance,
              aR->transform, 1.0f, aScope);
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1render
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHContext,
 jint aHVertices, jint aHIndices, jint aHAppearance, jbyteArray aTransform, jint aScope)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)aEnv->GetByteArrayElements(aTransform, NULL);
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }

    RenderStruct r;
    r.hVertices = (M3GVertexBuffer) aHVertices;
    r.hIndices = (M3GIndexBuffer) aHIndices;
    r.hAppearance = (M3GAppearance) aHAppearance;
    r.transform = transform;

    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&renderImmediate, ((M3GRenderContext) aHContext), &r, aScope);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, JNI_ABORT);
    }
}

static void clear(M3GRenderContext aHContext, M3GBackground aHBackground)
{
    m3gClear(aHContext, aHBackground);
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1clear
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aCtx, jint aBg)
{
    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&clear, (M3GRenderContext)aCtx, (M3GBackground)aBg);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1releaseImage
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHCtx)
{
    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
#ifdef RD_JAVA_NGA_ENABLED
    eventSource->ExecuteV(&releaseTarget, (M3GRenderContext)aHCtx, (CMIDGraphics*)NULL);
#else    
    eventSource->ExecuteV(&releaseTarget, (M3GRenderContext)aHCtx);
#endif    
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1addRef
(JNIEnv* aEnv, jclass, jint aObject)
{
    M3G_DO_LOCK
    m3gAddRef((M3GObject) aObject);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1addLight
(JNIEnv* aEnv, jclass, jint aHContext, jint aHLight, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return 0;
        }
    }
    M3G_DO_LOCK
    int idx = m3gAddLight((M3GRenderContext) aHContext, (M3GLight) aHLight, transform);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, JNI_ABORT);
    }

    return idx;
}

static void bindImage(M3GRenderContext hCtx, M3GImage hImg, M3Gbool depth, M3Gbitmask hintBits)
{
    if (m3gSetRenderBuffers(hCtx, depth ? M3G_COLOR_BUFFER_BIT|M3G_DEPTH_BUFFER_BIT : M3G_COLOR_BUFFER_BIT) && m3gSetRenderHints(hCtx, hintBits))
    {
        m3gBindImageTarget(hCtx, hImg);
    }
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1bindImage
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHCtx, jint aImageHandle, jboolean aDepth, jint aHintBits)
{
    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&bindImage, (M3GRenderContext)aHCtx, (M3GImage)aImageHandle, (M3Gbool)aDepth, (M3Gbitmask)aHintBits);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1resetLights
(JNIEnv* aEnv, jclass, jint aHContext)
{
    M3G_DO_LOCK
    m3gClearLights((M3GRenderContext) aHContext);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1setDepthRange
(JNIEnv* aEnv, jclass, jint aHContext, jfloat aDepthNear, jfloat aDepthFar)
{
    M3G_DO_LOCK
    m3gSetDepthRange((M3GRenderContext) aHContext, aDepthNear, aDepthFar);
    M3G_DO_UNLOCK(aEnv)
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1setLight
(JNIEnv* aEnv, jclass, jint aHContext, jint aLightIndex, jint aHLight, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }

    M3G_DO_LOCK
    m3gSetLight((M3GRenderContext) aHContext, aLightIndex, (M3GLight) aHLight, transform);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, JNI_ABORT);
    }
}

static void renderNode(M3GRenderContext aHCtx,
                       M3GNode aHNode,
                       const M3GMatrix *aMtx)
{
    m3gRenderNode(aHCtx, aHNode, aMtx);
}

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1renderNode
(JNIEnv* aEnv, jclass, jint aEventSourceHandle, jint aHCtx, jint aHNode, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }

    M3G_DO_LOCK
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&renderNode, (M3GRenderContext)aHCtx, (M3GNode)aHNode, (const M3GMatrix *)transform);
    M3G_DO_UNLOCK(aEnv)

    if (aTransform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, JNI_ABORT);
    }
}

#if defined(M3G_ENABLE_PROFILING)

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getStatistics
(JNIEnv* aEnv, jclass, jintArray aStatArray)
{
    const M3Gint *statArray = (M3Gint *)(aStatArray != NULL ? aEnv->GetIntArrayElements(aStatArray, NULL) : NULL);
    jint statArrayLength = aStatArray ? aEnv->GetArrayLength(aStatArray) : 0;

    if (statArray != NULL && statArrayLength >= sizeof(m3gs_statistic))
    {
        m3gCopy((void*)statArray, m3gs_statistic, sizeof(m3gs_statistic));
    }

    M3G_DO_LOCK
    m3gZero(m3gs_statistic, sizeof(m3gs_statistic));
    M3G_DO_UNLOCK(aEnv)

    if (statArray)
    {
        aEnv->ReleaseIntArrayElements(aStatArray, (jint*)statArray, 0);
    }

    return sizeof(m3gs_statistic);
}

#endif /* M3G_ENABLE_PROFILING */



/* M3G 1.1 JNI Calls */

JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1getViewTransform
(JNIEnv* aEnv, jclass, jint aHCtx, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return;
        }
    }

    M3G_DO_LOCK
    m3gGetViewTransform((M3GRenderContext) aHCtx, transform);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        /* copy array to Java side and release arrays */
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, 0);
    }
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getCamera
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    M3G_DO_LOCK
    jint camera = (jint)m3gGetCamera((M3GRenderContext)aHCtx);
    M3G_DO_UNLOCK(aEnv)

    return camera;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getLightTransform
(JNIEnv* aEnv, jclass, jint aHCtx, jint aLightIndex, jbyteArray aTransform)
{
    M3GMatrix *transform = NULL;
    if (aTransform)
    {
        transform = (M3GMatrix *)(aEnv->GetByteArrayElements(aTransform, NULL));
        if (transform == NULL)
        {
            M3G_RAISE_EXCEPTION(aEnv, "java/lang/OutOfMemoryError");
            return 0;
        }
    }
    M3G_DO_LOCK
    int lightTransform = (M3Guint)m3gGetLightTransform((M3GRenderContext)aHCtx, aLightIndex, transform);
    M3G_DO_UNLOCK(aEnv)

    if (transform)
    {
        aEnv->ReleaseByteArrayElements(aTransform, (jbyte*)transform, 0);
    }

    return (jint)lightTransform;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getLightCount
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    M3G_DO_LOCK
    jint lightCount = (jint)m3gGetLightCount((M3GRenderContext)aHCtx);
    M3G_DO_UNLOCK(aEnv)

    return lightCount;
}

JNIEXPORT jfloat JNICALL Java_javax_microedition_m3g_Graphics3D__1getDepthRangeNear
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    float depthNear = 0;
    float depthFar = 0;

    M3G_DO_LOCK
    m3gGetDepthRange((M3GRenderContext) aHCtx, &depthNear, &depthFar);
    M3G_DO_UNLOCK(aEnv)

    return (jfloat)depthNear;
}

JNIEXPORT jfloat JNICALL Java_javax_microedition_m3g_Graphics3D__1getDepthRangeFar
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    float depthNear = 0;
    float depthFar = 0;

    M3G_DO_LOCK
    m3gGetDepthRange((M3GRenderContext) aHCtx, &depthNear, &depthFar);
    M3G_DO_UNLOCK(aEnv)

    return (jfloat)depthFar;
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getViewportX
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    int viewport[4];

    M3G_DO_LOCK
    m3gGetViewport((M3GRenderContext)aHCtx, &viewport[0],
                   &viewport[1],
                   &viewport[2],
                   &viewport[3]);
    M3G_DO_UNLOCK(aEnv)

    return (jint)viewport[0];
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getViewportY
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    int viewport[4];

    M3G_DO_LOCK
    m3gGetViewport((M3GRenderContext)aHCtx, &viewport[0],
                   &viewport[1],
                   &viewport[2],
                   &viewport[3]);
    M3G_DO_UNLOCK(aEnv)

    return (jint)viewport[1];
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getViewportWidth
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    int viewport[4];

    M3G_DO_LOCK
    m3gGetViewport((M3GRenderContext)aHCtx, &viewport[0],
                   &viewport[1],
                   &viewport[2],
                   &viewport[3]);
    M3G_DO_UNLOCK(aEnv)

    return (jint)viewport[2];
}

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getViewportHeight
(JNIEnv* aEnv, jclass, jint aHCtx)
{
    int viewport[4];

    M3G_DO_LOCK
    m3gGetViewport((M3GRenderContext)aHCtx, &viewport[0],
                   &viewport[1],
                   &viewport[2],
                   &viewport[3]);
    M3G_DO_UNLOCK(aEnv)

    return (jint)viewport[3];
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_m3g_Graphics3D__1isAASupported
(JNIEnv* /*aEnv*/, jclass, jint aM3g)
{
    M3Gbool aaSupport = M3G_FALSE;

    aaSupport = m3gIsAntialiasingSupported((M3GInterface)aM3g);

    return (jboolean)aaSupport;
}

#ifdef RD_JAVA_NGA_ENABLED
    
JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getTargetHeight
    (JNIEnv*, jclass, jint aGraphicsHandle)
{
    CMIDGraphics* cmidGraphics = MIDUnhandObject<CMIDGraphics>(aGraphicsHandle);
    jint height = (jint)cmidGraphics->CanvasTargetSize().iHeight;
    return height;
}
  
JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getTargetWidth
    (JNIEnv*, jclass, jint aGraphicsHandle)
{
    CMIDGraphics* cmidGraphics = MIDUnhandObject<CMIDGraphics>(aGraphicsHandle);
    jint width = (jint)cmidGraphics->CanvasTargetSize().iWidth;
    return width;
}  
    
static void UpdateEglContent(CMIDGraphics* aGraphics)
{
    aGraphics->UpdateEglContent();
}
  
JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1updateEglContent
    (JNIEnv*, jclass, jint aEventSourceHandle, jint aGraphicsHandle)
{
    CMIDGraphics* cmidGraphics = MIDUnhandObject<CMIDGraphics>(aGraphicsHandle);
    // Call to the server side
    CJavaM3GEventSource* eventSource = JavaUnhand<CJavaM3GEventSource>(aEventSourceHandle);
    eventSource->ExecuteV(&UpdateEglContent, cmidGraphics);
}

#else // !RD_JAVA_NGA_ENABLED 

JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getTargetHeight
    (JNIEnv*, jclass, jint)
{
    return 0;
}
  
JNIEXPORT jint JNICALL Java_javax_microedition_m3g_Graphics3D__1getTargetWidth
    (JNIEnv*, jclass, jint)
{
    return 0;
}  
    
JNIEXPORT void JNICALL Java_javax_microedition_m3g_Graphics3D__1updateEglContent
    (JNIEnv*, jclass, jint, jint)
{
}  
  
#endif // RD_JAVA_NGA_ENABLED
