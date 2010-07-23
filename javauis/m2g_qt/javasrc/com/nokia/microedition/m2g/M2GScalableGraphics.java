/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.microedition.m2g.M2GRunnableQt;
import javax.microedition.m2g.*;
import javax.microedition.m2g.ScalableGraphics;
import javax.microedition.lcdui.Graphics;
import java.lang.StringBuffer;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;


/*
 * ScalableGraphics
 */
public class M2GScalableGraphics
        extends M2GObject
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    // Exception text
    /* Optimization: static finals changed to local variables
    private static final String ALPHA_OUT_OF_RANGE_ESTR =
        "The alpha is out of range";
    private static final String INVALID_TARGET_ESTR =
        "The target is invalid";
    private static final String MODE_INVALID_ESTR =
        "The mode is invalid";
    private static final String NULL_IMAGE_ESTR =
        "The image is null";
    private static final String NULL_TARGET_ESTR =
        "The target is null";
    private static final String TARGET_ALREADY_BOUND_ESTR =
        "Target is already bound";
    private static final String TARGET_NOT_BOUND_ESTR =
        "Target is not bound";
    */
    // Transparency alpha max and min limits
    private static final float MAX_TRANSPARENCY_ALPHA = 1.0f;
    private static final float MIN_TRANSPARENCY_ALPHA = 0.0f;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private GC        iTargetGC      = null;
    private int iUseNativeClear = 0;
    private Graphics iTargetGraphics;
    
     int iSurfaceHandle;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     */
    public M2GScalableGraphics()
    {
        super();
        System.out.println("P-Constructor M2GScalableGraphics() called");
        doConstruct();
    }
    /**
     * @see javax.microedition.m2g.ScalableGraphics#bindTarget()
     */
    public synchronized void bindTarget(Object target)
    {
    	System.out.println("P-Bind Target called");
				if (target instanceof org.eclipse.swt.graphics.GC)
        {
        	final Object finalTarget = target;   
        	// Execute in UI thread     
        	Platform.executeInUIThread(
                new M2GRunnableQt() {
                    public void doRun() {
                    	System.out.println("P-Bind Target Thread called");
                    	// Get GCData from GC
                        GCData gcData = ((org.eclipse.swt.graphics.GC)finalTarget).getGCData();    
                      System.out.println("P-Bind GCData Create");
                    	// Get internalGC (Graphicscontext), WindowSurface and the WindowSurface handle for native access
                        iSurfaceHandle = gcData.internalGc.getWindowSurface().getHandle();
                      System.out.println("P-Bind isurfacehandle Created " );
                      _bind(getHandle(), iSurfaceHandle);
                      }
                      });
                    //  currentTarget = target;
                    //We are mentaining seperate current Target as iTargetGC and iTargetGraphics
											iTargetGC = (GC)finalTarget;
		    // Handling for LCDUI Graphics
		    //
		    }
//Change accordingly to M3G not as site content.		    
		    else if (target instanceof javax.microedition.lcdui.Graphics)
		    {
		    	Graphics g = (Graphics)target;
		    	
		    	final Graphics finalG = g;
		    	
		    	 // Execute in UI thread     
			        Platform.executeInUIThread(
			                new M2GRunnableQt() {
			                    public void doRun() {
														System.out.println("P-Bind Graphics Case" );			                    	
			                    	LCDUIInvoker.startExternalRendering( finalG );  
			                    	System.out.println("P-Bind Invoker External rengdering Set Case" );			                    	
                        		iSurfaceHandle = LCDUIInvoker.getWindowSurface(finalG).getHandle();
                        		System.out.println("P-Bind Invoker External rengdering Set Bind called. Case" );			                    	
		                        _bind(getHandle(), iSurfaceHandle);

			                    }
			                  });
			                  
			                  iTargetGraphics  = (Graphics)finalG;
		    	}
			         
	         else {
			        throw new IllegalArgumentException();
			    }
    
  }
    	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doConstruct()
     */
    public void doConstruct()
    {
      super.doConstruct();
    	 // Execute in UI thread     
	    Platform.executeInUIThread(
            new M2GRunnableQt() {
                public void doRun() {
										        setHandle(_createRenderContext(
										                      getNativeSVGProxyHandle() ));
										        // Add object to the live objects container
										      }
										    });
        register(this);
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    public synchronized void doCleanup()
    {
    	
 		    	 // Execute in UI thread     
			        Platform.executeInUIThread(
			                new M2GRunnableQt() {
			                    public void doRun() {  	
																			        _deleteRenderContext(
																			            getHandle() );
																			          }
																			       }
																			    );      
        resetHandles();
        iTargetGC = null;
        iTargetGraphics = null;
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#releaseTarget()
     */
    public synchronized void releaseTarget()
    {
    	System.out.println("P-In Release Target" );	
      if (iTargetGC != null)
      {
      System.out.println("P-In Release Target graphics.GC" );	
			Platform.executeInUIThread(
            new M2GRunnableQt() {
             public void doRun() {
            	      				      _release(iSurfaceHandle, getHandle());
            	      				    }
            	      				  });
            iTargetGC = null;
        }
			else if (iTargetGraphics != null )
		    {
				
				System.out.println("P-In Release Target M2GScalableGraphics.. everything is fine... thread complete...." );	
				final Graphics finalG = iTargetGraphics;
				Platform.executeInUIThread(
	            new M2GRunnableQt() {
	             public void doRun() {
	            			_release(iSurfaceHandle, getHandle());
	            			LCDUIInvoker.endExternalRendering( finalG );
	            		}
	            	});
				System.out.println("P-In Release Target Thread Completed..." );	
        iTargetGraphics = null;
        }
      else 
      	{
      			System.out.println("P-In Release Target else case.." );	
      		   // check for invalid Graphics TODO this exception is getting trough when we trying to bind.s
      		   
         //       throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
      	}

    }
    public synchronized void render(
        int x, int y, ScalableImage image, boolean aUseNativeClear)
    {
				System.out.println("P-In Render M2GScalableGraphics.. erender(int x, int y, ScalableImage image, boolean aUseNativeClear)" );	

        iUseNativeClear = aUseNativeClear ? 1 : 0;
        render(x, y, image);
        iUseNativeClear = 0;
        
        System.out.println("P-Out Render M2GScalableGraphics.. erender(int x, int y, ScalableImage image, boolean aUseNativeClear)" );	
        
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#render()
     */
    public synchronized void render(int x, int y, ScalableImage image)
    {
    		System.out.println("P-In Render M2GScalableGraphics.." );	
        if (image == null)
        {
            Logger.ELOG(Logger.EJavaUI, "render() - exception: "
                        + /*SF*/"The target is null"/*SF*/);
            throw new NullPointerException(/*SF*/"The target is null"/*SF*/);
        }
       if (iTargetGC != null)
        {
            final M2GDocument finalDoc = (M2GDocument)((SVGImage)image).getDocument();
            // Get synchronized svg image data
            final int finalSvgW  = image.getViewportWidth();
            final int finalSvgH = image.getViewportHeight();
            if ((finalSvgW == 0) || (finalSvgW == 0))
            {
                return;
            }
            // Calculate clip dimensions
            Rectangle clipRect = iTargetGC.getClipping();
            final int finalClipX = clipRect.x;
            final int finalClipY = clipRect.y;
            final int finalClipW = clipRect.width;
            final int finalClipH = clipRect.height;

            // if none of the svg image is visible due to clipping then don't
            // bother to call render
            if ((x >= (finalClipX + finalClipW)) || (y >= (finalClipY + finalClipH)))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            if (((x + finalSvgW) < finalClipX) || ((y + finalSvgW) < finalClipY))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            

						final int finalX = x;            
						final int finalY = y; 
						         
        		    	 // Execute in UI thread   
	        Platform.executeInUIThread(
	                new M2GRunnableQt() {
	                    public void doRun() {         
																			final int[] finalData = _renderESWT(getHandle(),
											                                     finalDoc.getHandle(), finalX, finalY,
											                                     finalClipX, finalClipY, finalClipW, finalClipH, finalSvgW, finalSvgW,
											                                     finalDoc.getRootElement().getCurrentTime(),
											                                     iUseNativeClear);
																									                                     
																				int[] bitmapHandles;
														            bitmapHandles = new int[4];
														            for (int i = 0; i < 4; i++)
														            {
														                bitmapHandles[i] = finalData[i];
														            }
														            int[] renderData;
														            renderData = new int[6];
														            for (int i = 0; i < 6; i++)
														            {
														                renderData[i] = finalData[i+4];
														            }
														            //iTargetGC.internal_drawM2GImage(bitmapHandles, renderData, iUseNativeClear);

																					}});											                                     
            
        }//if(true)
		else if (iTargetGraphics != null)
		    {
		    	
   	    		System.out.println("P-In Render M2GScalableGraphics.. else case..." );	
            final M2GDocument finalDoc = (M2GDocument)((SVGImage)image).getDocument();
            // Get synchronized svg image data
            final int finalSvgW  = image.getViewportWidth();
            final int finalSvgH = image.getViewportHeight();
            if ((finalSvgW == 0) || (finalSvgH == 0))
            {
                return;
            }
            // Calculate clip dimensions
            final int finalClipX = iTargetGraphics.getClipX() + iTargetGraphics.getTranslateX();
            final int finalClipY = iTargetGraphics.getClipY() + iTargetGraphics.getTranslateY();
            final int finalClipW  = iTargetGraphics.getClipWidth();
            final int finalClipH = iTargetGraphics.getClipHeight();
            // if none of the svg image is visible due to clipping then don't
            // bother to call render
            if ((x >= (finalClipX + finalClipW)) || (y >= (finalClipY + finalClipH)))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            if (((x + finalSvgW) < finalClipX) || ((y + finalSvgH) < finalClipY))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            
				final int finalX = x;            
				final int finalY = y;
				
				System.out.println("P-In Render M2GScalableGraphics.. everything is fine... going in thread.." );	

      		    	 // Execute in UI thread     
        Platform.executeInUIThread(
                new M2GRunnableQt() {
                    public void doRun() {
								            _renderLCDUI(getHandle(), finalDoc.getHandle(), finalX, finalY,
								                         finalClipX, finalClipY, finalClipW, finalClipH, finalSvgW, finalSvgH,
								                         finalDoc.getRootElement().getCurrentTime());
								                       }});
								                       
				System.out.println("P-In Render M2GScalableGraphics.. everything is fine... thread complete...." );	
        }

       else 
        	{
                Logger.ELOG(Logger.EJavaUI, "render() - exception: "
                + /*SF*/"Target is not bound"/*SF*/);//TODO for time being commented.s
                //throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
        	}
        	System.out.println("P-In Render M2GScalableGraphics.. Render Completed.." );	
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#setRenderingQuality(int)
     */
    public synchronized void setRenderingQuality(int mode)
    {
        if ((mode == ScalableGraphics.RENDERING_QUALITY_LOW) ||
                (mode == ScalableGraphics.RENDERING_QUALITY_HIGH))
        {
        	final int finalMode = mode;
        			    	 // Execute in UI thread     
			        Platform.executeInUIThread(
			                new M2GRunnableQt() {
			                    public void doRun() {
											            _setRenderingQuality(getHandle(), finalMode);
											          }
											        });
        }
        else
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRenderingQuality() - exception: " + /*SF*/"The mode is invalid"/*SF*/);
            throw new IllegalArgumentException(/*SF*/"The mode is invalid"/*SF*/);
        }
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#setTransparency(float)
     */
    public synchronized void setTransparency(float alpha)
    {
        if (alpha >= MIN_TRANSPARENCY_ALPHA && alpha <= MAX_TRANSPARENCY_ALPHA)
        {
        	final float finalAlpha = alpha;
        			    	 // Execute in UI thread     
			        Platform.executeInUIThread(
			                new M2GRunnableQt() {
			                    public void doRun() {
            															_setTransparency(getHandle(), finalAlpha);
            														}
            													}
            													);
        }
        else
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setTransparency() - exception: " + /*SF*/"The alpha is out of range"/*SF*/);
            throw new IllegalArgumentException(/*SF*/"The alpha is out of range"/*SF*/);
        }
    }


    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private native int _bind(
        int aRenderContextHandle, int aTargetHandle);
    private native int _createRenderContext(
        int aSVGProxyHandle);
    private native void _deleteRenderContext(
        int aRenderContextHandle);
    private native int _release(
       int aSurfaceHandle ,int aRenderContextHandle);
    private native int _renderLCDUI(
       int aRenderContextHandle, int aDocumentHandle,
        int aX, int aY, int aClipX, int aClipY, int aClipW, int aClipH,
        int aSvgW, int aSvgH, float aCurrentTime);
    private native int[] _renderESWT(
       int aRenderContextHandle, int aDocumentHandle,
        int aX, int aY, int aClipX, int aClipY, int aClipW, int aClipH,
        int aSvgW, int aSvgH, float aCurrentTime, int iUseNativeClear);
    private native void _setRenderingQuality(
       int aRenderContextHandle, int aMode);
    private native void _setTransparency(
       int aRenderContextHandle, float aAlpha);
}
