/*
	This file is part of qt5-ora-plugin.

	qt5-ora-plugin is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	qt5-ora-plugin is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with qt5-ora-plugin.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "OraHandler.hpp"

#include <cstring>

#include <QFile>
#include <QImageReader>
#include <QBuffer>
#include <QPainter>

#include <archive_entry.h>

using namespace pugi;

bool OraHandler::canRead() const{
	//TODO: determine peek size
	return device()->peek( 64 ).contains( "image/openraster" );
}

static QByteArray read_data( archive* a ){
	QByteArray raw;
	
	//Read chuncks
	const char *buff;
	size_t size;
	int64_t offset;
	
	while( true ){
		switch( archive_read_data_block( a, (const void**)&buff, &size, &offset ) ){
			case ARCHIVE_OK: raw += QByteArray( buff, size ); break;
			case ARCHIVE_EOF: return raw;
			default:
				qWarning( "Error while reading zip data: %s", archive_error_string(a) );
				return raw;
		}
	}
}

static QImage read_image( archive* a ){
	QByteArray raw = read_data( a );
	QBuffer buf( &raw );
	QImageReader reader( (QIODevice*)&buf );
	return reader.canRead() ? reader.read() : QImage();
}

static QString next_file( archive* a ){
	archive_entry *entry;
	switch( archive_read_next_header( a, &entry ) ){
		case ARCHIVE_EOF: return QString();
		case ARCHIVE_OK:
			return archive_entry_pathname( entry ); //TODO: ensure it treats it as UTF-8
			
		default:
			qWarning( "Can't read the next zip header: %s", archive_error_string(a) );
			return QString();
	}
}

bool OraHandler::read_and_validate( archive *a ){
	//Read and check mime
	QString mime = next_file( a );
	if( mime.isNull() ){
		qWarning( "Ora image is not an usable zip archive" );
		return false;
	}
		
	if( mime != "mimetype" ){
		qWarning( "Ora image must start with the mimetype" );
		return false;
	}
	//TODO: check if it is STORED?
	
	QByteArray mime_data = read_data( a );
	if( mime_data != "image/openraster" ){
		qWarning( "Mimetype does not match" );
		return false;
	}
	
	//Read all files
	QString name;
	while( !(name = next_file( a )).isNull() ){
		if( name.startsWith( "data/" ) ){
			QImage img = read_image( a );
			if( !img.isNull() )
				images.insert( std::pair<QString,QImage>( name, img ) );
			else
				qWarning( "Could not read data file: %s", name.toLocal8Bit().constData() );
		}
		else if( name == "Thumbnails/thumbnail.png" ){
			thumbnail = read_image( a );
		}
		else if( name == "mergedimage.png" ){
			merged = read_image( a );
			if( merged.isNull() )
				qWarning( "Merged image could not be loaded" );
		}
		else if( name == "stack.xml" ){
			QByteArray data = read_data( a );
			if( !doc.load_buffer( data.data(), data.size() ) )
				return false;
		}
		else
			qWarning( "Unknown file: %s", name.toLocal8Bit().constData() );
	}
	
	if( thumbnail.isNull() ){
		qWarning( "Ora image does not contain thumbnail as required" );
		return false;
	}
	//TODO: check for stack.xml
	
	return true;
}

bool OraHandler::load(){
	bool success = true;
	archive* a = archive_read_new();
	archive_read_support_format_zip(a);
	
	QByteArray file = device()->readAll();
	if( archive_read_open_memory( a, file.data(), file.size() ) ){
		qDebug( "couldn't open: %s", archive_error_string(a) );
		success = false;
	}
	else
		success = read_and_validate( a );
	
	archive_read_close( a );
	archive_read_free( a );
	
	return success;
}


QImage OraHandler::render_stack( xml_node node, int width, int height ) const{
	//TODO: check width and height
	QImage output( width, height, QImage::Format_ARGB32 );
	output.fill( qRgba( 0,0,0,0 ) ); //TOOD: necessary?
	QPainter painter( &output );
	
	for( xml_node_iterator it = --node.end(); it != --node.begin(); it-- ){
		std::string name( (*it).name() );
		if( name == "stack" ){
			int x = (*it).attribute( "x" ).as_int( 0 );
			int y = (*it).attribute( "y" ).as_int( 0 );
			
			QImage img = render_stack( *it, width-x, height-y );
			painter.setOpacity( 1.0 );
			painter.drawImage( x, y, img );
		}
		else if( name == "text" ){
			qWarning( "No support for text" );
		}
		else if( name == "layer" ){
			QString source( (*it).attribute( "src" ).value() ); //TODO: ensure it treats it as UTF-8
			int x = (*it).attribute( "x" ).as_int( 0 );
			int y = (*it).attribute( "y" ).as_int( 0 );
			
			double opacity = (*it).attribute( "opacity" ).as_double( 1.0 );
			painter.setOpacity( opacity );
			
			//TODO: visibility
			//TODO: composite-op
			
			std::map<QString,QImage>::const_iterator img_it = images.find( source );
			if( img_it != images.end() )
				painter.drawImage( x, y, img_it->second );
			else
				qWarning( "Layer source not found: %s", source.toLocal8Bit().constData() );
		}
		else{
			qWarning( "Unrecognized element: %s", name.c_str() );
		}
	}
	return output;
}

bool OraHandler::read( QImage *img_pointer ){
	frame++;
	*img_pointer = QImage();
	
	if( frame == 1 )
		if( !load() )
			return false;
	
	if( frame > 1 )
		return false;
	
	if( !merged.isNull() )
		*img_pointer = merged;
	else{
		xml_node img = doc.child( "image" );
		int width = img.attribute( "w" ).as_int();
		int height = img.attribute( "h" ).as_int();
		*img_pointer = render_stack( img, width, height );
	}
	
	return true;
}


