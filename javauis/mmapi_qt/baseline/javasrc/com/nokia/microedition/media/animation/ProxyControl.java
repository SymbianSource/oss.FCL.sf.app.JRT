/**
 * 
 */
package com.nokia.microedition.media.animation;

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;

/**
 * This class will act like a proxy Control
 * @author d35kumar
 */
class ProxyControl extends Control {

	/**
	 *  this will be actual Shell, for which this class is acting like proxy
	 */
	Control iControl;
	
	public ProxyControl() {
		super(Display.getDefault().getActiveShell(), SWT.BORDER);
	}
	
	/**
	 * Actual Control will be created here   
	 */
	public boolean setParent(Composite parent){
		iControl= new Shell((Shell)parent);
		return true;
	}
	
	/**
	 * 
	 */
	public void addControlListener(ControlListener aListener){
		if(iControl!=null){
			iControl.addControlListener(aListener);
		}
	}
	/**
	 * 
	 */
	public void addFocusListener(FocusListener listener){
		if(iControl!=null)iControl.addFocusListener(listener);
	}
	/**
	 * 
	 */
	public void addKeyListener(KeyListener listener){
		if(iControl!=null)iControl.addKeyListener(listener);
	}
	public void addMouseListener(MouseListener listener){
		if(iControl!=null)iControl.addMouseListener(listener);
	}
	public void addMouseMoveListener(MouseMoveListener listener){
		if(iControl!=null)iControl.addMouseMoveListener(listener);
	}
	/**
	 * 
	 */
	public void addPaintListener(PaintListener aListener){
		if(iControl!=null)iControl.addPaintListener(aListener);
			
	}
	
	public void addTraverseListener(TraverseListener listener){
		if(iControl!=null)iControl.addTraverseListener(listener);
	}
	public Point computeSize(int wHint,
            int hHint){
		Point point=null;
		if(iControl!=null)point=iControl.computeSize(wHint, hHint);
		System.out.println("ProxyControl::computeSize()"+point);
		return point;
	}
	
	public Point computeSize(int wHint,
            int hHint,
            boolean changed){
		Point point=null;
		if(iControl!=null)point=iControl.computeSize(wHint, hHint,changed);
		System.out.println("ProxyControl::computeSize()"+point+"Bolean changed "+changed);
		return point;
	}
	
	public Color getBackground(){
		Color color=null;
		if(iControl!=null)color=iControl.getBackground();
		System.out.println("ProxyControl::getBackground()");
		return color;
	}
	public int getBorderWidth(){
		if(iControl!=null)return iControl.getBorderWidth();
		else return 0;
	}
	public Rectangle getBounds(){
		if(iControl!=null) return iControl.getBounds();
		else return null;
	}
	public boolean getEnabled(){
		if(iControl!=null) return iControl.getEnabled();
		else return false;
	}
	public Font getFont(){
		if(iControl!=null)return iControl.getFont();
		else return null;
	}
	public Color getForeground(){
		if(iControl!=null) return iControl.getForeground();
		else return null;
	}
	
	public java.lang.Object getLayoutData(){
		if(iControl!=null) return iControl.getLayoutData();
		else return null;
	}
	
	public Point getLocation(){
		if(iControl!=null) return iControl.getLocation();
		else return null;
	}
	
	public Menu getMenu(){
		if(iControl!=null) return iControl.getMenu();
		else return null;
	}
	public Composite getParent(){
		if(iControl!=null) return iControl.getParent();
		else return null;
	}
	public Shell getShell(){
		if(iControl!=null) return iControl.getShell();
		else return null;
	}
	public Point getSize(){
		if(iControl!=null) return iControl.getSize();
		else return null;
	}
	
	public java.lang.String getToolTipText(){
		if(iControl!=null) return iControl.getToolTipText();
		else return null;
	}
	public boolean getVisible(){
		if(iControl!=null)  return iControl.getVisible();
		else return false;
	}
	public boolean isEnabled(){
		if(iControl!=null) return iControl.isEnabled();
		else return false;
	}
	
	public boolean isFocusControl(){
		if(iControl!=null) return iControl.isFocusControl();
		else return false;
	}
	public boolean isReparentable(){
		if(iControl!=null) return iControl.isReparentable();
		else return false;
	}
	
