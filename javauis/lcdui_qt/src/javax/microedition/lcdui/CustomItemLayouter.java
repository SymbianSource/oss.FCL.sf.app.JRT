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
package javax.microedition.lcdui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.extension.CanvasExtension;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;

import org.eclipse.swt.widgets.Display;
/**
 * Responsible for layouting CustomItem.
 */
class CustomItemLayouter extends ItemLayouter {

    /**
     * Key name for paint listener.
     */
    private static final String PAINT_LISTENER = "paint";

    /**
     * Key name for mouse listener.
     */
    private static final String MOUSE_LISTENER = "mouse";

    Graphics graphics;

    boolean noBackground;

    private Timer timer = new Timer();
    private CustomItemTimerTask timerTask;

    private static final int DEFAULT_TIMEOUT = 500;
    private static final int DEFAULT_TWIPS = 200;

    private int pointerDownX;
    private int pointerDownY;
    private int twips;
    private int timeout;
    private boolean disableTapDetection;
    private boolean suppressDragEvent;
	  private boolean selectionKeyCompatibility;

    /**
     * Constructor.
     *
     * @param dflp DefaultFormLayoutPolicy used for layouting.
     */
    public CustomItemLayouter(DefaultFormLayoutPolicy dflp) {
        super(dflp);

        noBackground = JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_UI_ENHANCEMENT,
        			JadAttributeUtil.VALUE_CANVAS_HAS_BACKGROUND);

        graphics = new Graphics();

