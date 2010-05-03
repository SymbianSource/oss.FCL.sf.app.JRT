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

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.extension.CanvasExtension;
import org.eclipse.swt.widgets.Control;

/**
 * Implementation of LCDUI abstract <code>CustomItem</code> class.
 */
public abstract class CustomItem extends Item {

    protected static final int NONE = 0;
    protected static final int TRAVERSE_HORIZONTAL = 1;
    protected static final int TRAVERSE_VERTICAL = 2;

    protected static final int KEY_PRESS = 4;
    protected static final int KEY_RELEASE = 8;
    protected static final int KEY_REPEAT = 0x10;

    protected static final int POINTER_PRESS = 0x20;
    protected static final int POINTER_RELEASE = 0x40;
    protected static final int POINTER_DRAG = 0x80;

    /**
     * Constant used by layouter to repaint CustomItem's rectangle section.
     */
    static final int UPDATE_REPAINT_RECT = 4;

    private boolean cleanupNeeded;
    private int contentWidth;
    private int contentHeight;

	private boolean repaintPending;
	private int repaintX1;
	private int repaintY1;
	private int repaintX2;
	private int repaintY2;
	private Object repaintLock;
    private Object cleanupLock;
    private Object resizeLock;

	boolean bufferFlush;
	
	CustomItemLayouter layouter;
	
    /**
     * Constructor.
     *
     * @param label Label of CustomItem.
     */
    protected CustomItem(String label) {
    	repaintLock = new Object();
    	cleanupLock = new Object();
    	resizeLock = new Object();
        setLabel(label);
    }

    /**
     * Get the game action associated with the key code.
     *
     * @param keyCode key code
     * @return game action bound to the key
     */
    public int getGameAction(int keyCode) {
        return KeyTable.getGameAction(keyCode);
    }

    /**
     * Get the interaction modes available on this device.
     *
     * @return bitmask of available interaction modes.
     */
    protected final int getInteractionModes() {
        // TODO: check final interaction modes
        return TRAVERSE_HORIZONTAL | TRAVERSE_VERTICAL
            | KEY_PRESS | KEY_RELEASE
            | POINTER_PRESS | POINTER_DRAG | POINTER_RELEASE;
    }

