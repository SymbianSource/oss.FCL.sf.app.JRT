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

#include <QListView>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>
#include <QString>
#include <QApplication>
#include <QPainter>
#include <QStringMatcher>
 
#include "swtlog.h"
#include "listdatamodel.h"
#include "jniutils.h"
#include "swtapplication.h"

/**
 * list type
 */
const static int LISTTYPE_SORTEDLIST = 0x2;
const static int LISTTYPE_LISTVIEW = 0x4;
const static int LISTTYPE_LISTBOX = 0x8;

/**
 * layout styles
 */
const static int LB_STYLE_1LINE_ITEM = 0x1;
const static int LB_STYLE_2LINE_ITEM = 0x2;
const static int LB_MOD_SHOW_HEADING_ICONS = 0x20;
const static int LB_MOD_SHOW_DETAIL_ICONS = 0x40;
const static int LISTBASE_SHOW_CHECKBOX = 0x80;

using namespace Java::eSWT;

/**
 *  ListItem
 */
ListItem::ListItem(const QString& aText)
	: txt( aText )
	{
	SWT_LOG_FUNC_CALL();
	}

ListItem::~ListItem()
	{
	SWT_LOG_FUNC_CALL();
	} 


const QString& ListItem::text() const
	{
	return txt;
	}

const QPixmap* ListItem::image() const
	{
	return NULL;
	}

void ListItem::setText( const QString& string )
	{
	txt = string;
	}


Qt::CheckState ListItem::checkState() const
	{
	SWT_LOG_FUNC_CALL();
	return state;
	}


void ListItem::setCheckState( Qt::CheckState state )
	{
	SWT_LOG_FUNC_CALL();
	this->state = state;
	}
	
	
/** 
 * ListViewItem
 */

ListViewItem::ListViewItem(const QString& aText, const QPixmap* aImage) 
 : ListItem(aText), img(aImage)
	{
	SWT_LOG_FUNC_CALL();
	}

ListViewItem::~ListViewItem()
	{
	SWT_LOG_FUNC_CALL();
	}


const QPixmap* ListViewItem::image() const
	{
	return img;
	}
	
void ListViewItem::setImage(const QPixmap* pixmap)
	{
	img = pixmap;
	}


/**
 *  ListBoxItem
 */

ListBoxItem::ListBoxItem( const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
	const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount ) 
	: ListItem(aDetailText), headingTxt(aHeadingText)
	{
	SWT_LOG_FUNC_CALL();
	for (int i = 0; i < aDetailImageCount; i++) 
	    {
	    detailImgs.push_back(*aDetailImages[i]);
	    }
    for (int i = 0; i < aHeadingImageCount; i++) 
        {
        headingImgs.push_back(*aHeadingImages[i]);
        }
	}


ListBoxItem::~ListBoxItem()
	{
	SWT_LOG_FUNC_CALL();
	}
	


const QString& ListBoxItem::headingText() const
	{
	SWT_LOG_FUNC_CALL();
	return headingTxt;
	}
	
const QList<QPixmap>& ListBoxItem::detailImages() const
	{
	SWT_LOG_FUNC_CALL();
	return detailImgs;
	}
	
const QList<QPixmap>& ListBoxItem::headingImages() const
	{
	SWT_LOG_FUNC_CALL();
	return headingImgs;
	}
	
int ListBoxItem::detailImageCount() const
	{
	SWT_LOG_FUNC_CALL();
	return detailImgs.count();
	}
	
int ListBoxItem::headingImageCount() const
	{
	SWT_LOG_FUNC_CALL();
	return headingImgs.count();
	}
	
	
/**
 *  ListModel
 */

ListModel::ListModel( int type, int style, QListView *parent )
	: QAbstractListModel( parent ),
	listType( type ),
	layoutStyle( style )
	{
	SWT_LOG_FUNC_CALL();
	SWT_LOG_DATA_3("listType=%d layoutStyle=%x parent=%x ", style, type, reinterpret_cast<int>(parent) );
	}
	
ListModel::~ListModel()
	{
	SWT_LOG_FUNC_CALL();
	clearList();
	}

ListModel* ListModel::createDataModel( int type, int style, QListView *parent )
	{
	if( ( type & LISTTYPE_LISTBOX ) != 0 )
		{
		return new ListBoxModel( type, style, parent );
		}
	else if ( ( type & LISTTYPE_LISTVIEW ) != 0 )
		{
		return new ListViewModel( type, style, parent );
		}
	else
		{
		return new ListModel( type, style, parent );
		}	
	}


void ListModel::beginInsertRows( const QModelIndex& parent, int first, int last )
	{
	Q_ASSERT( first >= 0 && first <= list.size() );
	Q_ASSERT( last >= first );
	SWT_LOG_FUNC_CALL();
	QAbstractItemModel::beginInsertRows( parent, first, last );		
	}
	
	
void ListModel::endInsertRows()
	{
	SWT_LOG_FUNC_CALL();
	QAbstractItemModel::endInsertRows();
	}

void ListModel::beginRemoveRows( const QModelIndex& parent, int first, int last )
	{
	Q_ASSERT( first > -1 && first < list.size() );
	Q_ASSERT( last >= first && last < list.size() );
	SWT_LOG_FUNC_CALL();
	QAbstractItemModel::beginRemoveRows( parent, first, last );		
	}
	
void ListModel::endRemoveRows()
	{
	SWT_LOG_FUNC_CALL();
	QAbstractItemModel::endRemoveRows();
	}
	

void ListModel::emitLayoutAboutToBeChanged()
	{
	SWT_LOG_FUNC_CALL();
	emit layoutAboutToBeChanged();
	}

void ListModel::emitLayoutChanged()
	{
	SWT_LOG_FUNC_CALL();
	emit layoutChanged();
	}
	
Qt::ItemFlags ListModel::flags( const QModelIndex& index ) const
	{
	SWT_LOG_FUNC_CALL();
	Qt::ItemFlags  flags = ( QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable ) & ~Qt::ItemIsUserCheckable;
	if( ( layoutStyle & LISTBASE_SHOW_CHECKBOX ) != 0 )
		{
		flags |= Qt::ItemIsUserCheckable;
		}
	return flags;
	}
	
