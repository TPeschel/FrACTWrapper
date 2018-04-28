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


enum Acuity_LandoltCModes {

	ODWithoutOpticalAid,
	OSWithoutOpticalAid,
	ODWithTrialFrame,
	OSWithTrialFrame,
	ODWithHoleAperture,
	OSWithHoleAperture
};

static QString const modeStrings[ ] = {

"OD_PLACEHOLDER_WithoutOpticalAid",
"OS_PLACEHOLDER_WithoutOpticalAid",
"OD_PLACEHOLDER_WithTrialFrame",
"OS_PLACEHOLDER_WithTrialFrame",
"OD_PLACEHOLDER_WithHoleAperture",
"OS_PLACEHOLDER_WithHoleAperture"
};

enum StackedWidgetID {

	ADDNEWEXAMINATOR = 0,
	LOBBY,
	STARTSESSION,
	VISUSEXAMINATION,
	TABLEVIEW,
	SETUP,
	HELP
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

	void showWidget( StackedWidgetID const & p_id );
	void startFrACT( );
	void updateConfigView( );
	void updateExaminatorsView( );
	void updateExaminationView();
	void checkForExistingConfig( );
	void checkForExistingExaminators( );


public slots:

	void slotAddNewExaminator( );
	void slotShowLobby( );
	void slotShowProperties( );
	void slotExaminatorNameChanged( int p_id );
	void slotExaminatorIDChanged( int p_id );
	void slotNewExaminatorOK( );
	void slotNewExaminatorCancel( );
	void slotStartSession( );
	void slotScanSic( const QString & p_sic );
	void slotFrACTFinished( int p_exitCode );
	void slotStartFrACTAcuity_LandoltCOD( );
	void slotStartFrACTAcuity_LandoltCOS( );
	void slotStartFrACTAcuity_LandoltCODWithTrialFrame( );
	void slotStartFrACTAcuity_LandoltCOSWithTrialFrame( );
	void slotStartFrACTAcuity_LandoltCODWithHoleAperture( );
	void slotStartFrACTAcuity_LandoltCOSWithHoleAperture( );
	void slotDoVisusTest( );
	void slotStartFileDialogForFlashPlayer( );
	void slotStartFileDialogForFractSWF( );
	void slotStartFileDialogForDataDir( );
	void slotFinishSetup( );
	void slotFinishExamination( );

	signals:

	void signalNoConfigFound( );
	void signalNoExaminatorFound( );

private:

	Ui::MainWindow
	*ui;

private:

	Table
	config,
	tableExaminators,
	tableSession,
	tableSessions;

	QString
	currExaminatorName,
	currExaminatorID,
	currSIC,
	flashPlayerEXE,
	fractSWF,
	dataDir;

	QProcess
	*proc;

	Acuity_LandoltCModes
	currMode;

	StackedWidgetID
	currWidget,
	lastWidget;

	struct VisusMeasurements {

		bool
		measured;

		double
		value;

		VisusMeasurements( ) : measured( false ), value( 0. ) {

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

	VisusMeasurements
	visusMeasurements[ 6 ];
};

#endif // MAINWINDOW_H
