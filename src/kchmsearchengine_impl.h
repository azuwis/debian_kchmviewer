/***************************************************************************
 *   Copyright (C) 2004-2007 by Georgy Yunaev, gyunaev@ulduzsoft.com       *
 *   Portions Copyright (C) 2000-2005 Trolltech AS.                        * 
 *   Please do not use email address above for bug reports; see            *
 *   the README file                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QASSISTANTINDEX_H
#define QASSISTANTINDEX_H

#include <qstringlist.h>
#include <qdict.h>
#include <qdatastream.h>
#include <qobject.h>

namespace QtAs
{

struct Document
{
	Document( int d, int f ) : docNumber( d ), frequency( f ) {}
	Document() : docNumber( -1 ), frequency( 0 ) {}
	bool operator==( const Document &doc ) const
	{
		return docNumber == doc.docNumber;
	}
	
	bool operator<( const Document &doc ) const
	{
		return frequency > doc.frequency;
	}
	
	bool operator<=( const Document &doc ) const
	{
		return frequency >= doc.frequency;
	}
	
	bool operator>( const Document &doc ) const
	{
		return frequency < doc.frequency;
	}
	
	Q_INT16 docNumber;
	Q_INT16 frequency;
};

QDataStream &operator>>( QDataStream &s, Document &l );
QDataStream &operator<<( QDataStream &s, const Document &l );

class Index : public QObject
{
    Q_OBJECT
	public:
		struct Entry
		{
			Entry( int d ) { documents.append( Document( d, 1 ) ); }
			Entry( QValueList<Document> l ) : documents( l ) {}
			QValueList<Document> documents;
		};
		
		struct PosEntry
		{
			PosEntry( int p ) { positions.append( p ); }
			QValueList<uint> positions;
		};

		Index( const QString &dp, const QString &hp );
		Index( const QStringList &dl, const QString &hp );
		
		void 		writeDict();
		bool 		readDict();
		bool 		makeIndex();
		QStringList query( const QStringList&, const QStringList&, const QStringList& );
		
		void 		setDictionaryFile( const QString& );
		void 		setDocListFile( const QString& );
		void 		setDocList( const QStringList & );
		QString 	getCharsSplit() const { return m_charssplit; }
		QString 	getCharsPartOfWord() const { return m_charsword; }

	signals:
		void indexingProgress( int );

	public slots:
		void setLastWinClosed();

	private:
		void	setupDocumentList();
		bool	parseDocumentToStringlist( const QString& filename, QStringList& tokenlist );
		void	parseDocument( const QString& filename, int docnum );
		void	insertInDict( const QString&, int );
		
		void	writeDocumentList();
		bool	readDocumentList();
		
		QStringList				getWildcardTerms( const QString& );
		QStringList				split( const QString& );
		QValueList<Document> 	setupDummyTerm( const QStringList& );
		bool 					searchForPhrases( const QStringList &phrases, const QStringList &words, const QString &filename );
		
		QStringList 		docList;
		QDict<Entry> 		dict;
		QDict<PosEntry>		miniDict;
		QString 			docPath;
		QString 			dictFile;
		QString 			docListFile;
		bool 				lastWindowClosed;
	
		// Those characters are splitters (i.e. split the word), but added themselves into dictionary too.
		// This makes the dictionary MUCH larger, but ensure that for the piece of "window->print" both 
		// search for "print" and "->print" will find it.
		QString m_charssplit;

		// Those characters are parts of word - for example, '_' is here, and search for _debug will find only _debug.
		QString m_charsword;
};

struct Term
{
	Term( const QString &t, int f, QValueList<Document> l ) : term( t ), frequency( f ), documents( l ) {}
	
	QString 				term;
	int 					frequency;
	QValueList<Document>	documents;
};

class TermList : public QPtrList<Term>
{
	public:
		TermList() : QPtrList<Term>() {}
		int compareItems( QPtrCollection::Item i1, QPtrCollection::Item i2 );
};

};

#endif /* QASSISTANTINDEX_H */