int ListModel::rowCount(const QModelIndex& /*parent*/ ) const
	{
	SWT_LOG_FUNC_CALL();
	return list.count();
	}
	
QVariant ListModel::data(const QModelIndex &index, int role) const
	{
	SWT_LOG_FUNC_CALL();
	if(!index.isValid()) 
		{
		SWT_LOG_DATA_1("invalid index %d", -1);
		return QVariant();
		}
	Q_ASSERT( index.row() > -1  && index.row() < list.size() );
	
	if ( role == Qt::DisplayRole )
		{ 
		SWT_LOG_DATA_2("Request data on DisplayRole: row=%d col=%d", index.row(), index.column());
		return	list[index.row()]->text();
		}
	else if ( role == Qt::DecorationRole )
		{ 
		SWT_LOG_DATA_2("Request data on DecorationRole: row=%d col=%d", index.row(), index.column());
		const QPixmap* img = list[index.row()]->image();
		if( img )
			{
			return *img;
			}
		}
	else if( role == Qt::CheckStateRole && index.column()==0 ) 
		{
		SWT_LOG_DATA_2("Request checkState: row=%d col=%d", index.row(), index.column());
		return list[index.row()]->checkState();
		}
		
	SWT_LOG_DATA_2("Request something else: row=%d col=%d", index.row(), index.column());
	return QVariant();
	}

void ListModel::setCheckState( const QItemSelection& selection, Qt::CheckState state )
	{
	SWT_LOG_FUNC_CALL();
	const QModelIndexList indexes = selection.indexes ();
	for( int i=0; i< indexes.size(); i++)
		{
		list.at(indexes.at(i).row())->setCheckState( state );
		}
	}

const QString& ListModel::itemString( const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return list.at(row)->text();	
	}


jobjectArray ListModel::itemStrings( JNIEnv* aJniEnv )
	{
	SWT_LOG_FUNC_CALL();
	
	const int count = list.size();
   	jclass stringClass = aJniEnv->FindClass( "java/lang/String" );
    jobjectArray result = aJniEnv->NewObjectArray(count, stringClass, NULL);
    if( !result )
        {
        throw std::bad_alloc();
        }
    
 	jstring javaString;
    for ( int i=0; i<count; ++i ) 
	    {
		javaString = swtApp->jniUtils().QStringToJavaString(aJniEnv, list.at(i)->text() );
	    aJniEnv->SetObjectArrayElement( result, i, javaString ); 
	    aJniEnv->DeleteLocalRef( javaString );
	    }
	aJniEnv->DeleteLocalRef( stringClass );
        
    return result;
	}

int ListModel::indexOf(const QString& string, const int start ) const
	{
	SWT_LOG_FUNC_CALL();
 	Q_ASSERT(start > -1 && start < list.size());
    QStringMatcher matcher(string, Qt::CaseSensitive);
    for (int i = start; i < list.size(); ++i) 
    	{
        const QString & str = list.at(i)->text();
        if (string.length() == str.length() && matcher.indexIn(str) == 0)
            return i;
    	}
    return -1;
	}

int ListModel::itemHeight()
	{
	SWT_LOG_FUNC_CALL();
	const QListView* listView = static_cast<const QListView*>( static_cast<QObject*>(this)->parent() );

	if(list.size() > 0 ) 
		{
		return listView->sizeHintForRow(0);
		}
		
	return listView->fontMetrics().height();
	}

int ListModel::insertPosition(const QString& string, bool ascent )
	{
	SWT_LOG_FUNC_CALL();
	if( listType != LISTTYPE_SORTEDLIST )
		{
		throw std::bad_exception();
		}
		
	int pos = 0;	
	for(int i = list.size() -1; i > -1; i--)
		{
		if( ascent )
			{
			if( string < list.at(i)->text() )
			continue;
			}
		else
			{
			if( string > list.at(i)->text() )
			continue;
			}	
		pos = i + 1;
		break;
		}
	return pos;
	}


void ListModel::remove( const int row )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	ListItem* item = list.takeAt(row);
	delete item;
	}


void ListModel::clearList()
	{
	SWT_LOG_FUNC_CALL();
	qDeleteAll(list.begin(), list.end());
	list.clear();
	}


void ListModel::appendItem( const QString& string )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item = createItem( string );
	list.append(item);
	}
	
void ListModel::appendItem(const QString& string, const QPixmap* pixmap )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item = createItem( string, pixmap );
	list.append(item);
	}

	
void ListModel::appendItem(const QString& /*aDetailText*/, const QPixmap** /*aDetailImages*/, const int /*aDetailImageCount*/,  
	const QString& /*aHeadingText*/, const QPixmap** /*aHeadingImages*/, const int /*aHeadingImageCount*/ )
	{
	Q_ASSERT( false );	
	}

void ListModel::setItem( const int row, const QString& string )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	list.at(row)->setText(string);
	}

	
void ListModel::setItem( const int row, const QString& string, const QPixmap* pixmap )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	list.at(row)->setText(string);
	static_cast<ListViewItem*>(list.at(row))->setImage(pixmap);
	}

	

void ListModel::insertItem( const int row, const QString& string )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row <= list.size());
	ListItem* item = createItem( string );
	if( row == list.size() )
		{
		list.append(item);
		}
	else
		{
		list.insert( row, item );
		}
	} 

void ListModel::insertItem( const int row, const QString& string, const QPixmap* pixmap )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row <= list.size());
	ListItem* item = createItem( string, pixmap );
	if( row == list.size() )
		{
		list.append(item);
		}
	else
		{
		list.insert( row, item );
		}
	} 


int ListModel::style() const
	{
	SWT_LOG_FUNC_CALL();
	return layoutStyle;
	}
	

	
ListItem* ListModel::createItem( const QString& string )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item =  new ListItem(string);
	if( !item )
		{	
       	throw std::bad_alloc();
		}
	return item;	
	}


