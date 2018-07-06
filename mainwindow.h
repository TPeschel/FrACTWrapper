#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QClipboard>
#include <QTableWidgetItem>
#include "table.hpp"
#include "xmltree.hpp"
//#include "session.hpp"
#include "order.h"

enum MeasurementType {

	MT_OD_NO,	// OD without optical aid
	MT_OS_NO,	// OS without optical aid
	MT_OD_TF,	// OD with trial frame
	MT_OS_TF,	// OS with trial frame
	MT_OD_HA,	// OD with aperture
	MT_OS_HA,	// OS with aperture
	MT_NO,		// no measurement
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

	"Oculus Dexter  -  ohne optische Hilfsmittel",
	"Oculus Sinister  -  ohne optische Hilfsmittel",
	"Oculus Dexter  -  mit Messbrille",
	"Oculus Sinister  -  mit Messbrille",
	"Oculus Dexter  -  mit Lochblende",
	"Oculus Sinister  -  mit Lochblende",
	"keine Untersuchung"
};

static QString const modeStrings[ ] = {

	"dexter_PLACEHOLDER_none",
	"sinister_PLACEHOLDER_none",
	"dexter_PLACEHOLDER_trial_frame",
	"sinister_PLACEHOLDER_trial_frame",
	"dexter_PLACEHOLDER_hole_aperture",
	"sinister_PLACEHOLDER_hole_aperture",
	"undefined"
};

static QString const fileNameStrings[ ] = {

	"OD-NO",
	"OS-NO",
	"OD-TF",
	"OS-TF",
	"OD-HA",
	"OS-HA"
};

/*static QString const modeText[ ] = {

"no measurement",
"OD without optical aid",
"OS without optical aid",
"OD with trial frame",
"OS with trial frame",
"OD with hole aperture",
"OS with hole aperture"
};
*/
enum StackedWidgetID {

	SWID_ADDNEWEXAMINATOR = 0,
	SWID_LOBBY,
	SWID_STARTSESSION,
	SWID_EXAMINATION,
	SWID_TABLEVIEW,
	SWID_SETUP,
	SWID_HELP
};

struct
Examinator {

	enum SORTITEM { ID, NAME };

	int
	id;

	QString
	name;

	Examinator( ) {

	}

	Examinator( int p_id, QString const & p_name ) :
	id( p_id ),
	name( p_name ) {

	}
};

namespace Ui {
class MainWindow;
}

class MainWindow :
public QMainWindow {

		Q_OBJECT

	public:

		explicit
		MainWindow( QWidget *parent = 0 );

		~MainWindow( );

		void loadConfig( );
		void prepareSession( );
		void saveSession( );
		void switchWidget( StackedWidgetID p_newId );
		void switchWidgetBack( );
		void startFrACT( );
		void updateExaminatorsView( );
		void updateExaminationView( );
		void updatePathsConfigView( );
		void updateTableWidgetResults( );
		bool messageBox( );

	public slots:

		void slotAddNewExaminator( );
		void slotChangeComment( QTableWidgetItem * p_item );
		void slotExaminatorVNameChanged( int p_id );
		void slotExaminatorNNameChanged( int p_id );
		void slotExaminatorIDChanged( int p_id );
		void slotFinishHelp( );
		void slotFinishSetup( );
		void slotFinishSession( );
		void slotCancelSession( );
		void slotFrACTFinished( int p_exitCode );
		void slotNewExaminatorOK( );
		void slotNewExaminatorCancel( );
		void slotStartSession( );
		void slotScanSic( const QString & p_sic );
		void slotShowLobby( );
		void slotShowProperties( );
		void slotShowHelp( );
		void slotStartFrACTOD( );
		void slotStartFrACTOS( );
		void slotStartFrACTODTF( );
		void slotStartFrACTOSTF( );
		void slotStartFrACTODHA( );
		void slotStartFrACTOSHA( );
		void slotStartFileDialogForFlashPlayer( );
		void slotStartFileDialogForFractSWF( );
		void slotStartFileDialogForDataDir( );
		void slotClose( );

	signals:

		void signalNoConfigFound( );
		void signalNoExaminatorFound( );

	private:

		Ui::MainWindow
		*ui;

	private:

		XMLTree
		cfg;

		Table
		tableSession;

		QString
		currSIC,
		currExaminatorID;

		QProcess
		*proc;

		MeasurementType
		currMeasurementType;

		StackedWidgetID
		currWidget;

		QVector< StackedWidgetID >
		widgetHistory;

		struct Measurements {

			bool
			measured;

			double
			value;

			Measurements( ) :
			measured( false ),
			value( 0. ) {

			}

			void
			setValue( double const & p_value ) {

				value    = p_value;
				measured = true;
			}

			void
			clear( ) {

				value = 0.;
				measured = false;
			}
		};

		Measurements
		measurements[ 6 ];

		QStringList
		vnames,
		nnames,
		ids;

		Order
		vnames2id,
		vnames2nnames,
		nnames2id,
		nnames2vnames,
		id2vnames,
		id2nnames;

		std::size_t
		examinationId;

		bool
		fractRunning;
};

#endif // MAINWINDOW_H
