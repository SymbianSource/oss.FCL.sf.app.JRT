package org.eclipse.swt.internal.qt.graphics;

/**
 * Buffer provides a storage for GraphicsContext commands. If GraphicsContext is bound to
 * Buffer all subsequental draw commands are recorded within the buffer and can be replayed 
 * later by processing it with GraphicsContext.render()-method.
 * 
 * The main benefit of buffer is the possibility to render to it outside paint callbacks, 
 * i.e. when the actual rendering target cannot be bound. Usage of buffer may also improve 
 * performance.
 * 
 */
public final class NativeCommandBuffer {

	/**
	 * Native peer handle
	 */
	int handle = 0;
	
	/**
	 * State of native peer
	 */
	boolean disposed;
	
	/**
	 * Constructor
	 * @throws OutOfMemoryError If native buffer creation fails
	 */
	public NativeCommandBuffer() {
		Utils.validateUiThread();
		handle = OS.buffer_init();
		if (handle == 0) {
			throw new OutOfMemoryError("Native buffer creation failed");
		}
	}
	
	/**
	 * Disposes this buffer and cleans all related native resources
	 */
	public void dispose() {
		if (!disposed) {
			OS.buffer_dispose(handle);
			handle = 0;
			disposed = true;
		}
	}

    /**
     * Gets the rectangle where draw operations have affected,
     * i.e. the invalid rectangle.
     *
     * Values are stored in rect array as follows: 
     * <ul>
     * <li>rect[0] - the x coordinate of rect</li>
     * <li>rect[1] - the y coordinate of rect
     * <li>rect[2] - the width of rect
     * <li>rect[3] - the height of rect
     * </ul>
     * 
     * @param rect Integer array where the rect data is populated, size must be atleast 4
     * @throws NullPointerException if rect is null
     * @throws IllegalArgumentException if rect.length < 4
     * @throws IllegalStateException if the buffer has already been disposed
	 */
	public void getInvalidRect(int[] rect) {
		checkState();
		if(rect == null) {
			throw new NullPointerException("rect is null");
		}
		if(rect.length < 4) {
			throw new IllegalArgumentException("rect array size is < 4");
		}
		OS.buffer_getInvalidRect(handle, rect);
	}
	
    /**
     * Gets the state of this Buffer, i.e. is it disposed or not. 
     * 
     * @return true if this instanace has been disposed, otherwise false
     */
	public boolean isDisposed() {
		return disposed;
	}
	
    /**
     * private method for checking the state of this instance.
     * @throws IllegalStateException If this instance is already disposed 
     */
    private void checkState() {
    	Utils.validateUiThread();
        if (disposed) {
            throw new IllegalStateException("Buffer already disposed");
        }
    }
}