ListItem* ListModel::createItem( const QString& /*string*/, const QPixmap* /*pixmap*/ )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT( false );	
	return NULL;
	}
	
ListItem* ListModel::createItem(const QString& /*aDetailText*/, const QPixmap** /*aDetailImages*/, const int /*aDetailImageCount*/,  
	const QString& /*aHeadingText*/, const QPixmap** /*aHeadingImages*/, const int /*aHeadingImageCount*/ )
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT( false );	
	return NULL;
	}




/**
 *  ListViewModel
 */

ListViewModel::ListViewModel( int type, int style, QListView *parent )
	: ListModel(type, style, parent )
	{
	SWT_LOG_FUNC_CALL();
	}
	
	
ListViewModel::~ListViewModel()
	{
	SWT_LOG_FUNC_CALL();
	}
	
	
ListItem* ListViewModel::createItem( const QString& string )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item =  new ListViewItem(string, NULL );
	if( !item )
		{	
       	throw std::bad_alloc();
		}
	return item;	
	}
	
ListItem* ListViewModel::createItem( const QString& string, const QPixmap* pixmap )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item =  new ListViewItem( string, pixmap );
	if( !item )
		{	
       	throw std::bad_alloc();
		}
	return item;	
	}


// ListBoxModel

ListBoxModel::ListBoxModel( int type, int style, QListView *parent )
	: ListModel(type, style, parent ),
	detailTxtIndex(-1),
	headingTxtIndex(-1),
	detailTxtSize(QPoint(0, 0)),
	headingTxtSize(QPoint(0, 0)),
	detailImgCount(0),
	headingImgCount(0),
	imgHeight(0),
	detailImgWidths(NULL),
	headingImgWidths(NULL),
	rowHeights(NULL),
	detailImgWidth(0),
	headingImgWidth(0)
	{
	SWT_LOG_FUNC_CALL();
	}
	
ListBoxModel::~ListBoxModel()
	{
	SWT_LOG_FUNC_CALL();

	delete detailImgWidths;
	delete headingImgWidths;
	delete rowHeights;
	}

	
void ListBoxModel::clearList()
	{
	SWT_LOG_FUNC_CALL();
	ListModel::clearList();
	detailTxtIndex = -1;
	headingTxtIndex = -1;
	detailTxtSize = QPoint(0, 0);
	headingTxtSize = QPoint(0, 0);
	detailImgCount = 0;
	headingImgCount = 0;
	imgHeight = 0;
	delete detailImgWidths;
	detailImgWidths = NULL;
	delete headingImgWidths;
	headingImgWidths = NULL;
	delete rowHeights;
	rowHeights = NULL;
	detailImgWidth = 0;
	headingImgWidth = 0;
	}

void ListBoxModel::appendItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
	const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount )
	{
	SWT_LOG_FUNC_CALL();
	ListItem* item = createItem( aDetailText, aDetailImages,  aDetailImageCount,  
		aHeadingText, aHeadingImages, aHeadingImageCount );
	list.append(item);

	// remember the index of the item, which has the longest detail text
	const QListView* listView = static_cast<const QListView*>( static_cast<QObject*>(this)->parent() );
	if(!aDetailText.isNull())
		{
		int textWidth = listView->fontMetrics().boundingRect(aDetailText).width();	
		if( detailTxtSize.x() < textWidth )
			{
			detailTxtSize.rx() = textWidth;
			detailTxtIndex = list.size() - 1;
			}
		}
	
	// remember the index of the item, which has the longest heading text
	if( (layoutStyle | LB_STYLE_1LINE_ITEM) != 0 || (layoutStyle | LB_STYLE_2LINE_ITEM) != 0 )
		{
		if(!aHeadingText.isNull())
			{
			int textWidth = listView->fontMetrics().boundingRect(aHeadingText).width();	
			if( headingTxtSize.x() < textWidth )
				{
				headingTxtSize.rx() = textWidth;
				headingTxtIndex = list.size() - 1;
				}
			}
		}
	
	// remember the maximum detail image count
	// and remember the maximum widths for every columns of detail images
	if( (layoutStyle | LB_MOD_SHOW_DETAIL_ICONS) != 0 )	
		{
		if( detailImgCount < aDetailImageCount )
			{
			int* alloc =  new int[aDetailImageCount];
			memset(alloc, '\0', sizeof(int)*aDetailImageCount);
			if( !alloc )
				{	
		       	throw std::bad_alloc();
				}
			for(int i = 0; i < detailImgCount; i++)
				{
				alloc[i] =  detailImgWidths[i];
				}
			delete 	detailImgWidths;
			detailImgWidths = alloc;
			detailImgCount = aDetailImageCount;
			}	
		for(int i = 0; i < aDetailImageCount; i++)
			{
			if( aDetailImages[i] )
				{
				QSize size = aDetailImages[i]->size();
				if(detailImgWidths[i] < size.width())
					{
					detailImgWidths[i] = size.width();
					}
				if(imgHeight < size.height())
					{
					imgHeight = size.height();
					}
				}	
			}	
		}
		
	// remember the maximum detail image count
	// and remember the maximum widths for every columns of heading images
	if( (layoutStyle | LB_MOD_SHOW_HEADING_ICONS) != 0 )	
		{
		if( headingImgCount < aHeadingImageCount )
			{
			int* alloc =  new int[aHeadingImageCount ];
			memset(alloc, '\0', sizeof(int)*aHeadingImageCount);
			if( !alloc )
				{	
		       	throw std::bad_alloc();
				}
			for(int i = 0; i < headingImgCount; i++)
				{
				alloc[i] =  headingImgWidths[i];
				}
			delete 	headingImgWidths;
			headingImgWidths = alloc;
			headingImgCount = aHeadingImageCount;
			}	
		for(int i = 0; i < aHeadingImageCount; i++)
			{
			if( aHeadingImages[i] )
				{
				QSize size = aHeadingImages[i]->size();
				if(headingImgWidths[i] < size.width())
					{
					headingImgWidths[i] = size.width();
					}
				if(imgHeight < size.height())
					{
					imgHeight = size.height();
					}
				}	
			}	
		}
	}
	
	