    /**
     * Invalidates CustomItem.
     */
    protected final void invalidate() {
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     *  Requests repainting of CustomItem.
     */
    protected final void repaint() {
        repaint(0, 0, contentWidth, contentHeight);
    }

    /**
     * Requests repainting of the specified area in CustomItem.
     *
     * @param aX
     * @param aY
     * @param aWidth
     * @param aHeight
     */
    protected final void repaint(int aX, int aY, int aWidth, int aHeight) {
        Rectangle rect = new Rectangle(aX, aY, aWidth, aHeight);
        // From here it goes to updateItem()
        updateParent(UPDATE_REPAINT_RECT, rect);
    }

    /**
     * Callback method for traverse-in event.
     *
     * @param direction
     * @param viewportWidth
     * @param viewportHeight
     * @param visRect if the CustomItem supports internal traversal and keeps
     *            focus, visRect represents the visible rectangle (focused area)
     * @return true if internal traversal has occurred, false otherwise
     */
    protected boolean traverse(int direction, int viewportWidth,
            int viewportHeight, int[] visRect) {
        return false;
    }

    /**
     * Callback method for traverse-out event.
     */
    protected void traverseOut() {
    }

    /**
     * Callback method for key pressed event.
     */
    protected void keyPressed(int aKeyCode) {
    }

    /**
     * Callback method for key released event.
     */
    protected void keyReleased(int aKeyCode) {
    }

    /**
     * Callback method for key repeated event.
     */
    protected void keyRepeated(int aKeyCode) {
    }

    /**
     * Callback method for pointer pressed event.
     */
    protected void pointerPressed(int aX, int aY) {
    }

    /**
     * Callback method for pointer released event.
     */
    protected void pointerReleased(int aX, int aY) {
    }

    /**
     * Callback method for pointer dragged event.
     */
    protected void pointerDragged(int aX, int aY) {
    }

    /**
     * Callback method when item gets shown.
     */
    protected void showNotify() {
    }

    /**
     * Callback method when item gets hidden.
     */
    protected void hideNotify() {
    }

    /**
     * Callback method for content area size change event.
     */
    protected void sizeChanged(int aWidth, int aHeight) {
    }

    /**
     * Abstract method.
     *
     * @param graphics
     * @param aWidth
     * @param aHeight
     */
    protected abstract void paint(Graphics graphics, int aWidth, int aHeight);

    /**
     * Should return the minimum width of the content area.
     */
    protected abstract int getMinContentWidth();

    /**
     * Should return the minimum height of the content area.
     */
    protected abstract int getMinContentHeight();

    /**
     * Should return the preferred width of the content area.
     */
    protected abstract int getPrefContentWidth(int aHeight);

    /**
     * Should return the preferred height of the content area.
     */
    protected abstract int getPrefContentHeight(int aWidth);

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize() {
        return CustomItemLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize() {
        return CustomItemLayouter.calculatePreferredBounds(this);
    }

    /**
     * Set the size of the content area.
     *
     * @param newWidth
     * @param newHeight
     */
    void internalHandleSizeChanged(int newWidth, int newHeight) {
        if (contentWidth != newWidth || contentHeight != newHeight) {
        	synchronized(resizeLock) {
	        	contentWidth = newWidth;
	            contentHeight = newHeight;
        	}
        	EventDispatcher eventDispatcher = EventDispatcher.instance();
        	LCDUIEvent event = eventDispatcher.newEvent(LCDUIEvent.CUSTOMITEM_SIZECHANGED, layouter.dfi.getForm());
        	event.item = this;
        	eventDispatcher.postEvent(event);
            synchronized(cleanupLock) {
            	cleanupNeeded = true;
            }
            repaint();
        }
    }

	/*
	 * Note that if you call this and getContentHeight() from a non-UI thread
	 * then it must be made sure size doesn't change between the calls.
	 */
    int getContentWidth() {
        return contentWidth;
    }

	/*
	 * Note that if you call this and getContentHeight() from a non-UI thread
	 * then it must be made sure size doesn't change between the calls.
	 */
    int getContentHeight() {
        return contentHeight;
    }

    boolean isFocusable() {
        return true;
    }

    /*
     * This gets called when Form contents are modified. 
     */
    void setParent(Screen parent) {
        super.setParent(parent);
        if(parent == null) {
        	// Item was removed from a Form
        	layouter = null;
        } else {
        	// Item was added to a Form
        	layouter = ((CustomItemLayouter)((Form)parent).getLayoutPolicy().getLayouter(this));
        }
    }
    
	private boolean invalidate(int x, int y, int width, int height) {
		// Regularize bounds
		final int x1 = x;
		final int y1 = y;
		final int x2 = x + width;
		final int y2 = y + height;

		// Union the current and new damaged rects
		final boolean valid = ((repaintX2 - repaintX1) <= 0) && ((repaintY2 - repaintY1) <= 0);
		if (!valid) {
			repaintX1 = Math.min(repaintX1, x1);
			repaintY1 = Math.min(repaintY1, y1);
			repaintX2 = Math.max(repaintX2, x2);
			repaintY2 = Math.max(repaintY2, y2);
		} else {
			repaintX1 = x1;
			repaintY1 = y1;
			repaintX2 = x2;
			repaintY2 = y2;
		}

		// UI thread can change the size at any time. Must make sure it doesn't
		// change between reading the width and the height. 
		final int w;
		final int h;
		synchronized(resizeLock) {
			w = contentWidth;
			h = contentHeight;
		}

		// Clip to bounds
		repaintX1 = repaintX1 > 0 ? repaintX1 : 0;
		repaintY1 = repaintY1 > 0 ? repaintY1 : 0;
		repaintX2 = repaintX2 < w ? repaintX2 : w;
		repaintY2 = repaintY2 < h ? repaintY2 : h;

		return valid;
	}
	
	/*
	 * Note the control passed as a parameter can change. It must not be stored
	 * to the CustomItem. Adding and removing Form item is blocked for the
	 * duration of the call. 
	 */
	void updateItem(Rectangle rect, Control control) {
		// Paint callback event is posted without any invalid area info.
		// Invalid area info is kept in the member variables. Only one event 
    	// per Canvas is added to the queue. If there are more repaint() calls 
    	// before the already posted event has been served those are merged
		// to the invalid area in the member variables without posting a new 
    	// event. 
    	synchronized(repaintLock) {
			if (invalidate(rect.x, rect.y, rect.width, rect.height)) {
				// Note that repaintPending doesn't mean that there's a repaint
				// event in the queue. It means that the invalid area is going
				// to be repainted and there's no need to add a new event. 
				// It's possible that the repaint event has already been 
				// removed from the queue but repaintPending is still true. In 
				// that case it's currently being processed and we can still 
				// add to the invalid area. 
				if (!repaintPending) {
					EventDispatcher eventDispatcher = EventDispatcher.instance();			
					LCDUIEvent event = eventDispatcher.newEvent(
							LCDUIEvent.CUSTOMITEM_PAINT_MIDLET_REQUEST, layouter.dfi.getForm());
					event.widget = control;
					event.item = this;
					eventDispatcher.postEvent(event);
					repaintPending = true;
				}
			}
    	}
	}
    
    /*
     * Dispatcher thread thread calls. Operations changing Form content are
     * blocked for the duration of the method call. It has been checked that
     * the eSWT widget has not been disposed and that the CustomItem has not
     * been removed from the Form. 
     */
	void doCallback(final LCDUIEvent event) {
		switch(event.type) {
		case LCDUIEvent.CUSTOMITEM_PAINT_MIDLET_REQUEST:
		case LCDUIEvent.CUSTOMITEM_PAINT_NATIVE_REQUEST:
			doPaintCallback(event);
			break;
		case LCDUIEvent.CUSTOMITEM_HIDENOTIFY:
			hideNotify();
			break;
		case LCDUIEvent.CUSTOMITEM_SHOWNOTIFY:
			showNotify();
			break;
		case LCDUIEvent.CUSTOMITEM_KEYPRESSED:
			keyPressed(event.keyCode);
			break;
		case LCDUIEvent.CUSTOMITEM_KEYREPEATED:
			keyRepeated(event.keyCode);
			break;
		case LCDUIEvent.CUSTOMITEM_KEYRELEASED:
			keyReleased(event.keyCode);
			break;
		case LCDUIEvent.CUSTOMITEM_POINTERDRAGGED:
			pointerDragged(event.x, event.y);
			break;
		case LCDUIEvent.CUSTOMITEM_POINTERPRESSED:
			pointerPressed(event.x, event.y);
			break;
		case LCDUIEvent.CUSTOMITEM_POINTERRELEASED:
			pointerReleased(event.x, event.y);
			break;
		case LCDUIEvent.CUSTOMITEM_SIZECHANGED:
			sizeChanged(event.width, event.height);
			break;
		default:
			super.doCallback(event);
		}
	}
	
	/*
	 * Dispatcher thread calls. 
	 */
	void doPaintCallback(final LCDUIEvent event) {
		// Decide the area going to be painted by the callback. 
		final int redrawNowX;
		final int redrawNowY;
		final int redrawNowW;
		final int redrawNowH;
		// Before this thread obtains the repaintLock any repaint() calls
		// will still be adding to the invalid area that is going to be
		// painted by this callback. 
		synchronized(repaintLock) {
			if(event.type == LCDUIEvent.CUSTOMITEM_PAINT_NATIVE_REQUEST) {
				// Merge with possibly existing repaint() requests
				invalidate(event.x, event.y, event.width, event.height);
			} else {
				// Need to add a new event to the queue in subsequent repaint()
				// calls. 
				repaintPending = false;
			}
			
			// Store the current area to be painted
			redrawNowX = repaintX1;
			redrawNowY = repaintY1;
			redrawNowW = repaintX2-repaintX1;
			redrawNowH = repaintY2-repaintY1;
			
			// After releasing the lock the repaint() calls will start with
			// new invalid area. 
			repaintX1 = repaintX2 = repaintY1 = repaintY2 = 0;
			
			// Don't do the callback if there's nothing to paint
			if(!((redrawNowW > 0) && (redrawNowH > 0))) {
				return;
			}
		}
		
		// Prepare the GC's buffer if not done yet
		if (layouter.graphics.getCommandBuffer() == null) {
			layouter.graphics.initBuffered(this, event.x, event.y, event.width, event.height);
		}

		// Clean the background if dirty, buffer the operations. 
		synchronized(cleanupLock) {
			if (cleanupNeeded && layouter.noBackground) {
				// Must be made sure that size doesn't change between reading
				// the width and the height. 
				int contentWidth, contentHeight;
				synchronized(resizeLock) {
					contentWidth = this.contentWidth;
					contentHeight = this.contentHeight;
				}
				
				layouter.graphics.setClip(0, 0, contentWidth, contentHeight);
				layouter.graphics.cleanBackground(new Rectangle(0, 0, contentWidth, contentHeight));
				cleanupNeeded = false;
			}
		}
		
		// Clip must define the invalid area
		layouter.graphics.setClip(redrawNowX, redrawNowY, redrawNowW, redrawNowH);
		
		// The callback
		paint(layouter.graphics, contentWidth, contentHeight);

		// Wait until the UI thread is available. Then in the UI thread 
		// synchronously send a paint event. 
		ESWTUIThreadRunner.safeSyncExec(new Runnable() {
			public void run() {
				if(event.widget.isDisposed()) {
					return;
				}
				bufferFlush = true;
				((CanvasExtension) event.widget)
						.redrawNow(redrawNowX, redrawNowY, redrawNowW, redrawNowH);
		    	layouter.graphics.resetCommandBuffer();
				bufferFlush = false;
			}
		});
	}
}