        selectionKeyCompatibility = JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_S60_SELECTION_KEY_COMPATIBILITY,
        			JadAttributeUtil.VALUE_TRUE);

        String tapAttr = JadAttributeUtil.getValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_TAP_DETECTION_OPTIONS);
        if(tapAttr != null) {
          if(tapAttr.indexOf(',') == -1) {
						setDefaultTapValues();
          }
          else {
              String twipString = tapAttr.substring(0, tapAttr.indexOf(',')).trim();
              String timeoutString = tapAttr.substring(tapAttr.indexOf(',') + 1, tapAttr.length()).trim();
              try {
          	     twips = Integer.parseInt(twipString);
          	     timeout = Integer.parseInt(timeoutString);

                 // Check for Negative Values
          	     if( (twips < 0) && (timeout < 0) ) {
          		       setDefaultTapValues();
                 }

                 if( (twips == 0)  && (timeout == 0) ) {
                     disableTapDetection = true;
                 }

                 // if any one of the value is zero, set defaults
                 if( !((twips != 0) && (timeout != 0)) ) {
                     setDefaultTapValues();
                 }
              }
              catch (NumberFormatException e) {
              	 // Alpha Numeric Values of Timeouts and Timeout
					       setDefaultTapValues();
              }
          }
        }
        else
        {
        	 setDefaultTapValues();
        }
    }

    private void setDefaultTapValues() {
				twips = DEFAULT_TWIPS;
				timeout = DEFAULT_TIMEOUT;
    }

    /**
     * Creates the eSWT control.
     *
     * @param parent where to create.
     * @param item on which it is based. Must be CustomItem.
     * @return Control.
     */
    Control eswtGetControl(Composite parent, Item item) {
        Control ret = new CanvasExtension(parent,
                (noBackground ? SWT.NO_BACKGROUND : SWT.NONE));
        return ret;
    }

    /**
     * Set the size of the layouted Control.
     */
    void eswtResizeControl(Item item, Control control, int width, int height) {
        super.eswtResizeControl(item, control, width, height);
        CustomItem customitem = (CustomItem) item;
        customitem.internalHandleSizeChanged(width, height);
    }

    /**
     * Add listeners to Layouter specific control.
     */
    void eswtAddSpecificListeners(Item item, Control control) {
        super.eswtAddSpecificListeners(item, control);
        CanvasExtension canvas = (CanvasExtension) control;
        CIPaintListener pl = new CIPaintListener((CustomItem) item);
        canvas.addPaintListener(pl);
        canvas.setData(PAINT_LISTENER, pl);

        CIMouseListener ml = new CIMouseListener((CustomItem) item);
        canvas.addMouseListener(ml);
        canvas.addMouseMoveListener(ml);
        canvas.setData(MOUSE_LISTENER, ml);
    }

    /**
     * Remove listeners from Layouter specific control.
     */
    void eswtRemoveSpecificListeners(Item item, Control control) {
        super.eswtRemoveSpecificListeners(item, control);
        CanvasExtension canvas = (CanvasExtension) control;
        CIPaintListener pl = (CIPaintListener) canvas.getData(PAINT_LISTENER);
        if (pl != null) {
            canvas.removePaintListener(pl);
            canvas.setData(PAINT_LISTENER, null);
        }
        CIMouseListener ml = (CIMouseListener) canvas.getData(MOUSE_LISTENER);
        if (ml != null) {
            canvas.removeMouseListener(ml);
            canvas.removeMouseMoveListener(ml);
            canvas.setData(MOUSE_LISTENER, null);
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control) {
        return (control instanceof CanvasExtension);
    }

    /**
     * Updates the values of CustomItem.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param) {
        if (reason == CustomItem.UPDATE_REPAINT_RECT) {
            Rectangle rect = (Rectangle) param;
            ((CustomItem)item).updateItem(rect, control);
        }
    }

    /**
     * Gets Canvas direction based on SWT arrows.
     */
    int getCanvasDirection(int key) {
        int ret = 0;
        switch (key) {
            case SWT.ARROW_DOWN:
                ret = Canvas.DOWN;
                break;
            case SWT.ARROW_UP:
                ret = Canvas.UP;
                break;
            case SWT.ARROW_LEFT:
                ret = Canvas.LEFT;
                break;
            case SWT.ARROW_RIGHT:
                ret = Canvas.RIGHT;
                break;
            default:
        }
        return ret;
    }

    /**
     * Gets the specified visRect parameter needed for traverse.
     */
    int[] getVisRect(Control control) {
        final int[] visRect = new int[4];
        if (control != null) {
            Point size = control.getSize();
            visRect[0] = 0;
            visRect[1] = 0;
            visRect[2] = size.x;
            visRect[3] = size.y;
        }
        return visRect;
    }

    /**
     * Returns whether the key event was consumed by CustomItem or not.
     *
     * @param item CustomItem.
     * @param key key code.
     */
    boolean eswtOfferKeyPressed(Item item, int key) {
        CustomItem customItem = (CustomItem) item;

        if( !((selectionKeyCompatibility == true) && (key == -5)) ) {
            EventDispatcher eventDispatcher = EventDispatcher.instance();
            LCDUIEvent e = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_KEYPRESSED, dfi.getForm());
            e.item = customItem;
            e.keyCode = key;
            eventDispatcher.postEvent(e);
         }

        boolean consumed = true;
        int direction = getCanvasDirection(key);
        if (direction > 0) {
            Control control = eswtGetFirstControl(item);
            Control ctrl = eswtFindSpecificControl(item, control);
            int[] visRect = getVisRect(ctrl);
            // Offer event for inner traversal
            consumed = customItem.traverse(direction,
                    dfi.getFormWidth(), dfi.getFormHeight(), visRect);
            if (consumed) {
                // if inner focus is on - scroll to inner focus
                Point loc = new Point(0, 0);
                dfi.getControlPositionOnComposite(ctrl, loc);
                dfi.eswtScrolltoRegion(loc.y + visRect[1],
                                       loc.y + visRect[1] + visRect[3], key);
            }
            control.redraw();
        }
        return consumed;
    }

    /* (non-Javadoc)
     * @see ItemLayouter#eswtOfferKeyReleased(Item, int)
     */
    boolean eswtOfferKeyReleased(Item item, int key) {
        CustomItem customItem = (CustomItem) item;

        if( !((selectionKeyCompatibility == true) && (key == -5)) ) {
            EventDispatcher eventDispatcher = EventDispatcher.instance();
            LCDUIEvent e = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_KEYRELEASED, dfi.getForm());
            e.item = customItem;
            e.keyCode = key;
            eventDispatcher.postEvent(e);
        }

        if (getCanvasDirection(key) > 0) {
            return true;
        }
        return false;
    }

    /* (non-Javadoc)
     * @see ItemLayouter#eswtFocusGained(Item, int)
     */
    void eswtFocusGained(Item item, int swtDir) {
        super.eswtFocusGained(item, swtDir);
        CustomItem customItem = (CustomItem) item;
        Control control = eswtGetFirstControl(item);
        if (control != null) {
            Control ctrl = eswtFindSpecificControl(item, control);
            int[] visRect = getVisRect(ctrl);
            if (customItem.traverse(getCanvasDirection(swtDir),
                    dfi.getFormWidth(), dfi.getFormHeight(), visRect)) {
                // if inner focus is on - scroll to inner focus
                Point loc = new Point(0, 0);
                dfi.getControlPositionOnComposite(ctrl, loc);
                dfi.eswtScrolltoRegion(loc.y + visRect[1],
                                       loc.y + visRect[1] + visRect[3], swtDir);
            }
            control.redraw();
        }
    }

    /* (non-Javadoc)
     * @see ItemLayouter#eswtFocusLost(Item)
     */
    void eswtFocusLost(Item item) {
        super.eswtFocusLost(item);
        CustomItem customItem = (CustomItem) item;
        customItem.traverseOut();
    }

    /* (non-Javadoc)
     * @see temLayouter#eswtHandleShow(Item)
     */
    void eswtHandleShow(Item item) {
        EventDispatcher eventDispatcher = EventDispatcher.instance();
        LCDUIEvent event = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_SHOWNOTIFY, dfi.getForm());
        event.item = item;
        eventDispatcher.postEvent(event);
        CustomItem customItem = (CustomItem) item;
        customItem.repaint();
    }

    /* (non-Javadoc)
     * @see ItemLayouter#eswtHandleHide(Item)
     */
    void eswtHandleHide(Item item) {
        EventDispatcher eventDispatcher = EventDispatcher.instance();
        LCDUIEvent event = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_HIDENOTIFY, dfi.getForm());
        event.item = item;
        eventDispatcher.postEvent(event);
    }

    /**
     * Returns the minimum area needed to display a CustomItem.
     *
     * @param customitem CustomItem object.
     * @return Minimum area needed to display CustomItem.
     */
    static Point calculateMinimumBounds(final CustomItem customitem) {
        Point minSize = new Point(customitem.getMinContentWidth(),
                customitem.getMinContentHeight());
        applyMinMargins(customitem, minSize);
        return minSize;
    }

    /**
     * Returns the preferred area needed to display a CustomItem.
     *
     * @param item CustomItem object.
     * @return preferred area needed to display CustomItem.
     */
    static Point calculatePreferredBounds(final Item item) {
        CustomItem customitem = (CustomItem) item;
        Point prefSize = new Point(
                customitem.getPrefContentWidth(item.getLockedPreferredHeight()),
               customitem.getPrefContentHeight(item.getLockedPreferredWidth()));
        applyPrefMargins(item, prefSize);
        return prefSize;
    }

    /**
     * Paint listener.
     */
    class CIPaintListener implements PaintListener {

        private CustomItem customItem;

        CIPaintListener(CustomItem customItem) {
            this.customItem = customItem;
        }

        public void paintControl(PaintEvent pe) {
        	if(customItem.bufferFlush) {
        		// Paint event initiated by us to paint the Canvas.
				pe.gc.getGCData().internalGc.render(graphics.getCommandBuffer());
        	} else {
				// Native toolkit is requesting an update of an area that has
				// become invalid. Can't do anything here because the contents
				// need to be queried from the MIDlet in another thread by
				// a paint callback. For this a paint callback event is posted.
				// For a moment the native toolkit thinks that the area has
				// been validated when in truth it will be painted later after
				// the paint callback has been executed.
				EventDispatcher eventDispatcher = EventDispatcher.instance();
				LCDUIEvent event = eventDispatcher.newEvent(
						LCDUIEvent.CUSTOMITEM_PAINT_NATIVE_REQUEST, dfi.getForm());
				event.x = pe.x;
				event.y = pe.y;
				event.width = pe.width;
				event.height = pe.height;
				event.widget = pe.widget;
				event.item = customItem;
				eventDispatcher.postEvent(event);
        	}
        }
    }

    /**
     * Mouse listener.
     */
    class CIMouseListener implements MouseListener, MouseMoveListener {

        private CustomItem customItem;

        CIMouseListener(CustomItem customItem) {
            this.customItem = customItem;
        }

        public void mouseDown(MouseEvent event) {
            EventDispatcher eventDispatcher = EventDispatcher.instance();
            LCDUIEvent e = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_POINTERPRESSED, dfi.getForm());
            e.item = customItem;
            e.x = event.x;
            e.y = event.y;
            eventDispatcher.postEvent(e);

           if(!disableTapDetection) {
                // Supress Drag events
           	    suppressDragEvent = true;

                pointerDownX = event.x;
                pointerDownY = event.y;

                // Create and Schedule Timer
                timerTask = new CustomItemTimerTask();
                timer.schedule(timerTask, timeout);
           }
        }

        public void mouseUp(MouseEvent event) {
            int pointerUpX = event.x;
            int pointerUpY = event.y;

            if(!disableTapDetection) {
                timerTask.cancel();
                timerTask = null;

                // If Timer not expired and Mouseup is withing rectangle assign
                // PointercDown to Pinter Up
        	      if(suppressDragEvent && checkWithinRect(event.x, event.y)) {
        	  	      pointerUpX = pointerDownX;
        	  	      pointerUpY = pointerDownY;
       	  	  	    suppressDragEvent = false;
         	      }
           }
            EventDispatcher eventDispatcher = EventDispatcher.instance();
            LCDUIEvent e = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_POINTERRELEASED, dfi.getForm());
            e.item = customItem;
            e.x = pointerUpX;
            e.y = pointerUpY;
            eventDispatcher.postEvent(e);
        }

        public void mouseMove(MouseEvent event) {
        	  // Check for timeout expiry and PointerUp falls outside rectangle
         	  if( disableTapDetection || (!suppressDragEvent) || !checkWithinRect(event.x, event.y)) {
                  EventDispatcher eventDispatcher = EventDispatcher.instance();
                  LCDUIEvent e = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_POINTERDRAGGED, dfi.getForm());
                  e.item = customItem;
                  e.x = event.x;
                  e.y = event.y;
                  eventDispatcher.postEvent(e);
            }
        }

        public void mouseDoubleClick(MouseEvent event) {
        }

        boolean checkWithinRect(int x, int y) {
        	  // Get pixel per inch
        		Point P = Display.getCurrent().getDPI();

            float xPxielwidth  = (twips * P.x) / 1440;
            float yPixelHeight = (twips * P.y) / 1440;

            int RightX = pointerDownX + (int) xPxielwidth;

            // If the rectange width falls outside the custom area
            if(RightX > customItem.getContentWidth()) {
               RightX = customItem.getContentWidth();
            }

            int LeftX = pointerDownX - (int) xPxielwidth;

            // If the rectange width falls outside the custom area
            if(LeftX < 0)
              LeftX = 0;

            int TopY = pointerDownY - (int) yPixelHeight;

            // If the rectange height falls outside the custom area
            if( TopY < 0 )
               TopY = 0;

            int DownY = pointerDownY + (int) yPixelHeight;

            // If the rectange heightfalls outside the custom area.
            if( DownY > customItem.getContentHeight() )
              DownY = customItem.getContentHeight();

            // Find the PointerUp is withing rectange
            if( (x >= LeftX ) && (x <= RightX) ) {
                if( (y >= TopY ) && (y <= DownY) ) {
                	return true;
                }
            }
    	  return false;
        }
    }

    class CustomItemTimerTask extends TimerTask {

        public void run() {
        	   suppressDragEvent = false;
        }
    }

}
