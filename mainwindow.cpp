#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

QTableWidgetItem
* createTableItemAlignCenter( QString const & p_value ) {

	QTableWidgetItem* tmp = new QTableWidgetItem( p_value );
	tmp->setTextAlignment( Qt::AlignCenter );
	return tmp;
}

MainWindow::MainWindow( QWidget * parent ) :
QMainWindow( parent ),
ui( new Ui::MainWindow ) {
	ui->setupUi( this );

	currWidget = LOBBY;
	lastWidget = LOBBY;

	proc = nullptr;

	tableSession.newHeader(
	QStringList( ) <<
	"SIC" << "DATE" << "TIME" << "RESULT" <<
	"RESULT_UNIT" << "OCULUS" << "DETAILS" << "TEST_NAME" <<
	"DISTANCE" << "DISTANCE_UNIT" << "N_TRIALS" << "ID" << "COMMENT" );

	tableSessions.newHeader( tableSession.header( ) );


//    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
//    this->setWindowFlags( this->windowFlags( ) & ~( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::Window | Qt::MSWindowsFixedSizeDialogHint ) );
//    this->setFixedSize(this->width(), this->height());

	connect( this,                            SIGNAL( signalNoConfigFound( ) ),     this, SLOT( slotShowProperties( ) ) );

	connect( ui->actionEinstellungen,         SIGNAL( triggered( ) ),               this, SLOT( slotShowProperties( ) ) );
	connect( ui->lineEditSIC,                 SIGNAL( textChanged( QString ) ),     this, SLOT( slotScanSic( QString ) ) );

	connect( ui->comboBoxExaminatorName,      SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorNameChanged( int ) ) );
	connect( ui->comboBoxExaminatorID,        SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorIDChanged( int ) ) );

	connect( ui->pushButtonAddNewAccount,     SIGNAL( released( ) ), this, SLOT( slotAddNewExaminator( ) ) );
	connect( ui->pushButtonNewAccountOK,      SIGNAL( released( ) ), this, SLOT( slotNewExaminatorOK( ) ) );
	connect( ui->pushButtonNewAccountAbort,   SIGNAL( released( ) ), this, SLOT( slotNewExaminatorCancel( ) ) );
	connect( ui->pushButtonStartSession,      SIGNAL( released( ) ), this, SLOT( slotStartSession( ) ) );
	connect( ui->pushButtonStartVisusTest,    SIGNAL( released( ) ), this, SLOT( slotDoVisusTest( ) ) );
	connect( ui->pushButtonODVisus,           SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCOD( ) ) );
	connect( ui->pushButtonOSVisus,           SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCOS( ) ) );
	connect( ui->pushButtonODVisusTrialFrame, SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCODWithTrialFrame( ) ) );
	connect( ui->pushButtonOSVisusTrialFrame, SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCOSWithTrialFrame( ) ) );
	connect( ui->pushButtonODVisusAperture,   SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCODWithHoleAperture( ) ) );
	connect( ui->pushButtonOSVisusAperture,   SIGNAL( released( ) ), this, SLOT( slotStartFrACTAcuity_LandoltCOSWithHoleAperture( ) ) );

	connect( ui->toolButtonFlashPlayer,       SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForFlashPlayer( ) ) );
	connect( ui->toolButtonFractSWF,          SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForFractSWF( ) ) );
	connect( ui->toolButtonDataDir,           SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForDataDir( ) ) );

	connect( ui->pushButtonSetupBack,         SIGNAL( released( ) ), this, SLOT( slotFinishSetup( ) ) );
	connect( ui->pushButtonFinishExamination, SIGNAL( released( ) ), this, SLOT( slotFinishExamination( ) ) );

	connect( ui->pushButtonExitFromTabView,       SIGNAL( released( ) ), this, SLOT( close( ) ) );
	connect( ui->pushButtonNewProbandFromTabView, SIGNAL( released( ) ), this, SLOT( slotStartSession( ) ) );

	connect( ui->action_Beenden, SIGNAL(triggered( ) ), this, SLOT( close( ) ) );

	ui->tableWidgetExamination->horizontalHeader( )->setSectionResizeMode( QHeaderView::Stretch );
	ui->tableWidgetExaminators->horizontalHeader( )->setSectionResizeMode( QHeaderView::Stretch );

	showWidget( StackedWidgetID::LOBBY );

	checkForExistingConfig( );

	updateConfigView( );

	checkForExistingExaminators( );

	updateExaminatorsView( );

	updateExaminationView( );
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::checkForExistingConfig( ) {

	if( ! config.loadFromFile( "config.tsv" ) ) {

		config.clearAll( );

		config.newHeader( QStringList( ) << "FLASHPLAYER" << "FRACTSWF" << "DATADIR" );

		config.addRow( QStringList( ) << "Flash Player Projector" << "FrACT*.swf" << "data directory" );

		config.saveTSV( "config.tsv" );

		ui->statusBar->showMessage( "Creating dummy config.tsv ... Please configure first!" );

		emit signalNoConfigFound( );
	}
}

