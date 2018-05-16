#ifndef SESSION_HPP
#define SESSION_HPP

#include <QObject>
#include <QFile>
#include "table.hpp"
#include <QTextStream>
#include <QMessageBox>

enum MeasurementType {

	VDN,	// Visus OD without optical aid
	VSN,	// Visus OS without optical aid
	VDT,	// Visus OD with trial frame
	VST,	// Visus OS with trial frame
	VDA,	// Visus OD with aperture
	VSA,	// Visus OS with aperture
	VDC,	// Visus OD with aperture, condition VD > VDTF
	VSC, 	// Visus OS with aperture, condition VS > VSTF
	CDN,	// Contrast OD without optical aid
	CSN,	// Contrast OS without optical aid
	CDF,	// Contrast OD with trial frame
	CSF,	// Contrast OS with trial frame
	CDA,	// Contrast OD with aperture
	CSA,	// Contrast OS with aperture
	CDC,	// Contrast OD with aperture, condition CD > CDTF
	CSC, 	// Contrast OS with aperture, condition CS > CSTF
	NO		// no measurement
};

class Session {

	public:

		Session( ) :
		currSessionId( 0 ) {

		}

		Session( QString const & p_name ) :
		name( p_name ),
		currSessionId( 0 ) {

		}

		Session( Session const & p_s ) :
		name( p_s.name ),
		currSessionId( p_s.currSessionId ),
		measurements( p_s.measurements ) {

		}

		virtual
		~Session( ) {

		}

	public:

		QString
		name;

		int
		currSessionId;

		QVector< MeasurementType >
		measurements;

		void
		clear( ) {

			currSessionId = 0;
			measurements.clear( );
			name.clear( );
		}

		void
		addMeasurement( MeasurementType p_pt ) {

			measurements << p_pt;
		}

		void
		setMeasurement( int p_id, MeasurementType p_pt ) {

			measurements[ p_id ] = p_pt;
		}

		void
		delMeasurement( int p_id ) {

			measurements[ p_id ] = NO;
		}

		bool
		save( ) {

			QFile
			file( "sessions.dat" );

			while( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

				QMessageBox
				mb;

				mb.setText( "Kann sessions.dat nicht speichern. Bitte schliessen Sie die Datei und klicken Sie dann auf OK" );

				mb.exec( );
			}

			QTextStream
			ts( &file );

			for( auto it = measurements.cbegin( ); it != measurements. cend( ); ++ it ) {

				ts << *it << "\n";
			}

			file.close( );

			return true;
		}

		MeasurementType
		start( ) {

			currSessionId = 0;

			return measurements[ currSessionId ];
		}

		MeasurementType
		next( ) {

			return measurements[ ++ currSessionId ];
		}
};
#endif // SESSION_HPP
