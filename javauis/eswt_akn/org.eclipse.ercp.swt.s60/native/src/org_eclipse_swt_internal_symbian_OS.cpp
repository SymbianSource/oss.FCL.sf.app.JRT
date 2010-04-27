/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <gulutil.h>
#include "org_eclipse_swt_internal_symbian_OS.h"
#include "eswtwidgetscore.h"
#include "swtimagedata.h"
#include "templatehelpers.h"
#include "utils.h"
#include "swtimagedataloader.h"
#include "swtdialogbroker.h"


// ======== LOCAL FUNCTIONS ========


template<class C>
static jint NewCoreWidget(JNIEnv* aJniEnv,
                          C*(MSwtFactory::*aMethodL)(MSwtDisplay&, TSwtPeer, MSwtComposite&, TInt) const,
                          jobject aPeer,
                          jint aParent,
                          jint aStyle)
{
    CSwtDisplay& display = CSwtDisplay::Current();
    const MSwtFactory& factory = display.Factory();
    return NewWidget(aJniEnv, &factory, aMethodL, display, aPeer, aParent, aStyle);
}


extern "C"
{

    /*
     *  Class Device
     */
    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetBounds(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        TRect       bounds(TRect::EUninitialized);
        CallMethod(bounds, device, &MSwtDevice::Bounds);
        return NewJavaRectangle(aJniEnv, bounds);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetClientArea(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        TRect       clientArea(TRect::EUninitialized);
        CallMethod(clientArea, device, &MSwtDevice::ClientArea);
        return NewJavaRectangle(aJniEnv, clientArea);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetDepth(JNIEnv* , jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        TInt        depth;
        CallMethod(depth, device, &MSwtDevice::Depth);
        return depth;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetDPI(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        TSize       dpi(TSize::EUninitialized);
        CallMethod(dpi, device, &MSwtDevice::Dpi);
        return NewJavaPoint(aJniEnv, TPoint(dpi.iWidth, dpi.iHeight));
    }

    static TInt ConvertFontStyle(const TFontStyle& aFontStyle)
    {
        TInt result = KSwtNormal;

        if (aFontStyle.StrokeWeight() == EStrokeWeightBold)
            result |= KSwtBold;

        if (aFontStyle.Posture() == EPostureItalic)
            result |= KSwtItalic;

        return result;
    }

    static jobjectArray Device_GetFontListL(JNIEnv* aJniEnv, const MSwtDevice* aDevice, jstring aFaceName, TBool aScalable)
    {
        HBufC* faceName = ConvertStringLC(aJniEnv, aFaceName);
        TDesC& nameDesc = const_cast<TDesC&>((faceName!=NULL) ? static_cast<const TDesC&>(*faceName) : KNullDesC());
        CArrayFixFlat<TSwtFontData>* fontDataArray = NULL;
        CallMethodL(fontDataArray, aDevice, &MSwtDevice::GetFontListL, nameDesc, aScalable);
        if (faceName!=NULL)
            CleanupStack::PopAndDestroy(faceName);

        // Construct the array of Java Fontdata
        jobjectArray javaFontDataArray = NULL;
        TInt nbFontData = fontDataArray->Count();
        if (nbFontData > 0)
        {
            jclass fontDataClass = aJniEnv->FindClass("org/eclipse/swt/graphics/FontData");
            if (fontDataClass != NULL)
            {
                javaFontDataArray = aJniEnv->NewObjectArray(nbFontData, fontDataClass, NULL);
                if (javaFontDataArray != NULL)
                {
                    jmethodID constructId = aJniEnv->GetMethodID(fontDataClass, "<init>", "(Ljava/lang/String;II)V");
                    if (constructId != NULL)
                    {
                        for (TInt i=0; i<nbFontData; ++i)
                        {
                            // Construct a Java Fontdata object
                            const TFontSpec& fontSpec = (*fontDataArray)[i].iFontSpec;
                            jstring fontDataName   = NewJavaString(aJniEnv, fontSpec.iTypeface.iName);
                            jint    heightInPoints = FontUtils::PointsFromTwips(fontSpec.iHeight);
                            jint    style          = ConvertFontStyle(fontSpec.iFontStyle);
                            jobject fontDataObj    = aJniEnv->NewObject(fontDataClass, constructId, fontDataName, heightInPoints, style);
                            aJniEnv->DeleteLocalRef(fontDataName);

                            // Append to font data array
                            aJniEnv->SetObjectArrayElement(javaFontDataArray, i, fontDataObj); //lint !e613
                            aJniEnv->DeleteLocalRef(fontDataObj);
                        }
                    }
                }
                aJniEnv->DeleteLocalRef(fontDataClass);
            }
        }

        DeleteInUiHeap(fontDataArray);

        return javaFontDataArray;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetFontList(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aFaceName, jboolean aScalable)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        jobjectArray      result = NULL;
        TRAPD(error, (result=Device_GetFontListL(aJniEnv, device, aFaceName, aScalable)));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetSystemColor(JNIEnv* aJniEnv, jclass, jint aHandle, jint aId)
    {
        ASSERT(aHandle!=0);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        TSwtColorId colorId(static_cast<TSwtColorId>(aId));
        TRgb rgb;
        CallMethod(rgb, device, &MSwtDevice::GetSystemColor, colorId);
        return NewJavaRgb(aJniEnv, rgb);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Device_1GetSystemFont(JNIEnv*, jclass, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aHandle);
        const MSwtDevice* device = reinterpret_cast<MSwtDevice*>(aHandle);
        const MSwtFont* font;
        CallMethod(font, device, &MSwtDevice::GetSystemFont, display);
        ASSERT(font!=NULL);
        return reinterpret_cast<jint>(font);
    }


    /*
     * Class Display
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aDisplayParameter)
    {
        CSwtDisplay* display = NULL;
        TRAPD(error, (display=CSwtDisplay::NewL(*aJniEnv, aPeer, aDisplayParameter)));
        ThrowIfError(error, aJniEnv);
        return reinterpret_cast<jint>(display);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1DeviceHandle(JNIEnv*, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        return reinterpret_cast<jint>(static_cast<MSwtDevice*>(reinterpret_cast<CSwtDisplay*>(aHandle)));
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        if (!aHandle)
            return;

        CSwtDisplay* display = reinterpret_cast<CSwtDisplay*>(aHandle);

#if defined(__WINS__) && defined(_DEBUG)
        display->iInstanceCounts.CheckForLeaks();
#endif

        display->Dispose(*aJniEnv);
    }

    static void Display_SetAppNameL(JNIEnv* aJniEnv, jstring aString)
    {
        MSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (display)
        {
            HBufC* buf = ConvertStringLC(aJniEnv, aString);
            CallMethodL(&display->UiUtils(), &MSwtUiUtils::SetAppNameL, buf);
            CallMethodL(display, &MSwtDisplay::SetNameInTaskListL, buf);
            if (buf)
            {
                CleanupStack::PopAndDestroy(buf);
            }
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1SetAppName(
        JNIEnv* aJniEnv, jclass, jstring aString)
    {
        TRAPD(error, Display_SetAppNameL(aJniEnv, aString));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1Sleep(JNIEnv*, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        TBool result = reinterpret_cast<CSwtDisplay*>(aHandle)->Sleep(); //lint !e613
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1Wake(JNIEnv*, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        reinterpret_cast<CSwtDisplay*>(aHandle)->Wake();
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1ReadAndDispatch(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        TBool result = reinterpret_cast<CSwtDisplay*>(aHandle)->ReadAndDispatch(*aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1GetActiveShell(JNIEnv*, jclass, jint)
    {
        const MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();
        TSwtPeer peer;
        CallMethod(peer, &utils, &MSwtUiUtils::GetActiveShellPeer);
        return peer;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1getDoubleClickTime(JNIEnv *, jclass, jint)
    {
        const ASwtDisplayBase& display = CSwtDisplay::Current();
        TInt result;

        CallMethod(result, &display, &ASwtDisplayBase::GetDoubleClickTime);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1getIconDepth(JNIEnv *, jclass, jint)
    {
        const ASwtDisplayBase& display = CSwtDisplay::Current();
        TInt result;

        CallMethod(result, &display, &ASwtDisplayBase::GetIconDepth);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1GetShells(JNIEnv* aJniEnv, jclass, jint)
    {
        const MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();

        CSwtPeerArray* shells  = NULL;
        TRAPD(error, CallMethodL(shells, &utils, &MSwtUiUtils::GetShellPeersL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaControlArray(aJniEnv, shells);
        DeleteInUiHeap(shells);

        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1Beep(JNIEnv *, jclass, jint)
    {
        const ASwtDisplayBase& display = CSwtDisplay::Current();
        CallMethod(&display, &ASwtDisplayBase::Beep);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1GetFocusControl(JNIEnv *, jclass, jint)
    {
        const MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();
        TSwtPeer peer;

        CallMethod(peer, &utils, &MSwtUiUtils::GetFocusControl);
        return peer;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1StartTimer(JNIEnv* aJniEnv, jclass, jint, jint aDelayInMilliSeconds, jint aTimerHandle)
    {
        ASwtDisplayBase& display = CSwtDisplay::Current();

        TRAPD(error, CallMethodL(&display, &ASwtDisplayBase::AddTimerL, aDelayInMilliSeconds, aTimerHandle));
        ThrowIfError(error, aJniEnv);
    }


    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1Post
    (JNIEnv*, jclass, jint aEventType, jchar aEventCharacter, jint aEventKeyCode, jint aEventStateMask)
    {
        ASwtDisplayBase& display = CSwtDisplay::Current();
        TSwtKeyEventData data;
        data.iType           = static_cast<TSwtEventType>(aEventType);
        data.iCharacter      = aEventCharacter;
        data.iKeyCode        = aEventKeyCode;
        data.iStateMask      = aEventStateMask;
        TRAPD(ignore, CallMethodL(&display, &ASwtDisplayBase::PostL, data));
        return ConvertBoolean(ignore == KErrNone);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1MousePost
    (JNIEnv* aJniEnv, jclass, jint aEventType, jint xPos, jint yPos, jint aEventStateMask)
    {
        ASwtDisplayBase& display = CSwtDisplay::Current();

        TPoint point;
        point=TPoint(xPos, yPos);
        TSwtKeyEventData data;
        data.iType           = static_cast<TSwtEventType>(aEventType);
        data.iStateMask      = aEventStateMask;
        TBool results;
        TRAPD(error, CallMethodL(results, &display, &ASwtDisplayBase::MousePostL, data, point));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(results);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1SetAppUID
    (JNIEnv* , jclass, jint aUID)
    {
        MSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (display)
        {
            CallMethod(display, &MSwtDisplay::SetUIDInTaskList, aUID);
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1SetAppVisible
    (JNIEnv *, jclass, jboolean aVisible)
    {
        MSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (display)
        {
            CallMethod(display, &MSwtDisplay::SetAppVisible, aVisible);
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1SetOptimizedPaint
    (JNIEnv *, jclass, jint aHandle, jboolean aStatus)
    {
        CSwtDisplay* display = reinterpret_cast<CSwtDisplay*>(aHandle);
        if (display)
        {
            CSwtEventQueue* queue = display->EventQueue();
            if (queue)
            {
                CallMethod(queue, &CSwtEventQueue::SetOptimizedPaint, aStatus);
            }
        }
    }
    
    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Display_1NotifyFirstPaintComplete
    (JNIEnv *, jclass, jboolean aTopShell)
    {
        MSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (display)
        {
            CallMethod(display, &MSwtDisplay::SetUiReady, aTopShell);
        }
    }

    /*
     * Class DisplayExtension
     */
    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_DisplayExtension_1getBestImageSize
    (JNIEnv* aJniEnv, jclass, jint aImageType)
    {
        MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();
        TSize       imageSize(TSize::EUninitialized);
        CallMethod(imageSize, &utils, &MSwtUiUtils::GetBestImageSize, aImageType);
        return NewJavaPoint(aJniEnv, TPoint(imageSize.iWidth, imageSize.iHeight));
    }

    /*
     * Class Canvas
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Canvas_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewCanvasL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Canvas);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Canvas_1CompositeHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtCanvas*    canvas = reinterpret_cast<MSwtCanvas*>(aHandle);
        MSwtComposite* result;
        CallMethod(result, canvas, &MSwtCanvas::Composite);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Canvas_1Scroll(JNIEnv*, jclass, jint aHandle, jint aDestX, jint aDestY, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aAll)
    {
        MSwtCanvas* canvas = reinterpret_cast<MSwtCanvas*>(aHandle);
        TPoint      dest(aDestX, aDestY);
        TRect       rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(canvas, &MSwtCanvas::Scroll, dest, rect, aAll);
    }


    /*
     * Class Color
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Color_1New(JNIEnv* aJniEnv, jclass, jint aDevice, jint aRed, jint aGreen, jint aBlue)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtDevice*  device  = reinterpret_cast<MSwtDevice*>(aDevice);
        TRgb         rgb(aRed, aGreen, aBlue);
        MSwtColor*   result = NULL;
        TRAPD(error, CallMethodL(result, &(display->Factory()), &MSwtFactory::NewColorL, *device, rgb, display));
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT_USE_DISPLAY(result, Color, display);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Color_1Dispose(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtColor* color = reinterpret_cast<MSwtColor*>(aHandle);
        CallMethod(color, &MSwtColor::Dispose, display);

        DECREASE_INSTANCE_COUNT_USE_DISPLAY(Color, display);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Color_1RgbValue(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtColor* color = reinterpret_cast<MSwtColor*>(aHandle);
        TRgb       rgb;
        CallMethod(rgb, color, &MSwtColor::RgbValue, display);
        return rgb.Value();
    }


    /*
     * Class Composite
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint result = NewCoreWidget(aJniEnv, &MSwtFactory::NewCompositeL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(result, Composite);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1ScrollableHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtComposite*  composite = reinterpret_cast<MSwtComposite*>(aHandle);
        MSwtScrollable* result;
        CallMethod(result, composite, &MSwtComposite::Scrollable);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1GetChildren(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtComposite* composite = reinterpret_cast<MSwtComposite*>(aHandle);
        CSwtPeerArray* children  = NULL;
        TRAPD(error, CallMethodL(children, composite, &MSwtComposite::GetChildrenPeersL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaControlArray(aJniEnv, children);
        DeleteInUiHeap(children);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1SetTabList(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aList)
    {
        jboolean isCopy;
        TInt* intList = NULL;
        TInt  count   = 0;
        if (aList != NULL)
        {
            intList = aJniEnv->GetIntArrayElements(aList, &isCopy);
            count   = aJniEnv->GetArrayLength(aList);
        }

        MSwtComposite* composite = reinterpret_cast<MSwtComposite*>(aHandle);
        // This cast are safe : in JAVA side the array are fill with the handle 's control.
        MSwtControl**  ctrlList  = reinterpret_cast<MSwtControl**>(intList); //lint !e740
        TRAPD(error, CallMethodL(composite, &MSwtComposite::SetTabListL, ctrlList, count));

        if (intList != NULL)
            aJniEnv->ReleaseIntArrayElements(aList, intList, JNI_ABORT);

        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1GetTabList(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtComposite* composite = reinterpret_cast<MSwtComposite*>(aHandle);
        CSwtPeerArray*       tabList   = NULL;
        TRAPD(error, CallMethodL(tabList, composite, &MSwtComposite::GetTabListL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaControlArray(aJniEnv, tabList);
        DeleteInUiHeap(tabList);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Composite_1SetScrolledCompositeContent(JNIEnv* aJniEnv, jclass, jint aHandle, jint aContentHandle)
    {
        MSwtComposite* composite = reinterpret_cast<MSwtComposite*>(aHandle);
        MSwtControl* content = reinterpret_cast<MSwtControl*>(aContentHandle);
        TRAPD(error, CallMethodL(composite, &MSwtComposite::SetScrolledCompositeContentL, content));
        ThrowIfError(error, aJniEnv);
    }

    /*
     * Class Control
     */
    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TSwtPeer     peerRef;
        CallMethod(peerRef, control, &MSwtControl::Dispose);

        ReleasePeer(aJniEnv, peerRef);

#if defined(__WINS__) && defined(_DEBUG)
        ++(CSwtDisplay::Current().iInstanceCounts.iDisposedControls);
#endif
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetFocus(JNIEnv*, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool        isFocused;
        CallMethod(isFocused, control, &MSwtControl::SetSwtFocus, KSwtFocusByApi);
        return ConvertBoolean(isFocused);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1ForceFocus(JNIEnv*, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool        isFocused;
        CallMethod(isFocused, control, &MSwtControl::SetSwtFocus, KSwtFocusByForce);
        return ConvertBoolean(isFocused);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetEnabled(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool              isEnabled;
        CallMethod(isEnabled, control, &MSwtControl::GetEnabled);
        return ConvertBoolean(isEnabled);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetVisible(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool              isVisible;
        CallMethod(isVisible, control, &MSwtControl::GetVisible, display);
        return ConvertBoolean(isVisible);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetBackground(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRgb result;
        CallMethod(result, control, &MSwtControl::GetBackground);
        return result.Value();
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetForeground(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRgb result;
        CallMethod(result, control, &MSwtControl::GetForeground);
        return result.Value();
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetFont(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        const MSwtFont* result;
        CallMethod(result, control, &MSwtControl::GetFont);
        ASSERT(result!=NULL);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetBorderWidth(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TInt result;
        CallMethod(result, control, &MSwtControl::GetBorderWidth);
        return result;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetBounds(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRect        boundResult(TRect::EUninitialized);
        CallMethod(boundResult, control, &MSwtControl::GetBounds);
        return NewJavaRectangle(aJniEnv, boundResult);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetClientArea(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRect clientRect(TRect::EUninitialized);
        CallMethod(clientRect, control, &MSwtControl::ClientRect);
        return NewJavaRectangle(aJniEnv, TRect(clientRect.Size()));
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetSize(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TSize              controlSize(TSize::EUninitialized);
        CallMethod(controlSize, control, &MSwtControl::GetWidgetSize);
        return NewJavaPoint(aJniEnv, controlSize.AsPoint());
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetStyle(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TInt result;
        CallMethod(result, control, &MSwtControl::Style);
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1HasDoNotDrawFlag(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool hasDoNotDrawFlag;
        CallMethod(hasDoNotDrawFlag, control, &MSwtControl::HasDoNotDrawFlag);
        return ConvertBoolean(hasDoNotDrawFlag);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1GetLocation(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TPoint             controlLocation(TPoint::EUninitialized);
        CallMethod(controlLocation, control, &MSwtControl::GetLocation);
        return NewJavaPoint(aJniEnv, controlLocation);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1IsFocusControl(JNIEnv*, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool isFocused;
        CallMethod(isFocused,control, &MSwtControl::IsFocusControl);
        return ConvertBoolean(isFocused);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1MoveAbove(JNIEnv*, jclass, jint aHandle, jint aControlHandle)
    {
        MSwtControl* control1 = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtControl* control2 = reinterpret_cast<MSwtControl*>(aControlHandle);
        CallMethod(control1, &MSwtControl::MoveAbove, control2);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1MoveBelow(JNIEnv*, jclass, jint aHandle, jint aControlHandle)
    {
        MSwtControl* control1 = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtControl* control2 = reinterpret_cast<MSwtControl*>(aControlHandle);
        CallMethod(control1, &MSwtControl::MoveBelow, control2);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1PostTraverseEvent
    (JNIEnv*, jclass, jint aHandle, jint aTraversal)
    {
        MSwtControl*  control = reinterpret_cast<MSwtControl*>(aHandle);
        TBool         doIt    = ETrue;
        TSwtTraversal traversal(static_cast<TSwtTraversal>(aTraversal));
        TBool result;
        CallMethod(result, control, &MSwtControl::ForceTraverseEvent, traversal, doIt);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1Paint(
        JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth,
        jint aHeight, jboolean aPaintNatively, jint aGcHandle, jintArray aIntParams, jint aIntCount,
        jstring aStrParams)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aGcHandle);
        TRect rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        TInt* ints = NULL;
        if (aIntParams)
        {
            ints = JavaIntRegionToInts(aJniEnv, aIntParams, 0, aIntCount);
        }
        HBufC* strings = NULL;
        if (aStrParams)
        {
            // If this fails we proceed without strings
            TRAP_IGNORE(strings = ConvertStringL(aJniEnv, aStrParams));
        }
        CallMethod(control, &MSwtControl::Paint, rect, aPaintNatively, gc, ints, aIntCount, strings, display);
        delete[] ints;
        delete strings;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1Redraw(JNIEnv*, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        CallMethod(control, &MSwtControl::Redraw);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1RedrawRegion(JNIEnv*, jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRect rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(control, &MSwtControl::RedrawRegion, rect);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetRedraw(JNIEnv*, jclass, jint aHandle, jboolean aRedraw)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        CallMethod(control, &MSwtControl::SetRedraw, aRedraw);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetCapture(JNIEnv*, jclass, jint aHandle, jboolean aCapture)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        CallMethod(control, &MSwtControl::SetCapture, aCapture);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetFont(JNIEnv* aJniEnv, jclass, jint aHandle, jint aFontHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtFont*    font    = reinterpret_cast<MSwtFont*>(aFontHandle);
        TRAPD(error, CallMethodL(control, &MSwtControl::SetFontL, font));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetBackground(JNIEnv* aJniEnv, jclass, jint aHandle, jint aColorHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtColor*   color   = reinterpret_cast<MSwtColor*>(aColorHandle);
        TRAPD(error, CallMethodL(control, &MSwtControl::SetBackgroundL, color));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetForeground(JNIEnv* aJniEnv, jclass, jint aHandle, jint aColorHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtColor*   color   = reinterpret_cast<MSwtColor*>(aColorHandle);
        TRAPD(error, CallMethodL(control, &MSwtControl::SetForegroundL, color));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetVisible(JNIEnv*, jclass, jint aHandle, jboolean aVisible)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        CallMethod(control, &MSwtControl::SetVisible, aVisible);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetLocation(JNIEnv*, jclass, jint aHandle, jint aX, jint aY)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TPoint       point(aX, aY);
        CallMethod(control, &MSwtControl::SetLocation, point);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetSize(JNIEnv*, jclass, jint aHandle, jint aWidth, jint aHeight)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TSize        size(aWidth, aHeight);
        CallMethod(control, &MSwtControl::SetWidgetSize, size);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1ComputeSize(JNIEnv* aJniEnv, jclass, jint aHandle, jint aWHint, jint aHHint)
    {
        MSwtControl* theControl = reinterpret_cast<MSwtControl*>(aHandle);
        TSize size(0,0);
        TRAPD(error, CallMethodL(size, theControl, &MSwtControl::ComputeSizeL, aWHint, aHHint));
        if (error == KErrNone)
        {
            return NewJavaPoint(aJniEnv, size.AsPoint());
        }
        else
        {
            Throw(error, aJniEnv);
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetEnabled(JNIEnv*, jclass, jint aHandle, jboolean aEnabled)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        CallMethod(control, &MSwtControl::SetEnabled, aEnabled);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetBounds(JNIEnv*, jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TRect        rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(control, &MSwtControl::SetBounds, rect);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1SetMenu(JNIEnv* aJniEnv, jclass, jint aHandle, jint aMenuHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aMenuHandle);
        TRAPD(error, CallMethodL(control, &MSwtControl::SetPopupMenuL, menu));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1ToControl(JNIEnv* aJniEnv, jclass, jint aHandle, jint aX, jint aY)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TPoint point(aX, aY);
        TPoint result(TPoint::EUninitialized);
        CallMethod(result, control, &MSwtControl::ToControl, point);
        return NewJavaPoint(aJniEnv, result);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1ToDisplay(JNIEnv* aJniEnv, jclass, jint aHandle, jint aX, jint aY)
    {
        const MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        TPoint point(aX, aY);
        TPoint result(TPoint::EUninitialized);
        CallMethod(result, control, &MSwtControl::ToDisplay, point);
        return NewJavaPoint(aJniEnv, result);
    }

    static void Control_Update(JNIEnv* aJniEnv, MSwtControl* aControl)
    {
        ASSERT(aControl);
        CSwtDisplay::Current().PopAndDispatchPaintEvents(*aJniEnv, aControl);

        // check disposal of Display from java-side-paint-listener
        if (!CSwtDisplay::CurrentOrNull())
        {
            return;
        }

        MSwtComposite* composite = aControl->CompositeInterface();

        if (composite)
        {
            const RSwtControlArray* children = composite->Children();
            if (children)
            {
                for (TInt i = 0; i < children->Count(); ++i)
                {
                    MSwtControl* child = (*children)[i];
                    Control_Update(aJniEnv, child);

                    // check disposal of Display from java-side-paint-listener
                    if (!CSwtDisplay::CurrentOrNull())
                    {
                        return;
                    }
                }
            }
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Control_1Update(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtControl* control = reinterpret_cast<MSwtControl*>(aHandle);
        Control_Update(aJniEnv, control);
    }


    /*
     * Class Decorations
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewDecorationsL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Decorations);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1CanvasHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        MSwtCanvas*      result;
        CallMethod(result, decorations, &MSwtDecorations::Canvas);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1GetText(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        const TDesC* title;
        CallMethod(title, decorations, &MSwtDecorations::GetText);
        return NewJavaString(aJniEnv, (title!=NULL) ? *title : KNullDesC());
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetImage(JNIEnv* aJniEnv, jclass, jint aHandle, jint aImageHandle)
    {
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        TInt error = KErrNone;
        TRAP(error, CallMethodL(decorations, &MSwtDecorations::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetMaximized(JNIEnv*, jclass, jint aHandle, jboolean aMaximized)
    {
        MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        CallMethod(decorations, &MSwtDecorations::SetMaximized, aMaximized);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1GetMaximized(JNIEnv*, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtDecorations::GetMaximized));
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetMinimized(JNIEnv*, jclass, jint aHandle, jboolean aMinimized)
    {
        MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        CallMethod(decorations, &MSwtDecorations::SetMinimized, aMinimized);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1GetMinimized(JNIEnv*, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtDecorations::GetMinimized));
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetMenuBar(JNIEnv*, jclass, jint aHandle, jint aMenuHandle)
    {
        MSwtDecorations*  decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        MSwtMenu*         menuBar     = reinterpret_cast<MSwtMenu*>(aMenuHandle);
        CallMethod(decorations, &MSwtDecorations::SetMenuBar, menuBar);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aTitle)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtDecorations::SetTextL, aTitle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1SetDefaultButton(JNIEnv* , jclass, jint aHandle, jint aDefaultButtonHandle)
    {
        MSwtDecorations*    decorations     = reinterpret_cast<MSwtDecorations*>(aHandle);
        MSwtButton* defaultButton   = reinterpret_cast<MSwtButton*>(aDefaultButtonHandle);
        CallMethod(decorations, &MSwtDecorations::SetDefaultButton, defaultButton);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Decorations_1GetDefaultButton(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtDecorations* decorations = reinterpret_cast<MSwtDecorations*>(aHandle);
        TSwtPeer peer;
        CallMethod(peer, decorations, &MSwtDecorations::GetDefaultButton);
        return peer;
    }

    /*
     * Interface Drawable
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Drawable_1NewGc(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtDrawable* drawable = reinterpret_cast<MSwtDrawable*>(aHandle);
        MSwtGc*       result   = NULL;

        // GCs are always constructed in the UI thread.
        // If there is no display pointer in the TLS then this is not a UI thread
        // and we must throw an exception.
        CSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (!display)
        {
            Throw(ESwtErrorThreadInvalidAccess, aJniEnv);
            return NULL;
        }

        TRAPD(error, CallMethodL(result, drawable, &MSwtDrawable::NewGcL, display));
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT(result, Gc);

        return reinterpret_cast<jint>(result);
    }


    /*
     * Class Font
     */
    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1Dispose(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        ASSERT(aHandle!=0);
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        CallMethod(font, &MSwtFont::Dispose, display);

        DECREASE_INSTANCE_COUNT_USE_DISPLAY(Font, display);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1GetHeight(JNIEnv*, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        TInt height = 0;
        CallMethod(height, font, &MSwtFont::DesignHeightInPoints);
        return height;
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1GetName(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        HBufC* name = NULL;
        TRAPD(error, CallMethodL(name, font, &MSwtFont::GetNameL));
        ThrowIfError(error, aJniEnv);
        jstring javaName = (name!=NULL) ? NewJavaString(aJniEnv,*name) : NULL;
        DeleteInUiHeap(name);
        return javaName;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1GetStyle(JNIEnv*, jclass, jint aHandle)
    {
        ASSERT(aHandle!=0);
        const MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        TInt Style = 0;
        CallMethod(Style, font, &MSwtFont::Style);
        return Style;
    }

    static MSwtFont* Font_NewL(JNIEnv* aJniEnv, MSwtDevice* aDevice, const jstring aTypefaceName, TInt aHeight, TInt aStyle)
    {
        HBufC* typefaceName = ConvertStringLC(aJniEnv, aTypefaceName);
        CSwtDisplay* display = DisplayFromDevice(reinterpret_cast<jint>(aDevice));
        const MSwtFactory& factory = display->Factory();
        MSwtFont* font;
        CallMethodL(font, &factory, &MSwtFactory::NewFontL, *aDevice, *typefaceName, aHeight, aStyle, display);
        CleanupStack::PopAndDestroy(typefaceName);
        return font;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1New(JNIEnv* aJniEnv, jclass, jint aDevice, jstring aTypefaceName, jint aHeight, jint aStyle)
    {
        ASSERT(aDevice!=0);
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtDevice*  device  = reinterpret_cast<MSwtDevice*>(aDevice);
        MSwtFont*    font = NULL;
        TRAPD(error, (font=Font_NewL(aJniEnv, device, aTypefaceName, aHeight, aStyle)));
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT_USE_DISPLAY(font, Font, display);

        return reinterpret_cast<jint>(font);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1AddRef(JNIEnv *, jclass, jint aHandle)
    {
        ASSERT(aHandle);
        const MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        CallMethod(static_cast<const MSwtRefCounted*>(font), &MSwtFont::AddRef);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Font_1RemoveRef(JNIEnv *, jclass, jint aHandle)
    {
        ASSERT(aHandle);
        const MSwtFont* font = reinterpret_cast<MSwtFont*>(aHandle);
        CallMethod(static_cast<const MSwtRefCounted*>(font), &MSwtFont::RemoveRef);
    }


    /*
     * Class GC
     */
    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1CopyAreaToImage(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jint aImageHandle, jint aX, jint aY)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        MSwtImage*    image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TPoint        pos(aX, aY);
        TRAPD(error, CallMethodL(gc, &MSwtGc::CopyAreaL, *image, pos, display));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1CopyArea(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aSrcX, jint aSrcY, jint aWidth, jint aHeight, jint aDestX, jint aDestY)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rectSource(TPoint(aSrcX, aSrcY), TSize(aWidth, aHeight));
        TPoint  pointDest(aDestX, aDestY);
        CallMethod(gc, &MSwtGc::CopyArea, rectSource, pointDest, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);

        // GC is always disposed in the UI-thread so TLS can be used to get the display.
        // If there is no display pointer in the TLS then this is not a UI thread
        // and we must throw an exception.
        CSwtDisplay* display = CSwtDisplay::CurrentOrNull();
        if (!display)
        {
            Throw(ESwtErrorThreadInvalidAccess, aJniEnv);
            return;
        }

        // When display pointer is not passed here then TLS will be used.
        CallMethod(gc, &MSwtGc::Dispose);

        DECREASE_INSTANCE_COUNT(Gc);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetAlpha(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aAlpha)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc       = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, &MSwtGc::SetAlpha, aAlpha, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetBackground(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aColorHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc       = reinterpret_cast<MSwtGc*>(aHandle);
        MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(gc, &MSwtGc::SetBackground, *color, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetForeground(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aColorHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc       = reinterpret_cast<MSwtGc*>(aHandle);
        MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(gc, &MSwtGc::SetForeground, *color, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetLineStyle(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jint aStyle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc*       gc        = reinterpret_cast<MSwtGc*>(aHandle);
        TSwtLineStyle lineStyle(static_cast<TSwtLineStyle>(aStyle));
        TRAPD(error, CallMethodL(gc, &MSwtGc::SetLineStyleL, lineStyle, display));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetLineWidth(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aWidth)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, &MSwtGc::SetLineWidth, aWidth, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawImage(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aImageHandle, jint aSrcX, jint aSrcY, jint aSrcWidth, jint aSrcHeight, jint aDestX, jint aDestY, jint aDestWidth, jint aDestHeight)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc*    gc = reinterpret_cast<MSwtGc*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TPoint     srcPos(aSrcX, aSrcY);
        TSize      srcSize(aSrcWidth, aSrcHeight);
        TPoint     destPos(aDestX, aDestY);
        TSize      destSize(aDestWidth, aDestHeight);
        CallMethod(gc, &MSwtGc::DrawImage, *image, srcPos, srcSize, destPos, destSize, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawPoint(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TPoint  pt(aX,aY);
        CallMethod(gc, &MSwtGc::DrawPoint, pt, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawLine(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX1, jint aY1, jint aX2, jint aY2)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TPoint  pt1(aX1,aY1);
        TPoint  pt2(aX2,aY2);
        CallMethod(gc, &MSwtGc::DrawLine, pt1, pt2, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawPolyline(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jintArray aPointArray)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        TPoint* points = NULL;
        TInt    count  = JavaIntsToPoints(aJniEnv, aPointArray, points);
        if (count == 0)
            return;

        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, &MSwtGc::DrawPolyline, points, count, display);
        delete[] points;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawPolygon(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jintArray aPointArray, jboolean aFill)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        TPoint* points = NULL;
        TInt    count  = JavaIntsToPoints(aJniEnv, aPointArray, points);
        if (count == 0)
            return;

        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, (aFill) ? &MSwtGc::FillPolygon : &MSwtGc::DrawPolygon, points, count, display);
        delete[] points;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawRectangle(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aFill)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(gc, (aFill) ? &MSwtGc::FillRectangle : &MSwtGc::DrawRectangle, rect, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawRoundRectangle(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aArcWidth, jint aArcHeight, jboolean aFill)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        TSize   arcSize(aArcWidth, aArcHeight);
        CallMethod(gc, (aFill) ? &MSwtGc::FillRoundRectangle : &MSwtGc::DrawRoundRectangle, rect, arcSize, display);
    }

    static void Gc_DrawTextL(JNIEnv* aJniEnv, MSwtGc* aGc, const jstring aString, TInt aX, TInt aY, TInt aFlags, CSwtDisplay* aDevice)
    {
        CSwtDisplay* display = reinterpret_cast<CSwtDisplay*>(aDevice);
        HBufC* text = ConvertStringLC(aJniEnv, aString);
        TPoint position(aX, aY);
        TPtr   des(text->Des());
        CallMethodL(aGc, &MSwtGc::DrawTextL, des, position, aFlags, display);
        CleanupStack::PopAndDestroy(text);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawText(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jstring aString, jint aX, jint aY, jint aFlags)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        ASSERT(aHandle!=0);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRAPD(error, Gc_DrawTextL(aJniEnv, gc, aString, aX, aY, aFlags, display));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawOval(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aFill)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(gc, (aFill) ? &MSwtGc::FillOval : &MSwtGc::DrawOval, rect, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawArc(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aStartAngle, jint aArcAngle, jboolean aFill)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(gc, (aFill) ? &MSwtGc::FillArc : &MSwtGc::DrawArc, rect, aStartAngle, aArcAngle, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawFocusRect(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        const MSwtUiUtils& utils = display->UiUtils();
        CallMethod(gc, &MSwtGc::DrawFocusRect, rect, utils, display);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1IsClipped(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TBool         result;
        CallMethod(result, gc, &MSwtGc::IsClipped, display);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1FillGradientRectangle(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aVertical)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TBool invertGradient(EFalse);
        TInt  width = (0 < aWidth) ? aWidth : -aWidth;
        TInt  height = (0 < aHeight) ? aHeight : -aHeight;
        TRect rect(TPoint(aX, aY), TSize(width, height));

        if ((aWidth < 0) || (aHeight < 0))
        {
            invertGradient = ETrue;
        }
        CallMethod(gc, &MSwtGc::FillGradientRectangle, rect, aVertical, invertGradient, display);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetAdvanceWidth(JNIEnv*, jclass, jint aDevice, jint aHandle, jchar aChar)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TInt  advWidth = 0;
        CallMethod(advWidth, gc, &MSwtGc::GetAdvanceWidth, aChar, display);
        return advWidth;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetCharWidth(JNIEnv*, jclass, jint aDevice, jint aHandle, jchar aChar)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TInt  width = 0;
        CallMethod(width, gc, &MSwtGc::GetCharWidth, aChar, display);
        return width;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetClippingRect(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect         result(TRect::EUninitialized);
        CallMethod(result, gc, &MSwtGc::ClippingRect, display);
        return NewJavaRectangle(aJniEnv, result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetFont(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        ASSERT(aHandle!=0);
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc*   gc   = reinterpret_cast<MSwtGc*>(aHandle);
        const MSwtFont* font = NULL;
        CallMethod(font, gc, &MSwtGc::Font, display);
        return reinterpret_cast<jint>(font);
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetFontMetrics(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle)
    {
        ASSERT(aHandle!=0);
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc*   gc = reinterpret_cast<MSwtGc*>(aHandle);
        TSwtFontMetrics fontMetrics;
        CallMethod(gc, &MSwtGc::GetFontMetrics, fontMetrics, display);
        jintArray fontMetricsArray = aJniEnv->NewIntArray(5);
        if (!fontMetricsArray)
        {
            return NULL;
        }

        aJniEnv->SetIntArrayRegion(fontMetricsArray, 0, 1, &fontMetrics.iAscent); //lint !e613
        aJniEnv->SetIntArrayRegion(fontMetricsArray, 1, 1, &fontMetrics.iAverageCharWidth); //lint !e613
        aJniEnv->SetIntArrayRegion(fontMetricsArray, 2, 1, &fontMetrics.iDescent); //lint !e613
        aJniEnv->SetIntArrayRegion(fontMetricsArray, 3, 1, &fontMetrics.iHeight); //lint !e613
        aJniEnv->SetIntArrayRegion(fontMetricsArray, 4, 1, &fontMetrics.iLeading); //lint !e613
        return fontMetricsArray;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetLineStyle(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TInt          lineStyle;
        CallMethod(lineStyle, gc, &MSwtGc::LineStyle, display);
        return lineStyle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetLineWidth(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TInt          lineWidth;
        CallMethod(lineWidth, gc, &MSwtGc::LineWidth, display);
        return lineWidth;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1GetXORMode(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TBool         mode;
        CallMethod(mode, gc, &MSwtGc::XORMode, display);
        return ConvertBoolean(mode);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetClippingRect(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TRect   rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(gc, &MSwtGc::SetClippingRect, rect, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1CancelClipping(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, &MSwtGc::CancelClipping, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetFont(JNIEnv*, jclass, jint aDevice, jint aHandle, jint aFontHandle)
    {
        ASSERT(aHandle!=0);
        ASSERT(aFontHandle!=0);
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc*   gc   = reinterpret_cast<MSwtGc*>(aHandle);
        MSwtFont* font = reinterpret_cast<MSwtFont*>(aFontHandle);
        CallMethod(gc, &MSwtGc::SetFont, font, display);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1SetXORMode(JNIEnv*, jclass, jint aDevice, jint aHandle, jboolean aMode)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        CallMethod(gc, &MSwtGc::SetXORMode, aMode, display);
    }

    static void Gc_TextExtentL(TSize& aResult, JNIEnv* aJniEnv, const MSwtGc* aGc, const jstring aText, TInt aFlags, CSwtDisplay* aDevice)
    {
        HBufC* text = ConvertStringLC(aJniEnv, aText);
        TPtr des(text->Des());
        CallMethodL(aResult, aGc, &MSwtGc::TextExtentL, des, aFlags, aDevice);
        CleanupStack::PopAndDestroy(text);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1TextExtent(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jstring aString, jint aFlags)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        const MSwtGc* gc = reinterpret_cast<MSwtGc*>(aHandle);
        TSize extent(TSize::EUninitialized);
        TRAPD(error, Gc_TextExtentL(extent, aJniEnv, gc, aString, aFlags, display));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }
        return NewJavaPoint(aJniEnv, extent.AsPoint());
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_GC_1DrawM2GImage(JNIEnv* aJniEnv, jclass, jint aDevice, jint aHandle, jintArray aBitmapHandles,
            jint aSrcX, jint aSrcY, jint aSrcWidth, jint aSrcHeight, jint aDestX, jint aDestY, jint aDestWidth, jint aDestHeight, jint aUseNativeClear)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtGc*    gc = reinterpret_cast<MSwtGc*>(aHandle);

        TInt* bitmapHandles = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aBitmapHandles, bitmapHandles);
        if (count == 0)
            return;

        TPoint     srcPos(aSrcX, aSrcY);
        TSize      srcSize(aSrcWidth, aSrcHeight);
        TPoint     destPos(aDestX, aDestY);
        TSize      destSize(aDestWidth, aDestHeight);
        TRAP_IGNORE(CallMethod(gc, &MSwtGc::DrawM2GImageL, bitmapHandles, srcPos, srcSize, destPos, destSize, aUseNativeClear, display));
        delete[] bitmapHandles;
    }


    /*
     * Class Image
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1New(JNIEnv* aJniEnv, jclass, jint aDevice, jint aWidth, jint aHeight)
    {
        ASwtDisplayBase* display = DisplayFromDevice(aDevice);
        TSize        size(aWidth, aHeight);
        MSwtImage*   result  = NULL;
        TRAPD(error, CallMethodL(result, display, &ASwtDisplayBase::NewImageL, size, static_cast<CSwtDisplay*>(display)));
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT_USE_DISPLAY(result, Image, display);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1NewFromData(JNIEnv* aJniEnv, jclass, jint aDevice, jobject aData)
    {
        // Convert image data
        MSwtImageData* data = NULL;
        TRAPD(error, (data=CSwtImageData::NewL(aJniEnv,aData)));
        if (data == NULL)
        {
            ThrowIfError(error, aJniEnv);
            return 0;
        }

        // Create the image
        ASwtDisplayBase* display = DisplayFromDevice(aDevice);
        MSwtImage*   result  = NULL;
        TRAP(error, CallMethodL(result, display, &ASwtDisplayBase::NewImageFromDataL, *data, static_cast<CSwtDisplay*>(display)));
        delete data;
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT_USE_DISPLAY(result, Image, display);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1Dispose(JNIEnv*, jclass, jint aDevice, jint aHandle)
    {
        CSwtDisplay* display = DisplayFromDevice(aDevice);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandle);
        CallMethod(image, &MSwtImage::Dispose, display);

        DECREASE_INSTANCE_COUNT_USE_DISPLAY(Image, display);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1GetBounds(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandle);
        TRect            result(TRect::EUninitialized);
        CallMethod(result, image, &MSwtImage::GetBounds);
        return NewJavaRectangle(aJniEnv, result);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1GetData(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandle);
        MSwtImageData* data = NULL;
        TRAPD(error, CallMethodL(data, image, &MSwtImage::GetImageDataL));
        if (data == NULL)
        {
            ThrowIfError(error, aJniEnv);
            return NULL;
        }

        jobject result = NewJavaImageData(aJniEnv, *data);
        DeleteInUiHeap(data);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1AddRef(JNIEnv *, jclass, jint aHandle)
    {
        ASSERT(aHandle);
        const MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandle);
        CallMethod(static_cast<const MSwtRefCounted*>(image), &MSwtImage::AddRef);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Image_1RemoveRef(JNIEnv *, jclass, jint aHandle)
    {
        ASSERT(aHandle);
        const MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandle);
        CallMethod(static_cast<const MSwtRefCounted*>(image), &MSwtImage::RemoveRef);
    }


    /*
     * Class Menu
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aStyle)
    {
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtMenu*    result  = NULL;
        TRAPD(error, CallMethodL(result, &display.Factory(), &MSwtFactory::NewMenuL, display, peerRef, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, Menu);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aHandle);
        TSwtPeer  peerRef;
        CallMethod(peerRef, menu, &MSwtMenu::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(Menu);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1SetEnabled(JNIEnv*, jclass, jint aHandle, jboolean aEnabled)
    {
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aHandle);
        CallMethod(menu, &MSwtMenu::SetEnabled, aEnabled);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1SetVisible(JNIEnv*, jclass, jint aHandle, jboolean aVisible)
    {
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aHandle);
        CallMethod(menu, &MSwtMenu::SetVisible, aVisible);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1SetDefaultItem
    (JNIEnv *, jclass, jint aHandle, jint aMenuItemHandle)
    {
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aHandle);
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aMenuItemHandle);
        CallMethod(menu, &MSwtMenu::SetDefaultItem, menuItem);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Menu_1SetLocation(JNIEnv *, jclass, jint aHandle, jint aX, jint aY)
    {
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aHandle);
        CallMethod(menu, &MSwtMenu::SetLocation, aX, aY);
    }


    /*
     * Class MenuItem
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aIndex, jint aStyle)
    {
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtMenu* parent = reinterpret_cast<MSwtMenu*>(aParent);
        MSwtMenuItem* result = NULL;
        TRAPD(error, CallMethodL(result, &display.Factory(), &MSwtFactory::NewMenuItemL, display, peerRef, *parent, aIndex, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, MenuItem);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        TSwtPeer peerRef;
        CallMethod(peerRef, menuItem, &MSwtMenuItem::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(MenuItem);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1GetSelection(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        TBool result;
        CallMethod(result, menuItem, &MSwtMenuItem::GetSelection);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetAccelerator(JNIEnv* aJniEnv, jclass, jint aHandle, jint aAccelerator)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        TRAPD(error, CallMethodL(menuItem, &MSwtMenuItem::SetAcceleratorL, aAccelerator));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetEnabled(JNIEnv*, jclass, jint aHandle, jboolean aEnabled)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        CallMethod(menuItem, &MSwtMenuItem::SetEnabled, aEnabled);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetImage(JNIEnv* aJniEnv, jclass, jint aHandle, jint aImageHandle)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TRAPD(error, CallMethodL(menuItem, &MSwtMenuItem::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetMenu(JNIEnv* aJniEnv, jclass, jint aHandle, jint aMenuHandle)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        MSwtMenu* menu = reinterpret_cast<MSwtMenu*>(aMenuHandle);
        TRAPD(error, CallMethodL(menuItem, &MSwtMenuItem::SetMenuL, menu));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetSelection(JNIEnv*, jclass, jint aHandle, jboolean aSelected)
    {
        MSwtMenuItem* menuItem = reinterpret_cast<MSwtMenuItem*>(aHandle);
        CallMethod(menuItem, &MSwtMenuItem::SetSelection, aSelected);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MenuItem_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtMenuItem::SetTextL, aText);
    }

    static void MessageBox_OpenL(JNIEnv* aJniEnv, TSwtPeer aPeer,
                                 const jstring aTitle, const jstring aMessage, TInt aStyle)
    {
        HBufC* message = ConvertStringLC(aJniEnv, aMessage);
        HBufC* title   = ConvertStringLC(aJniEnv, aTitle);

        CSwtDisplay& display = CSwtDisplay::Current();

        display.RequestRunDialogL(aPeer, CSwtDialogBroker::EMessageBox, aStyle, *title, *message);

        CleanupStack::PopAndDestroy(2, message);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_MessageBox_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jstring aTitle,
        jstring aMessage,
        jint aStyle)
    {
        TRAPD(error, MessageBox_OpenL(aJniEnv, reinterpret_cast<TSwtPeer>(aPeer), aTitle, aMessage, aStyle));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Scrollable
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Scrollable_1ControlHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtScrollable* scrollable = reinterpret_cast<MSwtScrollable*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, scrollable, &MSwtScrollable::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Scrollable_1CreateScrollBar(JNIEnv* aJniEnv, jclass, jint aHandle, jobject aPeer, jint aStyle)
    {
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtScrollable* scrollable = reinterpret_cast<MSwtScrollable*>(aHandle);
        MSwtScrollBar*  result     = NULL;
        TRAPD(error, CallMethodL(result, scrollable, &MSwtScrollable::CreateScrollBarL, peerRef, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, ScrollBar);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Scrollable_1SetVisibleScrollBar(JNIEnv* /*aJniEnv*/, jclass, jint aHandle, jint aStyle, jboolean aVisible)
    {
        MSwtScrollable* scrollable = reinterpret_cast<MSwtScrollable*>(aHandle);
        CallMethod(scrollable, &MSwtScrollable::SetVisibleScrollBar, aStyle, aVisible);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Scrollable_1ComputeTrim(JNIEnv* aJniEnv, jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
    {
        const MSwtScrollable* scrollable = reinterpret_cast<MSwtScrollable*>(aHandle);
        TRect           rect(TPoint(aX,aY), TSize(aWidth,aHeight));
        CallMethod(scrollable, &MSwtScrollable::ComputeTrim, rect);
        return NewJavaRectangle(aJniEnv, rect);
    }


    /*
     * Class ScrollBar
     */
    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TSwtPeer       peerRef;
        CallMethod(peerRef, scrollbar, &MSwtScrollBar::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(ScrollBar);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetSize(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TSize result(TSize::EUninitialized);
        CallMethod(result, scrollbar, &MSwtScrollBar::GetSize);
        return NewJavaPoint(aJniEnv, result.AsPoint());
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetEnabled(JNIEnv*, jclass, jint aHandle, jboolean aEnabled)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        CallMethod(scrollbar, &MSwtScrollBar::SetEnabled, aEnabled);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetVisible(JNIEnv*, jclass, jint aHandle, jboolean aVisible)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        CallMethod(scrollbar, &MSwtScrollBar::SetVisible, aVisible);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetThumbPosition(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetThumbPosition);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetThumbPosition(JNIEnv* aJniEnv, jclass, jint aHandle, jint aPosition)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TRAPD(error, CallMethodL(scrollbar, &MSwtScrollBar::SetThumbPositionL, aPosition));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetThumbSpan(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetThumbSpan);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetThumbSpan(JNIEnv* aJniEnv, jclass, jint aHandle, jint aSpan)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TRAPD(error, CallMethodL(scrollbar, &MSwtScrollBar::SetThumbSpanL, aSpan));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetMinimum(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetMinimum);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetMinimum(JNIEnv* aJniEnv, jclass, jint aHandle, jint aMinimum)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TRAPD(error, CallMethodL(scrollbar, &MSwtScrollBar::SetMinimumL, aMinimum));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetMaximum(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetMaximum);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetMaximum(JNIEnv* aJniEnv, jclass, jint aHandle, jint aMaximum)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TRAPD(error, CallMethodL(scrollbar, &MSwtScrollBar::SetMaximumL, aMaximum));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetIncrement(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetIncrement);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetIncrement(JNIEnv* , jclass, jint aHandle, jint aIncrement)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        CallMethod(scrollbar, &MSwtScrollBar::SetIncrement, aIncrement);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1GetPageIncrement(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtScrollBar::GetPageIncrement);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetPageIncrement(JNIEnv* , jclass, jint aHandle, jint aPageIncrement)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        CallMethod(scrollbar, &MSwtScrollBar::SetPageIncrement, aPageIncrement);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ScrollBar_1SetValues(JNIEnv* aJniEnv, jclass, jint aHandle, jint aSelection, jint aMinimum, jint aMaximum, jint aThumb, jint aIncrement, jint aPageIncrement)
    {
        MSwtScrollBar* scrollbar = reinterpret_cast<MSwtScrollBar*>(aHandle);
        TRAPD(error, CallMethodL(scrollbar, &MSwtScrollBar::SetValuesL, aSelection, aMinimum, aMaximum, aThumb, aIncrement, aPageIncrement));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Shell
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1New
    (JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtShell*   parent  = reinterpret_cast<MSwtShell*>(aParent);
        MSwtShell*   result  = NULL;
        TRAPD(error, CallMethodL(result, &display.Factory(),
                                 &MSwtFactory::NewShellL, display, peerRef, parent, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, Shell);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1DecorationsHandle(
        JNIEnv*, jclass, jint aHandle)
    {
        MSwtShell*       shell = reinterpret_cast<MSwtShell*>(aHandle);
        MSwtDecorations* result;
        CallMethod(result, shell, &MSwtShell::Decorations);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1ForceActive(
        JNIEnv*, jclass, jint aHandle)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        CallMethod(shell, &MSwtShell::ForceActive);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1GetImeInputMode(
        JNIEnv*, jclass, jint aHandle)
    {
        const MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        TInt imeInputMode(0);

        CallMethod(imeInputMode,shell, &MSwtShell::GetImeInputMode);
        return imeInputMode;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1GetMinimumSize(
        JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        TSize minimumSize(TSize::EUninitialized);
        CallMethod(minimumSize,shell, &MSwtShell::GetMinimumSize);
        return NewJavaPoint(aJniEnv, minimumSize.AsPoint());
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1GetShells(
        JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        CSwtPeerArray* children  = NULL;
        TRAPD(error, CallMethodL(children, shell, &MSwtShell::GetChildShellsPeersL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaObjectArray(aJniEnv, children, "org/eclipse/swt/widgets/Shell");
        DeleteInUiHeap(children);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1SetMinimumSize(
        JNIEnv*, jclass, jint aHandle, jint aWidth, jint aHeight)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        TSize minimumSize(aWidth,aHeight);
        CallMethod(shell, &MSwtShell::SetMinimumSize, minimumSize);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1SetImeInputMode(
        JNIEnv*, jclass, jint aHandle, jint aInputMode)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        CallMethod(shell, &MSwtShell::SetImeInputMode, aInputMode);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1GetDefaultBounds(
        JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        TRect rect(TRect::EUninitialized);
        CallMethod(rect, shell, &MSwtShell::DefaultBounds);
        return NewJavaRectangle(aJniEnv, rect);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Shell_1SetAsyncPainting(
        JNIEnv*, jclass, jint aHandle, jboolean aStatus)
    {
        MSwtShell* shell = reinterpret_cast<MSwtShell*>(aHandle);
        CallMethod(shell, &MSwtShell::SetAsyncPainting, aStatus);
    }


    /*
     * Class Label
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1ControlHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtLabel* label = reinterpret_cast<MSwtLabel*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, label, &MSwtLabel::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewLabelL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Label);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1GetAlignment(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        const MSwtLabel* theLabel = reinterpret_cast<MSwtLabel*>(aHandle);
        TInt result;
        CallMethod(result, theLabel, &MSwtLabel::GetAlignment);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1SetAlignment(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aAlignment)
    {
        MSwtLabel* theLabel = reinterpret_cast<MSwtLabel*>(aHandle);
        CallMethod(theLabel, &MSwtLabel::SetAlignment, aAlignment);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1SetImage(JNIEnv* aJniEnv, jclass, jint aHandle, jint aHandleImage)
    {
        MSwtLabel* theControl = reinterpret_cast<MSwtLabel*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandleImage);
        TRAPD(error, CallMethodL(theControl, &MSwtLabel::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Label_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtLabel::SetTextL, aString);
    }


    /*
     * Class Button
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1ControlHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, theButton, &MSwtButton::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewButtonL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Button);
        return handle;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1GetSelection(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        TBool selection = EFalse;
        const MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        CallMethod(selection, theButton, &MSwtButton::GetSelection);
        return ConvertBoolean(selection);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1SetSelection(JNIEnv* /*aJniEnv*/, jclass, jint aHandle, jboolean aActivedState)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        CallMethod(theButton, &MSwtButton::SetSelection, aActivedState);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1GetAlignment(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        TInt result;
        const MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        CallMethod(result, theButton, &MSwtButton::GetAlignment);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1SetAlignment(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aAlignment)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        CallMethod(theButton, &MSwtButton::SetAlignment, aAlignment);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1SetImage(JNIEnv* aJniEnv, jclass, jint aHandle, jint aHandleImage)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aHandleImage);
        TRAPD(error, CallMethodL(theButton, &MSwtButton::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtButton::SetTextL, aString);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1SetDefault(JNIEnv *, jclass, jint aHandle, jboolean aIsDefault)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        CallMethod(theButton, &MSwtButton::SetDefault, aIsDefault);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Button_1Click(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtButton* theButton = reinterpret_cast<MSwtButton*>(aHandle);
        TRAPD(error, CallMethodL(theButton, &MSwtButton::ClickL));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Link
     */

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Link_1ControlHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtLink* link = reinterpret_cast<MSwtLink*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, link, &MSwtLink::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Link_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewLinkL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Link);
        return handle;
    }

    static void Link_SetTextL(
        JNIEnv* aJniEnv,
        jint aHandle,
        jobjectArray aTextFragments,
        jobjectArray aLinkTargets)
    {
        MSwtLink* link = reinterpret_cast<MSwtLink*>(aHandle);
        CDesCArray* fragments = ConvertStringArrayL(aJniEnv, aTextFragments);
        CleanupStack::PushL(fragments);
        CDesCArray* targets = ConvertStringArrayL(aJniEnv, aLinkTargets);
        CleanupStack::PushL(targets);
        CallMethodL(link, &MSwtLink::SetLinkDataL, fragments, targets);
        CleanupStack::PopAndDestroy(targets);
        CleanupStack::PopAndDestroy(fragments);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Link_1SetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aTextFragments,
        jobjectArray aLinkTargets)
    {
        TRAPD(result, Link_SetTextL(aJniEnv, aHandle,
                                    aTextFragments, aLinkTargets));
        ThrowIfError(result, aJniEnv);
    }

    /*
     * Class List
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewListL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, List);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1ScrollableHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        MSwtScrollable*      result;
        CallMethod(result, theList, &MSwtList::Scrollable);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Add__ILjava_lang_String_2(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtList::AppendL, aString);
    }

    static void List_AddL(JNIEnv* aJniEnv, MSwtList* aList, const jstring aString, TInt aIndex)
    {
        HBufC* text = ConvertStringLC(aJniEnv, aString);
        CallMethodL(aList, &MSwtList::InsertL, aIndex, *text);
        CleanupStack::PopAndDestroy(text);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Add__ILjava_lang_String_2I(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString, jint aIndex)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, List_AddL(aJniEnv, theList, aString, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Deselect__I_3I(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::DeselectItems, ints, count);
        delete[] ints;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Deselect__II(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::DeselectItem, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Deselect__III(JNIEnv*, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::DeselectRange, aStart, aEnd);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1DeselectAll(JNIEnv*, jclass, jint aHandle)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::DeselectAll);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetFocusIndex(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result = 0;
        CallMethod(result, theList, &MSwtList::GetFocusIndex);
        return result;
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetItem(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TPtrC result;
        TRAPD(error, CallMethodL(theList, &MSwtList::GetItemL, aIndex, result));
        if (error == KErrNone)
        {
            return NewJavaString(aJniEnv, result);
        }
        else
        {
            Throw(error, aJniEnv);
            return 0;
        }
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetItemCount(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result = 0;
        CallMethod(result, theList, &MSwtList::GetItemCount);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetItemHeight(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result = 0;
        CallMethod(result, theList, &MSwtList::GetItemHeight);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetSelectionCount(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result;
        CallMethod(result, theList, &MSwtList::GetSelectionCount);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetSelectionIndex(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result;
        CallMethod(result, theList, &MSwtList::GetSelectionIndex);
        return result;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetSelectionIndices(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        const CArrayFix<TInt>* selectionArray;
        CallMethod(selectionArray, theList, &MSwtList::GetSelectionIndices);
        TInt count = (selectionArray!=NULL) ? selectionArray->Count() : 0;
        jintArray result = aJniEnv->NewIntArray(count);
        if (!result)
        {
            return NULL;
        }

        for (TInt i=0; i<count; ++i)
        {
            TInt value = (*selectionArray)[i]; //lint !e613
            aJniEnv->SetIntArrayRegion(result, i, 1, &value);
        }
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1GetTopIndex(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result;
        CallMethod(result, theList, &MSwtList::GetTopIndex);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1IndexOf(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString, jint aStart)
    {
        HBufC* string = NULL;
        TRAPD(error, (string=ConvertStringL(aJniEnv, aString)));
        if (string == NULL)
        {
            Throw(error, aJniEnv);
            return 0;
        }

        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TInt result;
        CallMethod(result, theList, &MSwtList::IndexOf, *string, aStart);
        delete string;
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1IsSelected(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        const MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TBool result = EFalse;
        CallMethod(result, theList, &MSwtList::IsSelected, aIndex);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Remove__I_3I(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::RemoveItemsL, ints, count));
        ThrowIfError(error, aJniEnv);
        delete[] ints;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Remove__II(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::RemoveItemL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Remove__III(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::RemoveRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1RemoveAll(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::RemoveAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Select__IIZ(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex, jboolean aScroll)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::SelectItemL, aIndex, aScroll));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1Select__III(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::SelectRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1SelectAll(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAPD(error, CallMethodL(theList, &MSwtList::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1SetFocusIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::SetFocusIndex, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1SetItems(JNIEnv* aJniEnv, jclass, jint aHandle, jobjectArray aStringArray)
    {
        CDesCArray* resultArray = NULL;
        TRAPD(error, (resultArray=ConvertStringArrayL(aJniEnv, aStringArray)));
        if (error != KErrNone || resultArray == NULL)
        {
            delete resultArray;
            ThrowIfError(error, aJniEnv);
            return;
        }

        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        TRAP(error, CallMethodL(theList, &MSwtList::SetItemsL, resultArray));
        ThrowIfError(error, aJniEnv);
        delete resultArray;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1SetTopIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::SetTopIndex, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_List_1ShowSelection(JNIEnv*, jclass, jint aHandle)
    {
        MSwtList* theList = reinterpret_cast<MSwtList*>(aHandle);
        CallMethod(theList, &MSwtList::ShowSelection);
    }


    /*
     * Class Text
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewTextL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Text);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1ScrollableHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtText*       text = reinterpret_cast<MSwtText*>(aHandle);
        MSwtScrollable* scrollable;
        CallMethod(scrollable, text, &MSwtText::Scrollable);
        return reinterpret_cast<jint>(scrollable);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetText(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        return CallTextGetterThrow(aJniEnv, aHandle, &MSwtText::GetTextL);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetTextRange(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        const MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        HBufC* text = NULL;
        TRAPD(error, CallMethodL(text, textCtrl, &MSwtText::GetTextRangeL, aStart, aEnd));
        if (text == NULL)
        {
            ThrowIfError(error, aJniEnv);
            return NULL;
        }

        jstring result = NewJavaString(aJniEnv, *text);
        DeleteInUiHeap(text);
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtText::SetTextL, aText, EFalse));
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1Append(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtText::AppendL, aText, EFalse));
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1Insert(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtText::InsertL, aText, EFalse));
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetCharCount(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetCharCount);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetTextLimit(JNIEnv* aJniEnv, jclass, jint aHandle, jint aLimit)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TBool result;
        TRAPD(error,CallMethodL(result, textCtrl, &MSwtText::SetTextLimitL, aLimit));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetCaretLineNumber(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetCaretLineNumber);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetCaretLocation(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TPoint point(0,0);
        TRAPD(error, CallMethodL(point, textCtrl, &MSwtText::GetCaretLocationL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }
        return NewJavaPoint(aJniEnv, point);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetCaretPosition(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetCaretPosition);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetSelection(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        TPoint point(CallNonLeavingCoordsGetter(aHandle, &MSwtText::GetSelection));
        return NewJavaPoint(aJniEnv, point);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetSelectionText(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        return CallTextGetterThrow(aJniEnv, aHandle, &MSwtText::GetSelectionTextL);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetSelection(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::SetSelectionL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1ClearSelection(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::ClearSelectionL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SelectAll(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1ShowSelection(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::ShowSelectionL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1Copy(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::CopyL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1Paste(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TBool result = EFalse;
        TRAPD(error, CallMethodL(result, textCtrl, &MSwtText::PasteL));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetEditable(JNIEnv *, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtText::GetEditable));
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetEditable(JNIEnv*, jclass, jint aHandle, jboolean aEditable)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        CallMethod(textCtrl, &MSwtText::SetEditable, aEditable);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetLineCount(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetLineCount);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetLineHeight(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TInt result = 0;
        TRAPD(error, CallMethodL(result, textCtrl, &MSwtText::GetLineHeightL));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetTopIndex(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TInt result = 0;
        TRAPD(error, CallMethodL(result, textCtrl, &MSwtText::GetTopIndexL));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetTopIndex(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::SetTopIndexL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetTopPixel(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TInt result = 0;
        TRAPD(error, CallMethodL(result, textCtrl, &MSwtText::GetTopPixelL));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT jchar JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetEchoChar(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetEchoChar);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetEchoChar(JNIEnv* aJniEnv, jclass, jint aHandle, jchar aEcho)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error,CallMethodL(textCtrl, &MSwtText::SetEchoCharL, aEcho));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetDoubleClickEnabled(JNIEnv*, jclass, jint aHandle, jboolean aDoubleClick)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        CallMethod(textCtrl, &MSwtText::SetDoubleClickEnabled, aDoubleClick);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1GetOrientation(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtText::GetOrientation);
    }


    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Text_1SetOrientation(JNIEnv* aJniEnv, jclass, jint aHandle, jint aOrientation)
    {
        MSwtText* textCtrl = reinterpret_cast<MSwtText*>(aHandle);
        TRAPD(error, CallMethodL(textCtrl, &MSwtText::SetOrientationL, aOrientation));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Combo
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewComboL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Combo);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1CompositeHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtCombo* combo = reinterpret_cast<MSwtCombo*>(aHandle);
        MSwtComposite* result;
        CallMethod(result, combo, &MSwtCombo::Composite);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Add__ILjava_lang_String_2(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtCombo::AppendL, aString);
    }

    static void Combo_AddL(JNIEnv* aJniEnv, MSwtCombo* aCombo, const jstring aString, TInt aIndex)
    {
        HBufC* text = ConvertStringLC(aJniEnv, aString);
        CallMethodL(aCombo, &MSwtCombo::InsertL, aIndex, *text);
        CleanupStack::PopAndDestroy(text);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Add__ILjava_lang_String_2I(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString, jint aIndex)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, Combo_AddL(aJniEnv, theCombo, aString, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1ClearSelection(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::ClearSelectionL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Copy(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::CopyL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Deselect(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        CallMethod(theCombo, &MSwtCombo::DeselectItem, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1DeselectAll(JNIEnv*, jclass, jint aHandle)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        CallMethod(theCombo, &MSwtCombo::DeselectAll);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetItem(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TPtrC result;
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::GetItemL, aIndex, result));
        if (error == KErrNone)
        {
            return NewJavaString(aJniEnv, result);
        }
        else
        {
            Throw(error, aJniEnv);
            return 0;
        }
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetItemCount(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TInt result = 0;
        CallMethod(result, theCombo, &MSwtCombo::GetItemCount);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetItemHeight(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TInt result = 0;
        CallMethod(result, theCombo, &MSwtCombo::GetItemHeight);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetItems(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        CPtrCArray* array = NULL;
        TRAPD(error, CallMethodL(array, theCombo, &MSwtCombo::GetItemsL));
        if (error != KErrNone)
        {
            Throw(error, aJniEnv);
            DeleteInUiHeap(array);
            return NULL;
        }
        if (array == NULL)
        {
            return NULL;
        }

        // Construct the array of strings.
        jobjectArray javaStringArray = NULL;
        jclass stringClass = aJniEnv->FindClass("java/lang/String");
        if (stringClass != NULL)
        {
            const TInt count = array->Count();
            javaStringArray = aJniEnv->NewObjectArray(count, stringClass, NULL);
            if (javaStringArray != NULL)
            {
                jstring javaString;
                for (TInt i=0; i<count; ++i)
                {
                    javaString = NewJavaString(aJniEnv, array->At(i));
                    aJniEnv->SetObjectArrayElement(javaStringArray, i, javaString); //lint !e613
                    aJniEnv->DeleteLocalRef(javaString);
                }
            }
            aJniEnv->DeleteLocalRef(stringClass);
        }

        DeleteInUiHeap(array);
        return javaStringArray;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetOrientation(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtCombo::GetOrientation);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetSelection(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        TPoint point(CallNonLeavingCoordsGetter(aHandle, &MSwtCombo::GetSelection));
        return NewJavaPoint(aJniEnv, point);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetSelectionIndex(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtCombo::GetSelectionIndex);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetText(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        return CallTextGetterThrow(aJniEnv, aHandle, &MSwtCombo::GetTextL);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetTextHeight(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TInt result = 0;
        TRAPD(error, CallMethodL(result, theCombo, &MSwtCombo::GetTextHeightL));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetTextLimit(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtCombo::GetTextLimit);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1GetVisibleItemCount(JNIEnv* , jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtCombo::GetVisibleItemCount);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1IndexOf(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString, jint aStart)
    {
        HBufC* string = NULL;
        TRAPD(error, (string=ConvertStringL(aJniEnv, aString)));
        if (error != KErrNone || string == NULL)
        {
            Throw(error, aJniEnv);
            return 0;
        }

        const MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TInt result;
        CallMethod(result, theCombo, &MSwtCombo::IndexOf, *string, aStart);
        delete string;
        return result;
    }

    static TBool Combo_InsertInTextControlL(JNIEnv* aJniEnv, jint aHandle, jstring aString)
    {
        HBufC* string = ConvertStringLC(aJniEnv, aString);
        if (string == NULL)
        {
            return EFalse;
        }

        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TBool result;
        CallMethodL(result, theCombo, &MSwtCombo::InsertInTextControlL, *string);
        CleanupStack::PopAndDestroy(string);
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1InsertInTextControl
    (JNIEnv* aJniEnv, jclass, jint aHandle, jstring aString)
    {
        TBool result = EFalse;
        TRAPD(error, (result = Combo_InsertInTextControlL(aJniEnv, aHandle, aString)));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Paste(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TBool result = EFalse;
        TRAPD(error, CallMethodL(result, theCombo, &MSwtCombo::PasteL));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Remove__II(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::RemoveItemL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Remove__III(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::RemoveRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1RemoveAll(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::RemoveAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1Select(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::SelectItemL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetItems(JNIEnv* aJniEnv, jclass, jint aHandle, jobjectArray aStringArray)
    {
        CDesCArray* resultArray = NULL;
        TRAPD(error, (resultArray=ConvertStringArrayL(aJniEnv, aStringArray)));
        if (error != KErrNone || resultArray == NULL)
        {
            delete resultArray;
            ThrowIfError(error, aJniEnv);
            return;
        }

        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAP(error, CallMethodL(theCombo, &MSwtCombo::SetItemsL, resultArray));
        ThrowIfError(error, aJniEnv);
        delete resultArray;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetOrientation(JNIEnv* aJniEnv, jclass, jint aHandle, jint aOrientation)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::SetOrientationL, aOrientation));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetSelection(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TRAPD(error, CallMethodL(theCombo, &MSwtCombo::SetSelectionL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtCombo::SetTextL, aText, EFalse));
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetTextLimit(JNIEnv*, jclass, jint aHandle, jint aLimit)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        TBool result;
        CallMethod(result, theCombo, &MSwtCombo::SetTextLimit, aLimit);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Combo_1SetVisibleItemCount(JNIEnv*, jclass, jint aHandle, jint aCount)
    {
        MSwtCombo* theCombo = reinterpret_cast<MSwtCombo*>(aHandle);
        CallMethod(theCombo, &MSwtCombo::SetVisibleItemCount, aCount);
    }


    /*
     * Class ProgressBar
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1ControlHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtProgressBar* progressBar = reinterpret_cast<MSwtProgressBar*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, progressBar, &MSwtProgressBar::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewProgressBarL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, ProgressBar);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1GetMaximum(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtProgressBar::GetMaximum);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1GetMinimum(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtProgressBar::GetMinimum);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1GetSelection(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtProgressBar::GetSelection);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1SetMaximum(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aMaximum)
    {
        MSwtProgressBar* theProgressBar = reinterpret_cast<MSwtProgressBar*>(aHandle);
        CallMethod(theProgressBar, &MSwtProgressBar::SetMaximum, aMaximum);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1SetMinimum(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aMinimum)
    {
        MSwtProgressBar* theProgressBar = reinterpret_cast<MSwtProgressBar*>(aHandle);
        CallMethod(theProgressBar, &MSwtProgressBar::SetMinimum, aMinimum);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ProgressBar_1SetSelection(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aSelection)
    {
        MSwtProgressBar* theProgressBar = reinterpret_cast<MSwtProgressBar*>(aHandle);
        CallMethod(theProgressBar, &MSwtProgressBar::SetSelection, aSelection);
    }


    /*
     * Class Slider
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1ControlHandle(JNIEnv*, jclass, jint aHandle)
    {
        MSwtSlider* Slider = reinterpret_cast<MSwtSlider*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, Slider, &MSwtSlider::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewCoreWidget(aJniEnv, &MSwtFactory::NewSliderL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Slider);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1GetMaximum(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtSlider::GetMaximum);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1GetMinimum(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtSlider::GetMinimum);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1GetPageIncrement(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtSlider::GetPageIncrement);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1GetIncrement(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtSlider::GetIncrement);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1GetSelection(JNIEnv* /*aJniEnv*/, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtSlider::GetSelection);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetMaximum(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aMaximum)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetMaximum, aMaximum);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetMinimum(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aMinimum)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetMinimum, aMinimum);
    }


    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetIncrement(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aIncrement)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetIncrement, aIncrement);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetPageIncrement(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aPageIncrement)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetPageIncrement, aPageIncrement);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetSelection(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aSelection)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetSelection, aSelection);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_Slider_1SetValues(JNIEnv* /*aJniEnv*/, jclass , jint aHandle, jint aSelection, jint aMinimum, jint aMaximum, jint aIncrement, jint aPageIncrement)
    {
        MSwtSlider* theSlider = reinterpret_cast<MSwtSlider*>(aHandle);
        CallMethod(theSlider, &MSwtSlider::SetValues, aSelection, aMinimum, aMaximum, aIncrement, aPageIncrement);
    }


    /*
     * Class FileDialog
     */
    static void FileDialog_OpenL(
        JNIEnv* aJniEnv,
        TSwtPeer aPeer,
        jint aStyle,
        jstring aTitle,
        jstring aInitPath)
    {
        // Set initial path & title
        HBufC* initPath = aInitPath ?
                          ConvertStringLC(aJniEnv, aInitPath) :
                          HBufC::NewLC(0);
        HBufC* title = aTitle ?
                       ConvertStringLC(aJniEnv, aTitle) :
                       HBufC::NewLC(0);

        TInt dialogType = CSwtDialogBroker::EFileDialog;
        CSwtDisplay& display = CSwtDisplay::Current();
        display.RequestRunDialogL(aPeer, dialogType, aStyle, *title, *initPath);

        CleanupStack::PopAndDestroy(title);
        CleanupStack::PopAndDestroy(initPath);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_FileDialog_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jstring aInitPath,
        jstring aTitle,
        jint aStyle)
    {
        TRAPD(error,
              FileDialog_OpenL(
                  aJniEnv,
                  reinterpret_cast<TSwtPeer>(aPeer),
                  aStyle,
                  aTitle,
                  aInitPath
              ));
        ThrowIfError(error, aJniEnv);
    }


    /*
     *  Class ImageDataLoader
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1New(JNIEnv *aJniEnv, jclass)
    {
        // TRAPD(error, (result = MSwtImageDataLoader::NewImageDataLoaderL));
        MSwtImageDataLoader* result=NULL;
        TRAPD(error, result = CSwtImageDataLoader::NewL());
        ThrowIfError(error, aJniEnv);
        return reinterpret_cast<jint>(result);
    }


    static void ImageDataLoader_EncodeImageL(JNIEnv* aJniEnv, MSwtImageDataLoader* aLoader, jobject aImageData, jint aFormat, const jstring aFilename)
    {
        // Convert image data
        CSwtImageData* data = CSwtImageData::NewL(aJniEnv, aImageData);
        CleanupStack::PushL(data);

        HBufC* convertedFilename = ConvertStringLC(aJniEnv, aFilename);

        aLoader->EncodeImageToFileL(*data, aFormat, *convertedFilename);

        CleanupStack::PopAndDestroy(2, data); // convertedFilename, data
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1EncodeImage(JNIEnv* aJniEnv, jclass, jint aHandle, jobject aImageData, jint aFormat, jstring aFilename)
    {
        MSwtImageDataLoader* loader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        TRAPD(error, ImageDataLoader_EncodeImageL(aJniEnv, loader, aImageData, aFormat, aFilename));
        ThrowIfError(error, aJniEnv);
    }

    static void ImageDataLoader_EncodeImageToStreamL(JNIEnv* aJniEnv, MSwtImageDataLoader* aLoader, jobject aImageData, jint aFormat, jobject aOutputStream)
    {
        // Convert image data
        CSwtImageData* data = CSwtImageData::NewL(aJniEnv,aImageData);
        CleanupStack::PushL(data);

        HBufC8* buffer = NULL;
        buffer = aLoader->EncodeImageToStreamL(*data, aFormat);
        CleanupStack::PopAndDestroy(data);

        // Now: fill the OutputStream
        jclass clazz = aJniEnv->GetObjectClass(aOutputStream);
        if (clazz != NULL)
        {
            jmethodID id = aJniEnv->GetMethodID(clazz, "write", "([B)V");
            aJniEnv->DeleteLocalRef(clazz);
            if (id != NULL)
            {

                jbyteArray byte = NewJavaByteArray(aJniEnv, *buffer);
                if (byte != NULL)
                {
                    aJniEnv->CallVoidMethod(aOutputStream, id, byte);
                    aJniEnv->DeleteLocalRef(byte);
                }
            }
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1EncodeImageToStream(JNIEnv* aJniEnv, jclass, jint aHandle, jobject aImageData, jint aFormat, jobject aOutputStream)
    {
        MSwtImageDataLoader* loader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        TRAPD(error, ImageDataLoader_EncodeImageToStreamL(aJniEnv, loader, aImageData, aFormat, aOutputStream));
        ThrowIfError(error, aJniEnv);
    }

    static jobjectArray ImageDataLoader_DecodeImageL(JNIEnv* aJniEnv, MSwtImageDataLoader* aLoader, const jstring aFilename)
    {
        HBufC* convertedFilename = ConvertStringLC(aJniEnv, aFilename);

        aLoader->DecodeImageL(*convertedFilename);
        CleanupStack::PopAndDestroy(convertedFilename);

        CSwtImageDataArray* data;
        data = aLoader->GetImageData();

        return NewJavaImageDataArray(aJniEnv, data);
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1DecodeImage(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aFilename)
    {
        MSwtImageDataLoader* loader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        jobjectArray result = NULL;
        TRAPD(error, (result=ImageDataLoader_DecodeImageL(aJniEnv, loader, aFilename)));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    static jobjectArray ImageDataLoader_DecodeWholeImageFromBufferL(JNIEnv* aJniEnv, MSwtImageDataLoader* aLoader, jbyteArray aBuffer)
    {
        HBufC8* buffer = ConvertByteArrayL(aJniEnv, aBuffer);
        CleanupStack::PushL(buffer);

        aLoader->DecodeWholeImageFromBufferL(*buffer);
        CleanupStack::PopAndDestroy(buffer);

        CSwtImageDataArray* data;
        data = aLoader->GetImageData();
        if (!data)
        {
            return NULL;
        }
        return NewJavaImageDataArray(aJniEnv, data);
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1DecodeWholeImageFromBuffer(JNIEnv* aJniEnv, jclass, jint aHandle, jbyteArray aBuffer)
    {
        MSwtImageDataLoader* loader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        jobjectArray result = NULL;
        TRAPD(error, (result=ImageDataLoader_DecodeWholeImageFromBufferL(aJniEnv, loader, aBuffer)));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    typedef void (MSwtImageDataLoader::*TBufferDecoder)(const TDesC8&);

    static void ImageDataLoader_DecodeBufferL(JNIEnv* aJniEnv, MSwtImageDataLoader& aLoader, TBufferDecoder aDecodeMethodL, jbyteArray aBuffer)
    {
        HBufC8* buffer = ConvertByteArrayL(aJniEnv, aBuffer);
        CleanupStack::PushL(buffer);

        (aLoader.*aDecodeMethodL)(*buffer);

        CleanupStack::PopAndDestroy(buffer);
    }

    static void ImageDataLoader_DecodeBufferThrow(JNIEnv* aJniEnv, jint aHandle, TBufferDecoder aDecodeMethodL, jbyteArray aBuffer)
    {
        MSwtImageDataLoader* loader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        TRAPD(error, ImageDataLoader_DecodeBufferL(aJniEnv, *loader, aDecodeMethodL, aBuffer));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1StartImageDecoding(JNIEnv* aJniEnv, jclass, jint aHandle, jbyteArray aBuffer)
    {
        ImageDataLoader_DecodeBufferThrow(aJniEnv, aHandle, &MSwtImageDataLoader::DecodeImageFromBufferL, aBuffer);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1AppendData(JNIEnv* aJniEnv, jclass, jint aHandle, jbyteArray aBuffer)
    {
        ImageDataLoader_DecodeBufferThrow(aJniEnv, aHandle, &MSwtImageDataLoader::AppendDataL, aBuffer);
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1GetImageData(JNIEnv *aJniEnv, jclass , jint aHandle)
    {
        CSwtImageDataArray* data = NULL;
        MSwtImageDataLoader* imageDataLoader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        data = imageDataLoader->GetImageData();
        return NewJavaImageDataArray(aJniEnv, data);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1Dispose(JNIEnv*, jclass, jint aHandle)
    {
        MSwtImageDataLoader* imageDataLoader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        imageDataLoader->Dispose();
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1GetLogicalScreenHeight(JNIEnv *, jclass, jint aHandle)
    {
        MSwtImageDataLoader* imageDataLoader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        TInt result;
        result = imageDataLoader->GetLogicalScreenHeight();
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_swt_internal_symbian_OS_ImageDataLoader_1GetLogicalScreenWidth(JNIEnv *, jclass, jint aHandle)
    {
        MSwtImageDataLoader* imageDataLoader = reinterpret_cast<MSwtImageDataLoader*>(aHandle);
        TInt result;
        result = imageDataLoader->GetLogicalScreenWidth();
        return result;
    }

} // extern "C"
