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
#include "table.hpp"
#include "xmltree.hpp"
#include "session.hpp"
#include "order.h"

static QString const modeStrings[ ] = {

"OD_PLACEHOLDER_Normal",
"OS_PLACEHOLDER_Normal",
"OD_PLACEHOLDER_TrialFrame",
"OS_PLACEHOLDER_TrialFrame",
"OD_PLACEHOLDER_HoleAperture",
"OS_PLACEHOLDER_HoleAperture"
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

	ADDNEWEXAMINATOR = 0,
	LOBBY,
	STARTSESSION,
	VISUSEXAMINATION,
	TABLEVIEW,
	SETUP,
	HELP
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
		void saveSession( );
		void showWidget( StackedWidgetID const & p_id );
		void startFrACT( );
		void updateExaminatorsView( );
		void updateExaminationView( );
		void updatePathsConfigView( );
		void updateSessionView( );

	public slots:

		void slotAddNewExaminator( );
		void slotDoVisusTest( );
		void slotExaminatorVNameChanged( int p_id );
		void slotExaminatorNNameChanged( int p_id );
		void slotExaminatorIDChanged( int p_id );
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
		void slotStartFrACTAcuity_LandoltCOD( );
		void slotStartFrACTAcuity_LandoltCOS( );
		void slotStartFrACTAcuity_LandoltCODWithTrialFrame( );
		void slotStartFrACTAcuity_LandoltCOSWithTrialFrame( );
		void slotStartFrACTAcuity_LandoltCODWithHoleAperture( );
		void slotStartFrACTAcuity_LandoltCOSWithHoleAperture( );
		void slotStartFileDialogForFlashPlayer( );
		void slotStartFileDialogForFractSWF( );
		void slotStartFileDialogForDataDir( );

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
		currWidget,
		lastWidget;

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

		QVector< Session >
		sessions;

		int
		currSession;

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
};

#endif // MAINWINDOW_H
