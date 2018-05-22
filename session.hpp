#ifndef SESSION_HPP
#define SESSION_HPP

#include <QObject>
#include <QFile>
#include "table.hpp"
#include <QTextStream>
#include <QMessageBox>

enum MeasurementType {

	MT_NO,		// no measurement
	MT_OD_NO,	// OD without optical aid
	MT_OS_NO,	// OS without optical aid
	MT_OD_TF,	// OD with trial frame
	MT_OS_TF,	// OS with trial frame
	MT_OD_HA,	// OD with aperture
	MT_OS_HA	// OS with aperture
};

/*
enum ExaminationType {

	ET_VDN,	// Visus OD without optical aid
	ET_VSN,	// Visus OS without optical aid
	ET_VDT,	// Visus OD with trial frame
	ET_VST,	// Visus OS with trial frame
	ET_VDA,	// Visus OD with aperture
	ET_VSA,	// Visus OS with aperture
	ET_VDC,	// Visus OD with aperture, bedingt
	ET_VSC,	// Visus OS with aperture, bedingt
	ET_CDN,	// Contrast OD without optical aid
	ET_CSN,	// Contrast OS without optical aid
	ET_CDT,	// Contrast OD with trial frame
	ET_CST,	// Contrast OS with trial frame
	ET_CDA,	// Contrast OD with aperture
	ET_CSA,	// Contrast OS with aperture
	ET_CDC,	// Contrast OD with aperture, bedingt
	ET_CSC,	// Contrast OS with aperture, bedingt
	ET_NO	// no examination
};
*/

static QString const MeasurementText[ ] = {

	"keine Untersuchung",
	"Oculus Dexter  -  ohne optische Hilfsmittel",
	"Oculus Sinister  -  ohne optische Hilfsmittel",
	"Oculus Dexter  -  mit Messbrille",
	"Oculus Sinister  -  mit Messbrille",
	"Oculus Dexter  -  mit Lochblende",
	"Oculus Sinister  -  mit Lochblende"
};

class Measurement {

	public:

		Measurement( ) :
		measurementType( MT_NO ),
		__measured( false ),
		__value( 0. ) {

		}

		Measurement( MeasurementType p_measurementType ) :
		measurementType( p_measurementType ),
		__measured( false ),
		__value( 0. ) {

		}

		Measurement( Measurement const & p_measurement ) :
		measurementType( p_measurement.measurementType ),
		__measured( false ),
		__value( p_measurement.__value ) {

		}

		~Measurement( ) {

		}

	public:

		MeasurementType
		measurementType;

	private:

		bool
		__measured;

		double
		__value;

	public:

		void
		measure( double const &p_value ) {

			__value = p_value;
			__measured = true;
		}

		bool
		measured( ) const {

			return __measured;
		}

		double
		value( ) const {

			return __value;
		}

		QString
		typeToStr( ) {

			return MeasurementText[ measurementType ];
		}

		QString
		valToStr( ) {

			return QString( "%1" ).arg( __value );
		}

		QString
		measuredToStr( ) {

			return QString( "%1" ).arg( __measured ? "yes" : "no" );
		}
};

class Examination :
public Measurement {

	public:

		Examination( ) :
		Measurement( ),
		__conditioned( false ) {

		}

		Examination( MeasurementType p_measurementType, bool p_conditioned = false ) :
		Measurement( p_measurementType ),
		__conditioned( p_conditioned ) {

		}

		Examination( Examination const & p_examination ) :
		Measurement( p_examination ),
		__conditioned( p_examination.__conditioned ) {

		}

		~Examination( ) {

		}

	private:

		bool
		__conditioned;

	public:

		bool
		conditioned( ) const {

			return __conditioned;
		}

		QString
		typeToStr( ) {

			return Measurement::typeToStr( ) + ( __conditioned ? ", falls Messung ohne Messbrille besser war, als die mit Messbrille" : "" );
		}

		QString
		conditionedToStr( ) {

			return measured( ) ? "yes" : "no";
		}
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
		examinations( p_s.examinations ) {

		}

		virtual
		~Session( ) {

		}

	public:

		QString
		name;

		int
		currSessionId;

		QVector< Examination >
		examinations;

		void
		clear( ) {

			currSessionId = 0;
			examinations.clear( );
			name.clear( );
		}

		void
		addExamintion( MeasurementType p_mt, bool p_conditioned = false ) {

			examinations << Examination( p_mt, p_conditioned );
		}

		void
		setExamination( int p_id, MeasurementType p_mt, bool p_conditioned = false ) {

			examinations[ p_id ] = Examination( p_mt, p_conditioned );
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

			for( auto it = examinations.cbegin( ); it != examinations.cend( ); ++ it ) {

				ts << it->value( ) << "\n";
			}

			file.close( );

			return true;
		}

		MeasurementType
		start( ) {

			currSessionId = 0;

			return currSessionId < examinations.size( ) ? examinations[ currSessionId ].measurementType : MT_NO;
		}

		MeasurementType
		next( ) {

			return ++ currSessionId < examinations.size( ) ? examinations[ currSessionId ].measurementType : MT_NO;
		}
};
#endif // SESSION_HPP
