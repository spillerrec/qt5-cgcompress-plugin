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

#ifndef ORA_HANDLER_HPP
#define ORA_HANDLER_HPP

#include <map>

#include <QImageIOHandler>
#include <QImage>
#include <QPainter>

#include <archive.h>

#include "pugixml.hpp"

class OraHandler: public QImageIOHandler{
	private:
		int frame;
		
		QImage thumbnail;
		QImage merged;
		std::map<QString,QImage> images;
		
		pugi::xml_document doc;
		
		bool read_and_validate( archive *a );
		bool load();
		
		void render_stack( pugi::xml_node node, QPainter &painter, int offset_x=0, int offset_y=0 ) const;
	
	public:
		OraHandler( QIODevice *device ) : frame( 0 ){
			setDevice( device );
			setFormat( "ora" );
		}
		
		bool canRead() const;
		bool read( QImage *image );
		
};


#endif