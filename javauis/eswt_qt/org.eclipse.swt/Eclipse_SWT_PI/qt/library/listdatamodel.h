/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/


#ifndef SWT_LISTDATAMODEL_H
#define SWT_LISTDATAMODEL_H

#include <QAbstractListModel>
#include <QItemDelegate>
#include <QTextLayout>
#include <QItemSelection>
#include <QList>
#include "jniutils.h"


class QListView;
 
namespace Java { namespace eSWT {
	
 
class ListItem 
 	{
public:
	ListItem( const QString& aText );
	virtual ~ListItem();
	const QString& text() const;
	virtual const QPixmap* image() const;
	void setText(const QString& string);
	Qt::CheckState checkState() const;
	void setCheckState( Qt::CheckState state );	
	
protected:
	Qt::CheckState state;
	QString txt;
 	};

class ListViewItem : public ListItem
	{
public:
	ListViewItem(const QString& aText, const QPixmap* aImage);
	~ListViewItem();
	const QPixmap* image() const;
	void setImage(const QPixmap* pixmap);
	
private:	
	const QPixmap* img;
	};

class ListBoxItem : public ListItem
	{
public:
	ListBoxItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
		const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount );
	~ListBoxItem();
	const QString& headingText() const;
	const QList<QPixmap>& detailImages() const ;
	const QList<QPixmap>& headingImages() const;
	int detailImageCount() const;
	int headingImageCount() const;
	
private:
	const QString headingTxt;
	QList<QPixmap> detailImgs;
	QList<QPixmap> headingImgs;
	};	

class ListModel : public QAbstractListModel
	{
public:

     ListModel( int type, int style, QListView *parent );
     ~ListModel();
      
     
    /** 
	 * from QAbstractListModel
	 */
	int rowCount(const QModelIndex& parent) const;
   	QVariant data(const QModelIndex& index, int role) const;
	Qt::ItemFlags flags ( const QModelIndex& index ) const;
 
 	/**
 	 * called by JNI, these calls are delegated to protected methods from QAbstractListModel
 	 */
 	void beginInsertRows( const QModelIndex& parent, int first, int last );
	void endInsertRows(); 
	void beginRemoveRows( const QModelIndex& parent, int first, int last );
	void endRemoveRows(); 
	void emitLayoutAboutToBeChanged();
	void emitLayoutChanged();

 	/**
 	 * called by JNI
 	 */
    static ListModel* createDataModel( int type, int style, QListView *parent );
	const QString& itemString( const int row ) const;
	jobjectArray itemStrings( JNIEnv* aJniEnv );
	int indexOf( const QString& string, const int start ) const;
	int insertPosition(const QString& string, const bool ascent = true );
	void setCheckState( const QItemSelection& selection, Qt::CheckState state );
	void remove( const int row );
	void appendItem( const QString& string );
	void appendItem( const QString& string, const QPixmap* pixmap );
	void setItem( const int row, const QString& string );
	void setItem( const int row, const QString& string, const QPixmap* pixmap );
	void insertItem( const int row, const QString& string );
	void insertItem( const int row, const QString& string, const QPixmap* pixmap );
	virtual void appendItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
		const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount );
	virtual int itemHeight();
	virtual void clearList();
	
 	/**
 	 * called by other native classes
 	 */
	int style() const;
	
protected:
 	virtual ListItem* createItem( const QString& string );
 	virtual ListItem* createItem( const QString& string, const QPixmap* pixmap );
	virtual ListItem* createItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
		const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount );

protected:
	const int listType;
	const int layoutStyle;
 	/**
 	 * data body
 	 */
	QList<ListItem*> list;
	};	


class ListViewModel : public ListModel
	{
public:
     ListViewModel( int type, int style, QListView *parent );
     ~ListViewModel();
     
protected: 
 	/**
 	 * from ListModel
 	 */
 	ListItem* createItem( const QString& string );
 	ListItem* createItem( const QString& string, const QPixmap* pixmap );
	};