void ListBoxModel::updateLayoutData( const int row, const int rowHeight, const QPoint detailTxtSize, 
		const QPoint headingTxtSize, const int detailImgWidth, const int headingImgWidth ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	
	if(!rowHeights)
		{
		int count = list.size();
		rowHeights =  new int[count];
		if( !rowHeights )
			{	
	       	throw std::bad_alloc();
			}
		memset(rowHeights, '\0', sizeof(int)*count);
		}
		
	rowHeights[row] = rowHeight;
    this->detailTxtSize = detailTxtSize;
    this->headingTxtSize = headingTxtSize;
    this->detailImgWidth = detailImgWidth;
    this->headingImgWidth = headingImgWidth;
	}	

const QString& ListBoxModel::headingText( const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return static_cast<ListBoxItem*>(list.at(row))->headingText();
	}
	
	
int ListBoxModel::detailImageCount() const
	{
	SWT_LOG_FUNC_CALL();
	return detailImgCount;
	}
	
int ListBoxModel::headingImageCount() const
	{
	SWT_LOG_FUNC_CALL();
	return headingImgCount;
	}

const int& ListBoxModel::detailImageWidth(const int index) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(index > -1 && index < detailImgCount);
	return detailImgWidths[index];
	}
	
const int& ListBoxModel::headingImageWidth(const int index) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(index > -1 && index < headingImgCount);
	return headingImgWidths[index];
	}

	
int ListBoxModel::detailImageCount( const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return static_cast<ListBoxItem*>(list.at(row))->detailImageCount();
	}
	
int ListBoxModel::headingImageCount( const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return static_cast<ListBoxItem*>(list.at(row))->headingImageCount();
	}
	
const QList<QPixmap>& ListBoxModel::detailImages(const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return static_cast<ListBoxItem*>(list.at(row))->detailImages();
	}
	
const QList<QPixmap>& ListBoxModel::headingImages( const int row ) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return static_cast<ListBoxItem*>(list.at(row))->headingImages();
	}
	
QPoint ListBoxModel::detailTextSize() const
	{
	SWT_LOG_FUNC_CALL();
	return detailTxtSize;
	}
	
QPoint ListBoxModel::headingTextSize() const
	{
	SWT_LOG_FUNC_CALL();
	return headingTxtSize;
	}
	
QPoint ListBoxModel::detailImageSize(const int row) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return QPoint(detailImgWidth, rowHeights[row]);
	}
	
QPoint ListBoxModel::headingImageSize(const int row) const
	{
	SWT_LOG_FUNC_CALL();
	Q_ASSERT(row > -1 && row < list.size());
	return QPoint(headingImgWidth, rowHeights[row]);
	}
	
int ListBoxModel::rowWithMaxDetailTextSize() const
	{
	SWT_LOG_FUNC_CALL();
	return detailTxtIndex;
	}
	
int ListBoxModel::rowWithMaxHeadingTextSize() const
	{
	SWT_LOG_FUNC_CALL();
	return headingTxtIndex;
	}
	
	
ListItem* ListBoxModel::createItem(const QString& aDetailText, const QPixmap** aDetailImages, const int aDetailImageCount,  
	const QString& aHeadingText, const QPixmap** aHeadingImages, const int aHeadingImageCount )
	{
	SWT_LOG_FUNC_CALL();
	
	ListItem* item =  new ListBoxItem( aDetailText, aDetailImages, aDetailImageCount, aHeadingText, aHeadingImages, aHeadingImageCount );
	if( !item )
		{	
       	throw std::bad_alloc();
		}
		
	return item;	
	}



	
/**
 *  ListItemDelegate
 */	

ListBoxItemDelegate::ListBoxItemDelegate( QObject * parent ) 
	: QItemDelegate( parent ),
	headingFont( NULL )
	{
	SWT_LOG_FUNC_CALL();
	QListView* listView = static_cast< QListView* >( parent );
	dataModel = static_cast < ListBoxModel* > ( listView->model() );
	}
	
void ListBoxItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
	{
	SWT_LOG_FUNC_CALL();

    Q_ASSERT(index.isValid());

	// setup option
    QStyleOptionViewItemV3 opt = setOptions(index, option);
    const QStyleOptionViewItemV2* v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
    opt.features = v2 ? v2->features : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    opt.locale = v3 ? v3->locale : QLocale();
    opt.widget = v3 ? v3->widget : 0;

    painter->save();
    painter->setClipRect(opt.rect);
 
    
    // retrieve cached layout data from data model
    const int row = index.row();
	QRect headingImageRect(QPoint(0, 0), dataModel->headingImageSize(row));
	QRect detailImageRect(QPoint(0, 0),  dataModel->detailImageSize(row));
	QRect headingTextRect(QPoint(0, 0),  dataModel->headingTextSize());
	QRect detailTextRect(QPoint(0, 0),  dataModel->detailTextSize());
	
	// get check rectangle
	Qt::CheckState checkState = static_cast<Qt::CheckState>(dataModel->data(index, Qt::CheckStateRole).toInt());
	QRect checkRect = (dataModel->flags(index) & Qt::ItemIsUserCheckable) != 0  ? check( opt, opt.rect, checkState ) : QRect();
	if(checkRect.isValid())
		{
		checkRect.setRight(checkRect.right() + 2 * margin(opt));	
		}
		
	doLayout( option, &checkRect, &headingImageRect, &detailImageRect, & headingTextRect, &detailTextRect );

	const int layoutStyle = dataModel->style();
	
    drawBackground(painter, opt, index);
    
    if(checkRect.isValid())
	    {
	    drawCheck( painter, opt, checkRect, checkState );
	    }
    
    // draw heading images
	if ( ( layoutStyle & LB_MOD_SHOW_HEADING_ICONS ) != 0 && dataModel->headingImageCount(row) > 0 )
		{
		drawImages( painter, opt, headingImageRect, dataModel->headingImages(row), &(dataModel->headingImageWidth(0)), dataModel->headingImageCount(row) );
		}
		
	// draw detail images	
	if ( ( layoutStyle & LB_MOD_SHOW_DETAIL_ICONS ) != 0 && dataModel->detailImageCount(row) > 0 )
		{
		drawImages( painter, opt, detailImageRect, dataModel->detailImages(row), &(dataModel->detailImageWidth(0)), dataModel->detailImageCount(row) );
		}
		
	// draw detail text	
	if(detailTextRect.isValid())
		{
		SWT_LOG_DATA_4("Draw detail text; size=(%d, %d); topleft=(%d, %d)", detailTextRect.width(), detailTextRect.height(), detailTextRect.left(), detailTextRect.top() );   
	    drawDisplay( painter, opt, detailTextRect, dataModel->data(index, Qt::DisplayRole).toString() );
	    drawFocus( painter, opt, detailTextRect);
		}
    
	// draw heading text
	if ( headingTextRect.isValid() && ( ( layoutStyle & LB_STYLE_1LINE_ITEM ) != 0 ||  ( layoutStyle & LB_STYLE_2LINE_ITEM ) != 0  ))
		{
	    if ( headingFont != NULL)
	    	{
	        opt.font = *headingFont;
	        opt.fontMetrics = QFontMetrics( *headingFont );
	    	}
			
		SWT_LOG_DATA_4("Draw heading text; size=(%d, %d); topleft=(%d, %d)", headingTextRect.width(), headingTextRect.height(), headingTextRect.right(), headingTextRect.top() );   
		drawDisplay(painter, opt, headingTextRect, dataModel->headingText(row) );
    	drawFocus(painter, opt, headingTextRect);
		}
		
    painter->restore();
    
	}