void MainWindow::checkForExistingExaminators( ) {

	if( ! tableExaminators.loadFromFile( "examinators.tsv" ) ) {

		tableExaminators.clearAll( );

		tableExaminators.newHeader( QStringList( ) << "ID" << "NAME" );

		tableExaminators.saveTSV( "examinators.tsv" );

		emit signalNoExaminatorFound( );
	}
}

void MainWindow::showWidget( StackedWidgetID const & p_id ) {

	lastWidget = currWidget;
	currWidget = p_id;

	ui->spinBoxView->setValue( p_id );
	ui->stackedWidget->setCurrentIndex( p_id );
}

void MainWindow::startFrACT( ) {

	QApplication::clipboard( )->clear( QClipboard::Clipboard );

	if( proc )

		delete proc;

	proc = new QProcess( );

	QObject::connect( proc, SIGNAL( finished( int ) ), this, SLOT( slotFrACTFinished( int ) ) );

	QStringList
	arg;

	//arg << "http://www.michaelbach.de/fract/versions/FrACT3.10.0d.swf";
	//arg << "C:/Users/Thomas Peschel/Documents/dev/c++/life/Franziska/github.com/TPeschel/VisusTestWrapperQt/release/flash/FrACT3.10.0d.swf";
	arg << fractSWF;
	//proc->setWorkingDirectory ( "C:/Users/Thomas Peschel/Documents/dev/c++/life/Franziska/github.com/TPeschel/VisusWarapperQt/release/" );
	proc->setWorkingDirectory ( dataDir );

	proc->start( flashPlayerEXE, arg ); //windows

	// QTimer::singleShot( 2000, this, SLOT( slotPressA( ) ) );

	//QApplication::sendEvent( proc, &event );
//    proc->start( "flash/start.bat" ); //windows
	//    proc->startDetached ( "flashplayer", arg ); //linux
}

void MainWindow::updateConfigView( ) {

	flashPlayerEXE = config.get( 0, 0 );
	fractSWF       = config.get( 0, 1 );
	dataDir        = config.get( 0, 2 );

	ui->lineEditFlashPlayer->setText( flashPlayerEXE );
	ui->lineEditFractSWF->setText( fractSWF );
	ui->lineEditDataDir->setText( dataDir );
}

void MainWindow::updateExaminationView( ) {

	ui->tableWidgetExamination->clear( );
	ui->tableWidgetExamination->setRowCount( tableSession.rows( ) );
	ui->tableWidgetExamination->setColumnCount( tableSession.cols( ) );
	ui->tableWidgetExamination->setHorizontalHeaderLabels( tableSession.header( ) );
	ui->tableWidgetExamination->horizontalHeader( )->setSectionResizeMode( QHeaderView::ResizeToContents );

	for( int r = 0; r < tableSession.rows( ); ++ r ) {

		for( int c = 0; c < tableSession.cols( ); ++ c ) {

			ui->tableWidgetExamination->setItem( r, c, createTableItemAlignCenter( tableSession.get( r, c ) ) );
		}
	}
}

