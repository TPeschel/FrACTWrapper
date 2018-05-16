#ifndef TABLE_HPP
#define TABLE_HPP

#include <algorithm>
#include <QAbstractTableModel>
#include <QFile>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QTextStream>

typedef QStringList Row;

class Table {

	public:

		Table( ) {

		}

		~Table( ) {

		}

	private:

		Row
		__header;

		QVector< Row >
		__rows;

	public:
/*
		static void
		copy( QVector< QString > & p_dst, QStringList const & p_src ) {

			p_dst.clear( );

			for( QStringList::const_iterator s = p_src.cbegin( ); s != p_src.cend( ); ++ s ) {

				p_dst.push_back( *s );
			}
		}

		static void
		copy( QStringList & p_dst, QVector< QString > const & p_src ) {

			p_dst.clear( );

			for( QVector< QString >::const_iterator s = p_src.cbegin( ); s != p_src.cend( ); ++ s ) {

				p_dst << *s;
			}
		}
*/
	public:

		void
		addRow( QStringList const & p_row ) {

/*            QVector< QString >
			qv;

			copy( qv, p_row );

			__rows.push_back( qv );*/
			__rows.push_back( p_row );
		}

		int
		cols( ) const {

			return __header.length( );
		}

		void
		clearContent( ) {

			__rows.clear( );
		}

		void
		clearHeader( ) {

			__header.clear( );
		}

		void
		clearAll( ) {

			clearHeader( );
			clearContent( );
		}

		void
		delRow( int const & p_id ) {

			__rows.removeAt( p_id );
		}

		void
		fromString( QString const & p_text, bool p_firstRowIsHeader = false ) {

			QStringList
			dat = p_text.split( QRegExp( "[\n\r]+" ), QString::SkipEmptyParts );

			if( p_firstRowIsHeader ) {

				__header.clear( );

				__header = dat.at( 0 ).split( '\t' );
			}

			for( int i = 1; i < dat.size( ); ++ i ) {

				QStringList
				strLst = dat.at( i ).split( '\t', QString::SkipEmptyParts );

				addRow( strLst );
			}
		}

		QString
		get( int const & p_row, int const & p_col ) const {

			return p_row < __rows.length( ) && p_col < __rows[ p_row ].length( ) ? __rows[ p_row ][ p_col ] : "";
		}

		Row
		header( ) const {

			return __header;
		}

		bool
		load( QString const & p_filename, bool p_firstRowIsHeader = true ) {

			QFile
			file( p_filename );

			if( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {

				if( !file.atEnd( ) ) {

					if( p_firstRowIsHeader ) {

						__header.clear( );

						QString
						str = file.readLine( );

						QStringList
						strLst = str.split( QRegExp( "[\t\n\r]" ), QString::SkipEmptyParts );

						newHeader( strLst );
					}

					__rows.clear( );

					while( !file.atEnd( ) ) {

						QString
						str = file.readLine( );

						QStringList
						strLst = str.split( QRegExp( "[\t\n\r]" ), QString::SkipEmptyParts );

						addRow( strLst );
					}
				}

				file.close( );

				return true;
			}

			return false;
		}

		void
		newHeader( QStringList const & p_columnNames ) {

//            copy( __header, p_columnNames );
			__header = p_columnNames;
		}

		int
		rows( ) const {

			return __rows.length( );
		}

		void
		set( int const & p_row, int const & p_col, QString const & p_value ) {

			while( __rows[ p_row ].length( ) < p_col ) {

				__rows[ p_row ] << "";
			}

			__rows[ p_row ][ p_col ] = p_value;
		}

		bool
		save( QString const & p_filename, bool p_firstRowIsHeader = true ) {

			QFile
			file( p_filename );

			if( file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

				QTextStream
				fl( &file );

				fl.setCodec( "UTF-8" );

				if( p_firstRowIsHeader ) {

					int
					i = __header.length( );

					for( Row::const_iterator it = __header.cbegin( ); it < __header.cend( ); ++ it ) {

						fl << *it << ( 0 < -- i ? '\t' : '\n' );
					}
				}

				for( QVector< Row >::const_iterator row = __rows.cbegin( ); row < __rows.cend( ); ++ row ) {

					int
					i = row->length( );

					for( Row::const_iterator col = row->cbegin( ); col != row->cend( ); ++ col ) {

						fl << *col << ( 0 < -- i ? '\t' : '\n' );
					}
				}

				file.close( );

				return true;
			}

			return false;
		}

		int
		whereInColumnIs( QString const & p_pattern, int p_column = 0, int p_firstRow = 0 ) {

			for( int i = p_firstRow; i < rows( ); ++ i ) {

				if( __rows.at( i )[ p_column  ] == p_pattern )

					return i;
			}

			return -1;
		}
};
#endif // TABLE_H