class ListBoxModel : public ListModel
	{
public:
	ListBoxModel( int type, int style, QListView *parent );
	~ListBoxModel();
	
 	/**
 	 * from ListModel
 	 */
	void clearList();
	void appendItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
		const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount );

 	/**
 	 * Below 5 methods provide item data
  	 */
	const QString& headingText(const int row) const;
	int detailImageCount( const int row ) const;
	int headingImageCount( const int row ) const;	
	const QList<QPixmap>& detailImages(const int row ) const;
	const QList<QPixmap>& headingImages( const int row ) const;
		
 	/**
 	 * called by ListBoxDelegate to save cached layout data to ListModel
 	 * so that the ListBoxDelegate does not have to compute layout data from scratch
 	 * whenever drawing a list item.
 	 */
	void updateLayoutData( const int row, const int rowHeight, const QPoint detailTxtSize, 
		const QPoint headingTxtSize, const int detailImgWidth, const int headingImgWidth ) const;
	
 	/**
 	 * Below 10 methods provide cached layout data 
  	 */
	const int& detailImageWidth(const int index ) const;
	const int& headingImageWidth(const int index ) const;
	QPoint detailTextSize() const;
	QPoint headingTextSize() const;
	int detailImageCount() const;
	int headingImageCount() const;
	QPoint detailImageSize(const int row) const;
	QPoint headingImageSize(const int row) const;
	int rowWithMaxDetailTextSize() const;
	int rowWithMaxHeadingTextSize() const;
	
protected:	
 	/**
 	 * from ListModel
  	 */
	ListItem* createItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
		const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount );
private:
 	/**
 	 * layout data
  	 */
  	 
 	/**
 	 * holds the index which item has the largest detail text size
  	 */
	int detailTxtIndex;
	
 	/**
 	 * holds the index which item has the largest heading text size
  	 */
	int headingTxtIndex;
	
 	/**
 	 * holds the largest  detail text size
  	 */
	mutable QPoint detailTxtSize;
	
 	/**
 	 * holds the largest  heading text size
  	 */
	mutable QPoint headingTxtSize;
	
 	/**
 	 * holds the amount of columns for detail images
  	 */
	int detailImgCount;
	
 	/**
 	 * holds the amount of columns for heading images
  	 */
	int headingImgCount;
	
 	/**
 	 * holds maximum height of images 
  	 */
	int imgHeight;
	
 	/**
 	 * holds widths for every detail image columns 
  	 */
	int* detailImgWidths;
	
 	/**
 	 * holds widths for every heading image columns 
  	 */
	int* headingImgWidths;
	
 	/**
 	 * holds heights for every rows
  	 */
	mutable int* rowHeights;
	
 	/**
 	 * holds entire width for all of detail images.
  	 */
	mutable int detailImgWidth;
	
 	/**
 	 * holds entire width for all of heading images.
  	 */
	mutable int headingImgWidth;
	};

 
 class ListBoxItemDelegate : public QItemDelegate
	{
	Q_OBJECT
public:
	ListBoxItemDelegate( QObject* parent );
	
	// from QItemDelegate
	void paint(QPainter * painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;	
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
    // called from JNI
	void setHeadingFont( QFont* font );

private:

 	/**
 	 * compute sizes of checkbox, detailtext, headingtext, detailImage and  headingImage.
  	 */
	void computeSizes( const QModelIndex& index, const QStyleOptionViewItem& option, QSize* checkSize, 
	QSize *headingImageSize, QSize* detailImageSize, QSize* headingTextSize, QSize* detailTextSize) const;
	
 	/**
 	 * add margins to checkbox, detailtext, headingtext, detailImage and  headingImage.
  	 */
	void addMargins( const QStyleOptionViewItem &option, QSize* checkSize, 
		QSize* headingImageSize, QSize* detailImageSize, QSize* headingTextSize, QSize* detailTextSize ) const;
		
	/**	
	 * compute item entire size	for the item
	 */
	QSize computeSize(  const QStyleOptionViewItem &option, const QSize& check, const QSize& headingImage, 
		const QSize& detailImage, const QSize& headingText, const QSize& detailText ) const;
		
	/**	
	 * layout checkbox, detailtext, headingtext, detailImage and headingImage within a given rectangle
	 * and adjust sizes and positions according to alignments from option	
	 */
	void layoutRects( const QStyleOptionViewItem &option, QRect* check, QRect *headingImage, 
		QRect* detailImage, QRect* headingText, QRect* detailText, bool hint ) const;
	
	/**	
	 * layout checkbox, detailtext, headingtext, detailImage and  headingImage within a given rectangle
	 */
	void doLayout( const QStyleOptionViewItem &option, QRect* checkRect, QRect* headingImageRect,
		QRect* detailImageRect, QRect* headingTextRect, QRect* detailTextRect ) const;
    	QStyleOptionViewItem setOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const;
    
	QRect textRectangle(const int width, const QFont &font, const QString &text) const;
	
	int margin( const QStyleOptionViewItem &option ) const;

private:
    void drawImages( QPainter* painter, const QStyleOptionViewItem &option, 
            const QRect imageRect, const QList<QPixmap>& images, const int* imageWidths, const int imageCount ) const;
	
private:	

	/**	
	 * not own
	 */
	const ListBoxModel* dataModel;
    mutable QTextLayout textLayout;
    QFont* headingFont;
	};
	
 }}
#endif // SWT_LISTDATAMODEL_H