void MainWindow::updateExaminatorsView( ) {

	ui->comboBoxExaminatorName->clear( );
	ui->comboBoxExaminatorID->clear( );

	for( int i = 0; i < tableExaminators.rows( ); ++ i ) {

		ui->comboBoxExaminatorID->addItem( tableExaminators.get( i, 0 ) );
		ui->comboBoxExaminatorName->addItem( tableExaminators.get( i, 1 ) );
	}

	currExaminatorName = ui->comboBoxExaminatorName->currentText( );
	currExaminatorID = ui->comboBoxExaminatorID->currentText( );

	ui->tableWidgetExaminators->clear( );
	ui->tableWidgetExaminators->setRowCount( tableExaminators.rows( ) );
	ui->tableWidgetExaminators->setColumnCount( tableExaminators.cols( ) );

	ui->tableWidgetExaminators->setHorizontalHeaderLabels( tableExaminators.header( ) );

	for( int r = 0; r < tableExaminators.rows( ); ++ r ) {

		for( int c = 0; c < tableExaminators.cols( ); ++ c ) {

			ui->tableWidgetExaminators->setItem( r, c, createTableItemAlignCenter( tableExaminators.get( r, c ) ) );
		}
	}
}

void MainWindow::slotAddNewExaminator( ) {

	ui->lineEditExaminatorID->setText( "" );
	ui->lineEditExaminatorName->setText( "" );

	showWidget( ADDNEWEXAMINATOR );
}

void MainWindow::slotDoVisusTest( ) {

	ui->stackedWidget->setCurrentIndex( VISUSEXAMINATION );

	ui->pushButtonODVisus->setFocus( );
}

void MainWindow::slotExaminatorNameChanged( int p_id ) {

	ui->comboBoxExaminatorID->setCurrentIndex( p_id );

	currExaminatorName = ui->comboBoxExaminatorName->currentText( );

	currExaminatorID= ui->comboBoxExaminatorID->currentText( );

	ui->labelHelloMsg->setText( "Hallo " + currExaminatorName + "\nIhre ID: " + currExaminatorID );
}

void MainWindow::slotExaminatorIDChanged( int p_id ) {

	ui->comboBoxExaminatorName->setCurrentIndex( p_id );

	currExaminatorName = ui->comboBoxExaminatorName->currentText( );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );

	ui->labelHelloMsg->setText( "Hallo " + currExaminatorName + "\nIhre ID: " + currExaminatorID );
}

void MainWindow::slotFinishSetup( ) {

	showWidget( lastWidget );
}

void MainWindow::slotFinishExamination( ) {

	for( int r = 0; r < tableSession.rows( ); ++ r ) {

		QString
		oc = tableSession.get( r, 5 ),
		oa = tableSession.get( r, 6 );

		if( oc == "OD" ) {

			if( oa == "WithoutOpticalAid" ) {

				tableSession.set( r, 12, ui->lineEditCommentOD->text( ) );
			}
			else if ( oa == "WithTrialFrame" ) {

				tableSession.set( r, 12, ui->lineEditCommentODTF->text( ) );
			}
			else if ( oa == "WithHoleAperture" ) {

				tableSession.set( r, 12, ui->lineEditCommentODA->text( ) );
			}
		}
		else if( oc == "OS" ) {

			if( oa == "WithoutOpticalAid" ) {

				tableSession.set( r, 12, ui->lineEditCommentOS->text( ) );
			}
			else if ( oa == "WithTrialFrame" ) {

				tableSession.set( r, 12, ui->lineEditCommentOSTF->text( ) );
			}
			else if ( oa == "WithHoleAperture" ) {

				tableSession.set( r, 12, ui->lineEditCommentOSA->text( ) );
			}
		}
	}

	updateExaminationView( );

	QDate
	d = QDate::currentDate( );

	QTime
	t = QTime::currentTime( );

	QStringList
	dt;

	dt << d.toString( "yyyyMMdd" ) << t.toString( "HHmmss" );

	QString
	filename( dataDir + "/Session-" + currSIC + "-" + dt.join( "-" ) + "-Acuity_LandoltC.tsv" );

	tableSession.saveTSV( filename );

	showWidget( TABLEVIEW );

	ui->tableWidgetExamination->show( );
}

