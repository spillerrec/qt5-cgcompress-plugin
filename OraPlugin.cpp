#include <QImage>
#include <QByteArray>
#include <QImageIOHandler>
#include <QImageIOPlugin>

#include "OraPlugin.hpp"
#include "OraHandler.hpp"

QStringList OraPlugin::keys() const{
	return QStringList() << "cgcompress";
}


QImageIOPlugin::Capabilities OraPlugin::capabilities( QIODevice *device, const QByteArray &format ) const{
	if( format == "cgcompress" )
		return Capabilities( CanRead );
	else
		return 0;
}

QImageIOHandler* OraPlugin::create( QIODevice *device, const QByteArray &format ) const{
	return (QImageIOHandler*) new OraHandler( device );
}