void ListBoxItemDelegate::computeSizes( const QModelIndex& index, const QStyleOptionViewItem& option, QSize* checkSize, 
	QSize *headingImageSize, QSize* detailImageSize, QSize* headingTextSize, QSize* detailTextSize ) const
 	{
 	SWT_LOG_FUNC_CALL();
	const int layoutStyle = dataModel->style();
	
	 int imageMargin = margin(option);
	 // go through every columns of heading images to get width of entire heading image area
	 // go through every heading images in the row to get the height of entire heading image area
     if ( ( layoutStyle & LB_MOD_SHOW_HEADING_ICONS ) != 0 && dataModel->headingImageCount() > 0 ) 
		{
		int width = 0;	
		int height = 0;
		int headingImageCount = dataModel->headingImageCount();
		int itemHeadingImageCount = dataModel->headingImageCount(index.row());
		const QPixmap* pixmap;
		for( int i = 0; i < headingImageCount; i++)   
			{
			width += dataModel->headingImageWidth(i);
			pixmap = &(dataModel->headingImages(index.row())[i]);
			if(i <  itemHeadingImageCount && pixmap)
				{
				if( pixmap->size().height() > height )
					{
					height = pixmap->size().height();
					}
				}
			}
		width += (headingImageCount - 1) * imageMargin;	
		*headingImageSize = QSize( width, height );
 		SWT_LOG_DATA_3("heading image rect row=%d; size=(%d, %d);", index.row(), headingImageSize->width(), headingImageSize->height() );   
		} 
     else
		{
		*headingImageSize = QSize();
		}	

	 // go through every columns of detail images to get width of entire detail image area
	 // go through every detail images in the row to get height of entire detial image area
     if ( ( layoutStyle & LB_MOD_SHOW_DETAIL_ICONS ) != 0 && dataModel->detailImageCount() > 0 ) 
    	{
		int width = 0;	
		int height = 0;
		int detailImageCount = dataModel->detailImageCount();
		int itemDetailImageCount = dataModel->detailImageCount(index.row());
		const QPixmap* pixmap;
		for( int i = 0; i < detailImageCount; i++)   
			{
			width += dataModel->detailImageWidth(i);
			pixmap = &(dataModel->detailImages(index.row())[i]);
			if(i <  itemDetailImageCount && pixmap)
				{
				if( pixmap->size().height() > height )
					{
					height = pixmap->size().height();
					}
				}
			}
		width += (detailImageCount - 1) * imageMargin;	
		*detailImageSize = QSize( width, height );
 		SWT_LOG_DATA_3("detail image rect row=%d; size=(%d, %d);", index.row(), detailImageSize->width(), detailImageSize->height() );   
    	}
    else	 
    	{
    	*detailImageSize = QSize();
    	}

	const QStyleOptionViewItemV2* v2 = qstyleoption_cast< const QStyleOptionViewItemV2* >(&option);
    const bool wrapText = v2->features & QStyleOptionViewItemV2::WrapText;
    const int width = wrapText && option.rect.isValid() ? option.rect.width() : 0xffff;
	
	// compute width of heading text upon the item which has the longest heading text
	int row;
	if ( ( layoutStyle & LB_STYLE_1LINE_ITEM ) != 0  || ( layoutStyle & LB_STYLE_2LINE_ITEM ) != 0 ) 
		{
		row = dataModel->rowWithMaxHeadingTextSize();
		if( row > -1)
			{
			const QString& text = dataModel->headingText( row );
			if( !text.isNull() )
				{
				const QFont& font = headingFont ? *headingFont : option.font;
				*headingTextSize = textRectangle( width, font, text ).size();
				}
			}
		}
	else
		{
		*headingTextSize = QSize();
		}
	    
	// compute width of detail text upon the item which has the longest detail text
	row = dataModel->rowWithMaxDetailTextSize();
	if( row > -1)
		{
		const QString& text = dataModel->data( index, Qt::DisplayRole).toString();
		if( !text.isNull() )
			{
			*detailTextSize = textRectangle( width, option.font, text ).size();
			}
		}
	else
		{
		*detailTextSize = QSize();
		}
		

    *checkSize = check( option, option.rect, Qt::Unchecked ).size();
  }