void MainWindow::slotFrACTFinished( int p_exitCode ) {

	std::cout << "exit code: " << p_exitCode << std::endl;

	if( p_exitCode != 0 )

		return;

	QClipboard
	*cb = QApplication::clipboard( );

	QString
	txt = cb->text( QClipboard::Clipboard );

	std::cout << "txt: " << txt.toStdString( ) << std::endl;

	Row
	row = txt.split( QRegExp( "[\t\n\r]" ), QString::SkipEmptyParts );

	if( 0 < row.size( ) && row.at( 0 )[ 0 ] == '2' ) {

		QStringList
		date,
		time;

		date << row.at( 0 ).split( '-' );
		time << row.at( 1 ).split( ':' );

		QString
		dt = date.join( "" ) + "-" + time.join( "" );

		QStringList
		msl = modeStrings[ currMode ].split( "_PLACEHOLDER_" );

		QFile
		fileResult( dataDir + "/" + currSIC + "-" + dt + "-Acuity_LandoltC-" + msl[ 0 ] + "-" + msl[ 1 ] + "-Result.tsv" );

		if( fileResult.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

			QTextStream
			ts( &fileResult );

			ts <<
			"SIC" << "\t" << "DATE" << "\t" << "TIME" << "\t" << "RESULT" << "\t" <<
			"RESULT_UNIT" << "\t" << "OCULUS" << "\t" << "DETAILS" << "\t" << "TEST_NAME" <<
			"\t" << "DISTANCE" << "\t" << "DISTANCE_UNIT" << "\t" << "N_TRIALS" << "\t" << "ID" << "\t" << "COMMENT" << "\n" <<
			currSIC << "\t" <<
			row.at( 0 ) << "\t" <<
			row.at( 1 ) << "\t" <<
			row.at( 2 ) << "\t" <<
			row.at( 3 ) << "\t" <<
			msl[ 0 ] << "\t" <<
			msl[ 1 ] << "\t" <<
			row.at( 4 ) << "\t" <<
			row.at( 5 ) << "\t" <<
			row.at( 6 ) << "\t" <<
			row.at( 7 ) << "\t" <<
			currExaminatorID << "\n";

			fileResult.close( );
		}

		if( 12 < row.size( ) ) {

			QFile
			fileData( dataDir + "/" + currSIC + "-" + dt + "-Acuity_LandoltC-" + msl[ 0 ] + "-" + msl[ 1 ] + "-Data.tsv" );

			if( fileData.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

				QTextStream
				ts( &fileData );

				if( ts.atEnd( ) ) {
					ts <<
					"TRIAL" << "\t" << "VALUE" << "\t" <<
					"ORIENTATION_ANGLE_PRESENTED[DEG]" << "\t" << "ORIENTATION_ANGLE_RESPONDED[DEG]" <<
					"\t" << "CORRECTNESS[1YES0NO]" << "\t" << "REACTION_TIME[MS]" << "\n";
				}

				for( int i = 13; i < row.size( ); ++ i ) {

					ts << row.at( i );

					ts << ( ( i - 13 ) % 6 < 5 ? "\t" : "\n" );
				}

				fileData.close( );
			}
		}

		tableSession.addRow(
		QStringList( ) <<
		currSIC <<
		row.at( 0 ) <<
		row.at( 1 ) <<
		row.at( 2 ) <<
		row.at( 3 ) <<
		msl[ 0 ] <<
		msl[ 1 ] <<
		row.at( 4 ) <<
		row.at( 5 ) <<
		row.at( 6 ) <<
		row.at( 7 ) <<
		currExaminatorID << "" );

		double
		val = row.at( 2 ).toDouble( );

		switch ( currMode ) {

			case ODWithoutOpticalAid : {

				//ui->pushButtonODVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ ODWithoutOpticalAid ].setValue( val );

				break;
			}

			case OSWithoutOpticalAid : {

				//ui->pushButtonOSVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ OSWithoutOpticalAid ].setValue( val );

				break;
			}

			case ODWithTrialFrame : {

				//ui->pushButtonODVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODTFVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ ODWithTrialFrame ].setValue( val );

				break;
			}

			case OSWithTrialFrame : {

				//ui->pushButtonOSVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSTFVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ OSWithTrialFrame ].setValue( val );

				break;
			}

			case ODWithHoleAperture : {

				//ui->pushButtonODVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODAVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ ODWithHoleAperture ].setValue( val );

				break;
			}

			case OSWithHoleAperture : {

				//ui->pushButtonOSVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSAVal->setText( QString( "VA = %1  logMAR = %2" ).arg(val ).arg( -log10( val ) ) );
				visusMeasurements[ OSWithHoleAperture ].setValue( val );

				break;
			}

			default:

				break;
		}

		ui->statusBar->clearMessage( );

		if( visusMeasurements[ ODWithoutOpticalAid ].measured && visusMeasurements[ ODWithTrialFrame ].measured && !visusMeasurements[ ODWithHoleAperture ].measured && visusMeasurements[ ODWithoutOpticalAid ].value > visusMeasurements[ ODWithTrialFrame ].value ) {

			//ui->pushButtonODVisusAperture->setEnabled( true );
			ui->labelODAVal->setStyleSheet( "QLabel { color : red; }" );
			ui->labelODAVal->setText( "noch kein kein Wert gemessen" );

			ui->statusBar->showMessage( "Die Messung des rechten Auges (OD) ohne Messbrille ist besser als die mit Messbrille.\nBitte messen Sie nun mit Messblende!" );
		}

		if( visusMeasurements[ OSWithoutOpticalAid ].measured && visusMeasurements[ OSWithTrialFrame ].measured && !visusMeasurements[ OSWithHoleAperture ].measured && visusMeasurements[ OSWithoutOpticalAid ].value > visusMeasurements[ OSWithTrialFrame ].value ) {

			ui->labelOSAVal->setStyleSheet( "QLabel { color : red; }" );
			ui->labelOSAVal->setText( "noch kein kein Wert gemessen" );

			ui->statusBar->showMessage( "Die Messung des linken Auges (OS) ohne Messbrille ist besser als die mit Messbrille.\nBitte messen Sie nun mit Messblende!" );
		}
	}
}

