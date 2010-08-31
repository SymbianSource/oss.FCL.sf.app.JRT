package com.nokia.mj.impl.uitestutils;

/**
 * Mask implementation for storing and manipulating reference mask 
 * for pixel checking. Mask can be mirrored, rotated, restored etc.
 * 
 * A new mask can be created from int[] array which holds indexes for 
 * refrence colors, e.g. value 1 can stand for red and 0 for white.
 * 
 * example:
 * <code>
 * int[] maskData  = {
 * 		0,0,0,0,0,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,0,0,0,0,0
 * };
 * </code>
 * 
 * ImageUtils can validate image pixels against mask indexes and given index colors.
 * 
 * @author sampkaar
 */


public class Mask {
	
	int[] iMask = null;
	int[] iOrigMask = null;
	int iScanlength = 0;
	int iWidth = 0;
	int iHeight = 0;
	Point iCenter;
	
	/**
	 * Creates instance of Mask.
	 * 
	 * @param mask - int array for mask to be created, values should be 0 or 1
	 * @param scanlength - length of line in mask
	 * 
	 * @throws IllegalArgumentException - if mask is null
	 * @throws IllegalArgumentException - if remainder or mask.length/scanlength is not zero
	 */
	public Mask(int[] mask, int scanlength) {
		if(mask == null) {
			throw new IllegalArgumentException("mask is null");
		}
		if((mask.length % scanlength) != 0) {
			throw new IllegalArgumentException("Illegal scanlength");
		}
		iMask = mask;
		iOrigMask = mask;
		iScanlength = scanlength;
		iCenter = new Point(scanlength/2, (iMask.length/scanlength)/2);
		iWidth = scanlength;
		iHeight = iMask.length/scanlength;
	}
	
	/**
	 * Returns mask's data array. 
	 */
	public int[] getData() {
			return iMask;
	}
	
	/**
	 * gets subarea (rect) of mask specified by given arguments.
	 * 
	 * @param x The x-coordinate of ractangle
	 * @param y The y-coordinate of ractangle
	 * @param width The width of ractangle
	 * @param height The height of ractangle
	 */
	public int[] getData(int x, int y, int width, int height) {
		// specified area cannot exceed mask bounds
		if((x+width > iWidth) || (y+height > iHeight)) {
			throw new IllegalArgumentException("Area to be retrieved exceeds mask bounds");
		}
		
		// create new array 
		int[] result = new int[width*height];
		final int h = y+height;
		final int w = x+width;
		
		// populate array
		int resultIndex = 0;
		for(int i = y; i < h ; i++) {
			for(int j=x; j<w ; j++) {
				result[resultIndex] = iMask[(i*iWidth)+j];
				resultIndex++;
			}
		}
		return result;
	}
	
	
	/**
	 * gets the width of mask.
	 */
	public int getWidth() {
		return iWidth;
	}
	
	/**
	 * gets the height of mask.
	 */
	public int getHeight() {
		return iHeight;
	}
	
	/**
	 * Mirrors mask around its center point.
	 * 
	 * @param vertical if true mirror is done along y-axis otherwise along x-axis
	 */
	public void mirror(boolean vertical) {
		int[] resultMask = new int[iMask.length];
		int y = 0; // linecounter
		
		// along y-axis
		if(vertical) {
			// start at end for first scanline
			int sourceIndex = iScanlength-1; 
			for(int i=0; i<iMask.length; i++) {
				resultMask[i] = iMask[sourceIndex];
				sourceIndex--;
				
				// move to next line
				if(i==((y*iScanlength) + (iScanlength-1))) {
					y++;
					sourceIndex = (y*iScanlength)+(iScanlength-1); 
				} 
			}
		}
		// along x-axis
		else {
			int dstIndex = 0;
			for(int srcIndex = (iMask.length-iScanlength); srcIndex >= 0; srcIndex -= iScanlength) {
				System.arraycopy(iMask, srcIndex, resultMask, dstIndex, iScanlength);
				dstIndex += iScanlength;
			}
		}
		// save result mask
		iMask = resultMask;	
	}
	
	/**
	 * Prints mask contents to console
	 */
	public void print() {
		String output = "\n Mask("+iScanlength+","+iMask.length/iScanlength+"):\n";
		int y = 0;
		for(int i = 0; i < iMask.length; i++) {
			output += iMask[i]+",";
			if(i==((y*iScanlength) + (iScanlength-1))) {
				output += "\n";
				y++;
			}
		}
		System.out.println(output);
	}
	
	/**
	 * prints given mask data
	 * 
	 * @param maskData The data of mask to be printed
	 * @param scanlengt The scanlength of one line in maskData
	 */
	public void print(int[] maskData, int scanlength) {
		
		final int width = scanlength;
		final int height = maskData.length / width;
		
		String output = "\n Mask("+width+","+height+"):\n";
		
		int y = 0;
		for(int i = 0; i < maskData.length; i++) {
			output += maskData[i]+",";
			if(i==((y*width) + (width-1))) {
				output += "\n";
				y++;
			}
		}
		System.out.println(output);
	}
	
	/**
	 * Resets mask to original mask given in constructor.
	 */
	public void resetOriginal() {
		iMask = iOrigMask;
	}
	
	/**
	 * Rotates mask.
	 * Positive angle rotates clockwise 
	 * Negative angle rotates counterclockwise
	 * 
	 * NOTE output is 100% correct only if width/height of mask is not even, i.e.
	 * 21 is ok, but 20 not. This is because currently the rotation is done after 
	 * translating origin to the center of the mask and if width is 20 there no clear 
	 * center...
	 * 
	 * @param angel - The rotation angel
	 * @throws IllegalArgumentException - if remainder of angle / 90 is not zero
	 */
	public void rotate(float angle) {
		if((angle % 90) != 0) {
			throw new IllegalArgumentException("do not use other than 90 degree rotations");
		}
		Matrix m = new Matrix();
		m.translate(iCenter.x, iCenter.y);
		m.rotate(angle);
		m.translate(-iCenter.x, -iCenter.y);
		transform(m);
	}
	
	/**
	 * Transforms mask based on given transform matrix.
	 * Note e.g. 45 degree rotation will fail as it needs bigger
	 * buffer, which is not supported.
	 * 
	 * @param m - The trasformation matrix
	 * @throws IllegalArgumentException - if matrix m is null
	 * 
	 */
	private void transform(Matrix m) {
		if(m == null) {
			throw new IllegalArgumentException("Matrix m is null");
		}
		
		int[] resultMask = new int[iMask.length];
		int y = 0; // line in source mask
		int targetIndex = 0; // result mask index
		
		// points for source and target
		Point sourcePoint;
		Point targetPoint;
		
		for(int i=0; i< iMask.length ; i++){
			sourcePoint = new Point((i-(y*iScanlength)), y);
			targetPoint = m.transform(sourcePoint);
			targetIndex = (round(targetPoint.y)*iScanlength)+round(targetPoint.x);
			resultMask[targetIndex] = iMask[i];
			
			if(i==((y*iScanlength) + (iScanlength-1))) {
				y++;
			}                   
		}
		iMask = resultMask;
	}
	
	/**
	 * Rounds given float to closest int value, e.g.
	 * If value >= 0.5 result is 1 
	 * if value <= 0.49 result is 0
	 * 
	 * @param value - The value to be rounded
	 */
	private int round(float value) {
		int result = 0;
		float temp = (int)value;  
		
		if((temp+0.5 <= value))  {
			result = (int)value+1;
		} else {
			result = (int)value;
		}
		return result;
	}
}