void ListBoxItemDelegate::addMargins( const QStyleOptionViewItem &option, QSize* checkSize, 
	QSize* headingImageSize, QSize* detailImageSize, QSize* headingTextSize, QSize* detailTextSize ) const
	{
 	SWT_LOG_FUNC_CALL();
 	
    const bool hasCheck = checkSize->isValid();
    const bool hasHeadingImage = headingImageSize->isValid();
    const bool hasDetailImage = detailImageSize->isValid();
 
    const int pixmapMargin = margin(option);
    const int checkMargin = pixmapMargin;
    
    // don't need to add margin to text since text size includes margin
    
    // set height for text if text height is 0 and there is no image
    if ( detailTextSize->height() == 0 && !hasHeadingImage && !hasDetailImage  )
    	{
        detailTextSize->setHeight( option.fontMetrics.height() );
     	}
 	if ( headingTextSize->height() == 0 && !hasHeadingImage && !hasDetailImage )
    	{
	    if ( headingFont )
		    {
	        headingTextSize->setHeight( QFontMetrics( *headingFont ).height() );
		    }
		else
			{
			headingTextSize->setHeight( detailTextSize->height() );	
			}	    
	    }


    if ( hasHeadingImage ) 
    	{
  	    headingImageSize->rwidth() += pixmapMargin * 2;
    	}
    
    if ( hasDetailImage ) 
    	{
  	    detailImageSize->rwidth() += pixmapMargin * 2;
     	}
    	
    if ( hasCheck ) 
    	{
  	    checkSize->rwidth() += checkMargin * 2;
    	}
	}

QSize ListBoxItemDelegate::computeSize(  const QStyleOptionViewItem &option, const QSize& check, const QSize& headingImage, 
	const QSize& detailImage, const QSize& headingText, const QSize& detailText ) const
	{
	SWT_LOG_FUNC_CALL();
 	const int layoutStyle = dataModel->style();
 	int h, w;
    if ( ( layoutStyle &  LB_STYLE_2LINE_ITEM ) != 0 ) 
        {
		const int textWidth = qMax( headingText.width(),  detailText.width() );	
    	const int textMargin = margin(option);
        h = qMax( check.height(), qMax( headingText.height() + detailText.height() + textMargin,
        	qMax( headingImage.height(), detailImage.height() ) ) );
        w = check.width() + textWidth + headingImage.width() + detailImage.width();
        }
    else 
    	{
        h = qMax( check.height(), qMax( qMax( headingText.height(), detailText.height() ),
        	qMax( headingImage.height(), detailImage.height() ) ) );
        w = check.width() + headingText.width() + detailText.width() + headingImage.width() + detailImage.width();
    	}
    	
    return QSize( w, h );	
	}