void MainWindow::slotNewExaminatorOK( ) {

	bool
	ok  = true;

	if( ui->lineEditExaminatorName->text( ).isEmpty( ) ) {

		QMessageBox
		mb;

		mb.setText( "Geben Sie bitte Ihren Namen ein!" );

		mb.exec( );

		ok = false;
	}

	if( ui->lineEditExaminatorID->text( ).isEmpty( ) ) {

		QMessageBox
		mb;

		mb.setText( "Geben Sie bitte Ihre Untersucher-ID ein!" );

		mb.exec( );

		ok = false;
	}

	if( ok ) {

		tableExaminators.loadFromFile( "examinators.tsv", true );

		currExaminatorID = ui->lineEditExaminatorID->text( );

		currExaminatorName = ui->lineEditExaminatorName->text( );

		int
		id = tableExaminators.whereIsInColumn( currExaminatorID );

		if( 0 <= id ) {

			QMessageBox
			mb;

			mb.setText( "Diese Untersucher ID wurde bereits an " + tableExaminators.get( id, 1 ) + " vergeben. Sind Sie " + tableExaminators.get( id, 1 ) + ", dann klicken Sie auf abbrechen. Andernfalls kontaktieren Sie bitte den Studienleiter, um die Eindeutigkeit der Untersucher-IDs zu gewährleisten!" );

			mb.exec( );
		}

		else {

			tableExaminators.addRow( QStringList( { currExaminatorID, currExaminatorName } ) );

			while( ! tableExaminators.saveTSV( "examinators.tsv", true ) ) {

				QMessageBox
				mb;

				mb.setText( "Die Datei examinators.tsv ist in eine anderen Anwendung geöffnet. Schliessen Sie diese bitte, um erfolgreich speichern zu können!" );

				mb.exec( );
			}

			updateExaminatorsView( );

			ui->pushButtonStartSession->setFocus( );

			showWidget( LOBBY );
		}
	}
}

void MainWindow::slotNewExaminatorCancel( ) {

	ui->stackedWidget->setCurrentIndex( LOBBY );
}