	public boolean isVisible(){
		if(iControl!=null)return iControl.isVisible(); 
		else return false;
	}
	public void moveAbove(Control control){
		if(iControl!=null) iControl.moveAbove(control);
	}
	public void moveBelow(Control control){
		if(iControl!=null) iControl.moveBelow(control);
	}
	public void pack(){
		if(iControl!=null) iControl.pack();
	}
	public void pack(boolean changed){
		if(iControl!=null) iControl.pack(changed);
	}
	public void redraw(){
		if(iControl!=null) iControl.redraw();
	}
	public void redraw(int x,
            int y,
            int width,
            int height,
            boolean all){
		if(iControl!=null) iControl.redraw(x,y,width,height, all);
	}
	public void removeControlListener(ControlListener listener){
		if(iControl!=null) iControl.removeControlListener(listener);
	}
	public void removeFocusListener(FocusListener listener){
		if(iControl!=null) iControl.removeFocusListener(listener);
	}
	public void removeMouseListener(MouseListener listener){
		if(iControl!=null) iControl.removeMouseListener(listener);
	}
	public void removeMouseMoveListener(MouseMoveListener listener){
		if(iControl!=null) iControl.removeMouseMoveListener(listener);	
	}
	public void removePaintListener(PaintListener listener){
		if(iControl!=null)iControl.removePaintListener(listener);	
	}
	public void removeTraverseListener(TraverseListener listener){
		if(iControl!=null)iControl.removeTraverseListener(listener);	
	}
	public void setBounds(int x,
            int y,
            int width,
            int height){
		if(iControl!=null)iControl.setBounds(x, y, width, height);
	}
	public void setBounds(Rectangle rect){
		if(iControl!=null)iControl.setBounds(rect);
	}
	public void setCapture(boolean capture){
		if(iControl!=null)iControl.setCapture(capture);	
	}
	public void setEnabled(boolean enabled){
		if(iControl!=null)iControl.setEnabled(enabled);
	}
	public boolean setFocus(){
		if(iControl!=null) return iControl.setFocus();
		else return false;
	}
	public void setFont(Font font){
		if(iControl!=null)iControl.setFont(font);
	}
	public void setForeground(Color color){
		if(iControl!=null)iControl.setForeground(color);	
	}
	public void setLayoutData(java.lang.Object layoutData){
		if(iControl!=null)iControl.setLayoutData(layoutData);	
	}
	public void setLocation(int x,
            int y){
		if(iControl!=null)iControl.setLocation(x, y);	
	}
	
	public void setLocation(Point location){
		if(iControl!=null)iControl.setLocation(location);	
	}
	public void setMenu(Menu menu){
		if(iControl!=null)iControl.setMenu(menu);	
	}
	public void setRedraw(boolean redraw){
		if(iControl!=null)iControl.setRedraw(redraw);
	}
	public void setSize(int width,
            int height){
		if(iControl!=null) iControl.setSize(width, height);	
	}
	
	public void setSize(Point size){
		if(iControl!=null) iControl.setSize(size);
	}
	public void setToolTipText(java.lang.String string){
		if(iControl!=null)iControl.setToolTipText(string);	
	}
	public void setVisible(boolean visible){
		if(iControl!=null)iControl.setVisible(visible);	
	}
	public Point toControl(int x,
            int y){
		if(iControl!=null)return iControl.toControl(x, y);
		else return null;
	}
	public Point toControl(Point point){
		if(iControl!=null) return iControl.toControl(point);
		else return null;
	}
	public Point toDisplay(int x,
            int y){
		if(iControl!=null)return iControl.toDisplay(x,y);
		else return null;
	}
	public Point toDisplay(Point point){
		if(iControl!=null)return iControl.toDisplay(point);
		else return null;
	}
	public boolean traverse(int traversal){
		if(iControl!=null)return iControl.traverse(traversal);
		else return false;
	}
	public void update(){
		if(iControl!=null)iControl.update();
	}
	
	public void checkSubclass() {
		// do nothing,sub-classing allowed
	}
}