void ListBoxItemDelegate::layoutRects( const QStyleOptionViewItem &option, QRect* check, QRect *headingImage, 
	QRect* detailImage, QRect* headingText, QRect* detailText, bool hint ) const
	{
	SWT_LOG_FUNC_CALL();
	
	const int layoutStyle = dataModel->style();
 	int h, w; 
 	int textWidth;
 	if( ( layoutStyle & LB_STYLE_2LINE_ITEM ) != 0 )
	 	{
 		textWidth = qMax( headingText->width(),  detailText->width() );
	 	}
 	else
	 	{
 		textWidth = headingText->width() + detailText->width();
	 	}
 	
    if ( hint )
	    {
		QSize size = computeSize( option, check->size(), headingImage->size(), detailImage->size(), headingText->size(), detailText->size() );
		h = size.height();
		w = size.width();
	    }
	else
		{
        w = option.rect.width();
        h = option.rect.height();
		} 
	const int x = option.rect.left(), y = option.rect.top();   
	SWT_LOG_DATA_4("space for layout: size=(%d, %d); topleft=(%d, %d)", w, h, x, y );   
	
    int leftX = x, rightX = x + w;	
	if ( option.direction == Qt::RightToLeft ) 
     	{
     	if ( check->isValid() )	
         	{
            check->setRect( qMax( rightX - check->width(), leftX ) , y, qMin( check->width(), rightX - leftX ), h );
            rightX = check->left();
         	}
         	
        if ( headingImage->isValid() )
        	{
        	if ( rightX > leftX )
            	{	
            	headingImage->setRect( qMax( rightX - headingImage->width(), leftX ), y, qMin( headingImage->width(), rightX - leftX  ) , h );
            	rightX = headingImage->left();
            	}
        	else
            	{
            	*headingImage = QRect();
            	}
        	} 
        	
        if ( detailImage->isValid() )
        	{
        	if (  rightX > leftX  )
            	{
            	int detailImageX = leftX;
            	int detailImageWidth = detailImage->width();
            	if( (rightX -leftX) >= (textWidth + detailImageWidth) )
	            	{
	            	detailImageX = rightX - textWidth - detailImageWidth;
	            	}
            	else
	            	{
	            	detailImageX = qMin( rightX, leftX + detailImageWidth );
	             	}
           		detailImage->setRect( detailImageX, y, qMin( detailImageWidth, rightX - detailImageX ), h );
            	leftX = detailImage->right();
            	}
        	else
            	{
            	*detailImage = QRect();
            	}
        	}
        	
        if (  ( layoutStyle & LB_STYLE_2LINE_ITEM ) != 0 )
            {
            if ( rightX > leftX )	
	            {
		    	const int textMargin = margin(option);
    			headingText->setRect( qMax( leftX, rightX - textWidth ), y, qMin( textWidth, rightX - leftX ), headingText->height() );
    			detailText->setRect( headingText->left(), headingText->bottom() + textMargin, headingText->width(), detailText->height() );
	            }
	        else
		        {
		        *headingText = QRect();	
		        *detailText = QRect();	
		        }    
            }
        else
	        {
            if ( headingText->isValid() )	
	            {
	            if ( rightX > leftX )
		            {
	            	headingText->setRect( qMax( rightX - headingText->width(), leftX ), y, qMin( headingText->width(), rightX - leftX  ) , h );
	            	rightX = headingText->left();
		            }
		        else 
		        	{
			        *headingText = QRect();	
		        	}
	            }  
	              
            if ( detailText->isValid() )	
	            {
	            if ( rightX > leftX )
		            {
	            	detailText->setRect( qMax( rightX - detailText->width(), leftX ), y, qMin( detailText->width(), rightX - leftX  ) , h );
		            }
		        else 
		        	{
			        *detailText = QRect();	
		        	}
	            }    
	        }  
     	}
 	else // left to right
     	{
     	if (check->isValid() )	
         	{
            check->setRect( leftX, y, qMin( check->width(), rightX - leftX ), h );
            leftX = check->right();
         	}
         	
        if ( headingImage->isValid() )
        	{
        	if ( rightX > leftX )
            	{	
            	headingImage->setRect( leftX, y, qMin( headingImage->width(), rightX - leftX  ) , h );
            	leftX = headingImage->right();
            	}
        	else
            	{
            	*headingImage = QRect();
            	}
        	} 
        	
        if ( detailImage->isValid() )
        	{
        	if (  rightX > leftX  )
            	{
            	int detailImageX = leftX;
            	int detailImageWidth = detailImage->width();
            	if( (rightX -leftX) >= (textWidth + detailImageWidth) )
	            	{
	            	detailImageX = leftX + textWidth;
	            	}
            	else
	            	{
	            	detailImageX = qMax( leftX, rightX - detailImageWidth );
	             	}
           		detailImage->setRect( detailImageX, y, qMin( detailImageWidth, rightX - detailImageX ), h );
            	rightX = detailImage->left();
            	}
        	else
            	{
            	*detailImage = QRect();
            	}
        	}
        	
        if (  ( layoutStyle & LB_STYLE_2LINE_ITEM ) != 0 )
            {
            if ( rightX > leftX )	
	            {
				const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
				const QWidget* widget = v3 ? v3->widget : NULL;
			    QStyle* style = widget ? widget->style() : QApplication::style();
		    	const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
    			headingText->setRect( leftX , y,  qMin( textWidth, rightX - leftX ), headingText->height() );
    			detailText->setRect( headingText->left(), headingText->bottom() + textMargin, headingText->width(), detailText->height() );
	            }
	        else
		        {
		        *headingText = QRect();	
		        *detailText = QRect();	
		        }    
            }
        else
	        {
            if ( headingText->isValid() )	
	            {
	            if ( rightX > leftX )
		            {
	            	headingText->setRect( leftX, y, qMin( headingText->width(), rightX - leftX  ) , h );
	            	leftX = headingText->right();
		            }
		        else 
		        	{
			        *headingText = QRect();	
		        	}
	            }  
	              
            if ( detailText->isValid() )	
	            {
	            if ( rightX > leftX )
		            {
	            	detailText->setRect( leftX, y, qMin( detailText->width(), rightX - leftX  ) , h );
		            }
		        else 
		        	{
			        *detailText = QRect();	
		        	}
	            }    
	        }  
    	} 
	SWT_LOG_DATA_4("check rect: size=(%d, %d); topleft=(%d, %d)", check->width(), check->height(), check->left(), check->top() );   
	SWT_LOG_DATA_4("heading image rect: size=(%d, %d); topleft=(%d, %d)", headingImage->width(), headingImage->height(), headingImage->left(), headingImage->top() );   
	SWT_LOG_DATA_4("detail image rect: size=(%d, %d); topleft=(%d, %d)", detailImage->width(), detailImage->height(), detailImage->left(), detailImage->top() );   
	SWT_LOG_DATA_4("heading text rect: size=(%d, %d); topleft=(%d, %d)", headingText->width(), headingText->height(), headingText->left(), headingText->top() );   
	SWT_LOG_DATA_4("detail text rect: size=(%d, %d); topleft=(%d, %d)", detailText->width(), detailText->height(), detailText->left(), detailText->top() );   
    	 
 	}


void ListBoxItemDelegate::doLayout( const QStyleOptionViewItem &option, QRect* checkRect, QRect* headingImageRect,
	QRect* detailImageRect, QRect* headingTextRect, QRect* detailTextRect ) const
	{
    Q_ASSERT( checkRect && headingImageRect && detailImageRect && headingTextRect && detailTextRect );
    
    // duplicate rects
 	QRect check = *checkRect;
 	QRect headingImage = *headingImageRect;
 	QRect detailImage = *detailImageRect;
 	QRect headingText = *headingTextRect;
 	QRect detailText = *detailTextRect;
 	
    // layout duplicated rects	
	layoutRects( option, &check, &headingImage, &detailImage, &headingText, &detailText, false );
	

	// remove margins
	const int mg = margin(option);
	if( checkRect->isValid() )
		{
	 	checkRect->setRight( checkRect->right() - 2 * mg );
		}
	if( headingImageRect->isValid() )
		{
	 	headingImageRect->setRight( headingImageRect->right() - 2 * mg );
		}
	if( detailImageRect->isValid() )
		{
	 	detailImageRect->setRight( detailImageRect->right() - 2 * mg );
		}
	
	// adust rectangles	
    *checkRect = QStyle::alignedRect( option.direction, Qt::AlignCenter, checkRect->size(), check );
    *headingImageRect = QStyle::alignedRect( option.direction, option.decorationAlignment, headingImageRect->size(), headingImage );
    *detailImageRect = QStyle::alignedRect( option.direction, option.decorationAlignment, detailImageRect->size().boundedTo(detailImage.size()), detailImage );
     if (option.showDecorationSelected)
    	{
    	// take up entire space	
        *headingTextRect = headingText;
        *detailTextRect = detailText;
    	}
    else
    	{
    	// aligne to the space
        *headingTextRect = QStyle::alignedRect( option.direction, option.displayAlignment, 
        	headingTextRect->size().boundedTo(headingText.size()), headingText );
        *detailTextRect = QStyle::alignedRect( option.direction, option.displayAlignment, 
        	detailTextRect->size().boundedTo(detailText.size()), detailText );
    	}
	}

