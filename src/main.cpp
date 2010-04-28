/**************************************************************************
 *  Kchmviewer - a CHM file viewer with broad language support            *
 *  Copyright (C) 2004-2010 George Yunaev, kchmviewer@ulduzsoft.com       *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *																	      *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "kde-qt.h"

#include "mainwindow.h"
#include "config.h"
#include "keyeventfilter.h"
#include "dbus_interface.h"
#include "version.h"

#if !defined (WIN32)
	#include <QtDBus/QtDBus>
#endif

#if defined (USE_KDE)
	#include <kaboutdata.h>
#endif

MainWindow * mainWindow;


int main( int argc, char ** argv )
{
#if defined (USE_KDE)
 	KCmdLineOptions options;
	options.add( "autotestmode", ki18n("Perform auto testing") );
	options.add( "shortautotestmode", ki18n("Perform short auto testing") );
	options.add( "+[chmfile]", ki18n("A CHM file to show") );
	options.add( "search <query>", ki18n("'--search <query>' specifies the search query to search, and activate the first entry if found") );
	options.add( "sindex <word>", ki18n("'--sindex <word>' specifies the word to find in index, and activate if found") );
	options.add( "stoc <word>", ki18n("'--stoc <word(s)>' specifies the word(s) to find in TOC, and activate if found. Wildcards allowed.") );

	KAboutData aboutdata ( "kchmviewer",
				QByteArray(),
				ki18n("kchmviewer"),
				qPrintable( QString("%1.%2") .arg(APP_VERSION_MAJOR) .arg(APP_VERSION_MINOR) ),
				ki18n("CHM file viewer"),
				KAboutData::License_GPL,
				ki18n("(c) 2004-2008 George Yunaev, gyunaev@ulduzsoft.com"),
				ki18n("Please report bugs to kchmviewer@ulduzsoft.com"),
				"http://www.kchmviewer.net",
				"kchmviewer@ulduzsoft.com");

	KCmdLineArgs::init (argc, argv, &aboutdata);
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app;
#else
	QApplication app( argc, argv );

	app.addLibraryPath ( "qt-plugins" );
#endif
	
	// Set data for QSettings
	QCoreApplication::setOrganizationName("Ulduzsoft");
	QCoreApplication::setOrganizationDomain("kchmviewer.net");
	QCoreApplication::setApplicationName("kchmviewer");

	// Configuration
	pConfig = new Config();

	app.installEventFilter( &gKeyEventFilter );
	
#if !defined (WIN32)	
	if ( QDBusConnection::sessionBus().isConnected() )
	{
		if ( QDBusConnection::sessionBus().registerService(SERVICE_NAME) )
		{
			DBusInterface * dbusiface = new DBusInterface();
			QDBusConnection::sessionBus().registerObject( "/", dbusiface, QDBusConnection::ExportAllSlots );
		}
		else
			qWarning( "Cannot register service %s on session bus. Going without D-BUS support.", SERVICE_NAME );
	}
	else
		qWarning( "Cannot connect to the D-BUS session bus. Going without D-BUS support." );
#endif

	mainWindow = new MainWindow();
	mainWindow->show();
	
	app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
	
	return app.exec();
}