void MainWindow::slotScanSic( QString const & p_sic ) {

	QRegularExpression re( "^LI\\d{7}[\\d{1}xX]", QRegularExpression::CaseInsensitiveOption );
	QRegularExpressionMatch m = re.match( p_sic );

	if( m.hasMatch( ) ) {

		currSIC = m.captured( ).toUpper( );

		ui->pushButtonStartVisusTest->setFocus( );

		ui->lineEditSIC->setEnabled( false );

		ui->pushButtonStartVisusTest->setEnabled( true );
	}
}

void MainWindow::slotShowLobby( ) {

	updateExaminatorsView( );
}

void MainWindow::slotShowProperties( ) {

	ui->lineEditFlashPlayer->setText( config.get( 0, 0 ) );
	ui->lineEditFractSWF->setText( config.get( 0, 1 ) );
	ui->lineEditDataDir->setText( config.get( 0, 2 ) );

	lastWidget = SETUP;

	ui->spinBoxView->setValue( SETUP );
}

void MainWindow::slotStartSession( ) {

	tableSession.clearContent( );

	updateExaminationView( );

	for( int i = 0; i < 6; ++ i ) {

		visusMeasurements[ i ].clear( );
	}

	ui->lineEditSIC->setEnabled( true );
	ui->lineEditSIC->clear( );
	ui->lineEditSIC->setFocus( );

	ui->pushButtonStartVisusTest->setEnabled( false );

	ui->labelODVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODVal->setText( "noch kein Wert gemesen" );
	ui->lineEditCommentOD->clear( );

	ui->labelOSVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSVal->setText( "noch kein Wert gemesen" );
	ui->lineEditCommentOS->clear( );

	ui->labelODTFVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODTFVal->setText( "noch kein Wert gemesen" );
	ui->lineEditCommentODTF->clear( );

	ui->labelOSTFVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSTFVal->setText( "noch kein Wert gemesen" );
	ui->lineEditCommentOSTF->clear( );

	ui->labelODAVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODAVal->setText( "keine Messung nötig" );
	ui->lineEditCommentODA->clear( );

	ui->labelOSAVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSAVal->setText( "keine Messung nötig" );
	ui->lineEditCommentOSA->clear( );

	ui->stackedWidget->setCurrentIndex( STARTSESSION );
}

void MainWindow::slotStartFileDialogForFlashPlayer( )
{
	flashPlayerEXE = QFileDialog::getOpenFileName( this, tr( "Flash Player Projector" ), ".", tr( "Exe Datei (*.exe)" ) );

	if( flashPlayerEXE.isEmpty( ) ) {

		return;
	}

	ui->lineEditFlashPlayer->setText( flashPlayerEXE );

	config.set( 0, 0, flashPlayerEXE );

	config.saveTSV( "config.tsv" );
}

void MainWindow::slotStartFileDialogForFractSWF( )
{
	fractSWF = QFileDialog::getOpenFileName( this, tr( "FrACT*.swf" ), ".", tr( "SWF Datei (*.swf)" ) );

	if( fractSWF.isEmpty( ) ) {

		return;
	}

	ui->lineEditFractSWF->setText( fractSWF );

	config.set( 0, 1, fractSWF );

	config.saveTSV( "config.tsv" );
}

void MainWindow::slotStartFileDialogForDataDir( ) {

	dataDir = QFileDialog::getExistingDirectory( this, tr( "Daten Verzeichnis" ), tr( "." ), QFileDialog::ShowDirsOnly );

	if( dataDir.isEmpty( ) ) {

		return;
	}

	ui->lineEditDataDir->setText( dataDir );

	config.set( 0, 2, dataDir );

	config.saveTSV( "config.tsv" );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOD( )
{
	currMode = ODWithoutOpticalAid;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOS( )
{
	currMode = OSWithoutOpticalAid;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCODWithTrialFrame( )
{
	currMode = ODWithTrialFrame;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOSWithTrialFrame( )
{
	currMode = OSWithTrialFrame;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCODWithHoleAperture( ) {

	currMode = ODWithHoleAperture;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOSWithHoleAperture( )
{
	currMode = OSWithHoleAperture;
	startFrACT( );
}