QSize ListBoxItemDelegate::sizeHint(const QStyleOptionViewItem &option,
	const QModelIndex &index) const
	{
 	SWT_LOG_FUNC_CALL();
   	
 	QSize checkSize;
 	QSize headingImageSize;
 	QSize detailImageSize;
 	QSize headingTextSize;
 	QSize detailTextSize;

	computeSizes( index, option, &checkSize, &headingImageSize, &detailImageSize, &headingTextSize, &detailTextSize ); 
	addMargins( option, &checkSize, &headingImageSize, &detailImageSize, &headingTextSize, &detailTextSize ); 
	SWT_LOG_DATA_2("check size=(%d, %d) ", checkSize.width(), checkSize.height() );   
	SWT_LOG_DATA_2("heading image size=(%d, %d) ", headingImageSize.width(), headingImageSize.height() );   
	SWT_LOG_DATA_2("detail imageSize size=(%d, %d) ", detailImageSize.width(), detailImageSize.height() );   
	SWT_LOG_DATA_2("headingText size=(%d, %d) ", headingTextSize.width(), headingTextSize.height() );   
	SWT_LOG_DATA_2("detail text size=(%d, %d) ", detailTextSize.width(), detailTextSize.height() );   
	QSize size = computeSize( option, checkSize, headingImageSize, detailImageSize, headingTextSize, detailTextSize );
	SWT_LOG_DATA_2("hint size=(%d, %d) ", size.width(), size.height() ); 
	dataModel->updateLayoutData( index.row(), size.height(), 
		QPoint( detailTextSize.width(), detailTextSize.height() ), 
		QPoint( headingTextSize.width(), headingTextSize.height() ), 
		detailImageSize.width(), headingImageSize.width() );
    return size;
	}
	
void ListBoxItemDelegate::setHeadingFont( QFont* font )
	{
 	SWT_LOG_FUNC_CALL();
	headingFont = font;
	}
	

QStyleOptionViewItem ListBoxItemDelegate::setOptions(const QModelIndex &index,
	const QStyleOptionViewItem &option) const
	{
	SWT_LOG_FUNC_CALL();
    QStyleOptionViewItem opt = option;
    
	// ask data modle for font, alignment and brush, then set them
	
   // set font
    QVariant value = index.data(Qt::FontRole);
    if (value.isValid()){
        opt.font = qvariant_cast<QFont>(value).resolve(opt.font);
        opt.fontMetrics = QFontMetrics(opt.font);
    }

    // set text alignment
    value = index.data(Qt::TextAlignmentRole);
    if (value.isValid())
        opt.displayAlignment = static_cast<Qt::Alignment>(value.toInt());

    // set foreground brush
    value = index.data(Qt::ForegroundRole);
    if (qVariantCanConvert<QBrush>(value))
        opt.palette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));
        
    return opt;
	}

QRect ListBoxItemDelegate::textRectangle( const int width, const QFont &font, const QString &text) const
	{
 	SWT_LOG_FUNC_CALL();
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WordWrap);
    textLayout.setTextOption( textOption );
    textLayout.setFont( font );
    
    // replace new line char with QChar::LineSeparator
    QString replaceText = text;
    const QChar nl = QLatin1Char('\n');
    for (int i = 0; i < text.count(); ++i)
    	{
        if (replaceText.at(i) == nl)
            replaceText[i] = QChar::LineSeparator;
    	}
    textLayout.setText( replaceText );
    
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    while (true) 
    	{
        QTextLine line = textLayout.createLine();
        if  ( !line.isValid() )
        	{
            break;
        	}
        line.setLineWidth( width );
        line.setPosition( QPointF( 0, height ) );
        height += line.height();
        widthUsed = qMax( widthUsed, line.naturalTextWidth() );
    	}
    textLayout.endLayout();
    QSize size =  QSizeF(widthUsed, height).toSize();
    
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    return QRect(0, 0, size.width() + 2 * textMargin, size.height());
	}

int ListBoxItemDelegate::margin( const QStyleOptionViewItem &option ) const
	{
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	const QWidget* widget = v3 ? v3->widget : NULL;
    QStyle* style = widget ? widget->style() : QApplication::style();
	return style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
	}

void ListBoxItemDelegate::drawImages( QPainter* painter, const QStyleOptionViewItem &option, 
	const QRect imageRect, const QList<QPixmap>& images, const int* imageWidths, const int imageCount ) const
	{
	SWT_LOG_FUNC_CALL();
	SWT_LOG_DATA_4( "Heading image rect, row=%d: size=(%d, %d); topleft=(%d, %d)", imageRect.width(), imageRect.height(), imageRect.left(), imageRect.top() );
	const int imageMargin = margin(option);
	const int top = imageRect.top();
	const int height = imageRect.height();
	int left = imageRect.left();
	int right = left;
	QRect rect;
	if( option.direction == Qt::RightToLeft )
		{
		right = imageRect.right();
		left=right;
		for( int i = 0; i < imageCount; i++)   
			{
			left -= imageWidths[i];
			rect = QRect( left, top, right - left, height );
			if( !rect.isValid() )break;
	   		rect = QStyle::alignedRect( option.direction, option.decorationAlignment, images[i].size(), rect );
		    drawDecoration( painter, option, rect, images[i] );
			SWT_LOG_DATA_5("Image No.%d, size=(%d, %d); topleft=(%d, %d)", i, right - left, height, left, top );
			right = left - imageMargin;
			left = right;
			}
		}
	else
		{
		for( int i = 0; i < imageCount; i++)   
			{
			right += imageWidths[i];
			rect = QRect( left, top, right - left, height );
			if( !rect.isValid() )break;
	   		rect = QStyle::alignedRect( option.direction, option.decorationAlignment, images[i].size(), rect );
		    drawDecoration( painter, option, rect, images[i] );
			SWT_LOG_DATA_5("Image No.%d, size=(%d, %d); topleft=(%d, %d)", i, right - left, height, left, top );
			left = right + imageMargin;
			right = left;
			}
		}	
	}


