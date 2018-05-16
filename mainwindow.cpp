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
	"DISTANCE" << "DISTANCE_UNIT" << "N_TRIALS" << "EID" << "COMMENT" );

//    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
//    this->setWindowFlags( this->windowFlags( ) & ~( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::Window | Qt::MSWindowsFixedSizeDialogHint ) );
//    this->setFixedSize(this->width(), this->height());

	connect( this,                            SIGNAL( signalNoConfigFound( ) ),     this, SLOT( slotShowProperties( ) ) );

	connect( ui->actionEinstellungen,         SIGNAL( triggered( ) ),               this, SLOT( slotShowProperties( ) ) );
	connect( ui->lineEditSIC,                 SIGNAL( textChanged( QString ) ),     this, SLOT( slotScanSic( QString ) ) );

	connect( ui->comboBoxExaminatorVName,     SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorVNameChanged( int ) ) );
	connect( ui->comboBoxExaminatorNName,     SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorNNameChanged( int ) ) );
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
	connect( ui->pushButtonFinishSession,     SIGNAL( released( ) ), this, SLOT( slotFinishSession( ) ) );
	connect( ui->pushButtonCancelSession,     SIGNAL( released( ) ), this, SLOT( slotCancelSession( ) ) );

	connect( ui->pushButtonExitFromTabView,       SIGNAL( released( ) ), this, SLOT( close( ) ) );
	connect( ui->pushButtonNewProbandFromTabView, SIGNAL( released( ) ), this, SLOT( slotStartSession( ) ) );

	connect( ui->action_Beenden,              SIGNAL(triggered( ) ), this, SLOT( close( ) ) );

	ui->tableWidgetExamination->horizontalHeader( )->setSectionResizeMode( QHeaderView::Stretch );

	showWidget( StackedWidgetID::LOBBY );

	loadConfig( );

	updatePathsConfigView( );

	updateSessionView( );

	updateExaminatorsView( );

	updateExaminationView( );
}

MainWindow::~MainWindow( ) {

	delete ui;
}

/*
void MainWindow::checkForExistingConfig( ) {

	if( ! config.load( "config.tsv" ) ) {

		config.clearAll( );

		config.newHeader( QStringList( ) << "FLASHPLAYER" << "FRACTSWF" << "DATADIR" );

		config.addRow( QStringList( ) << "Flash Player Projector" << "FrACT*.swf" << "data directory" );

		config.save( "config.tsv" );

		ui->statusBar->showMessage( "Creating dummy config.tsv ... Please configure first!" );

		emit signalNoConfigFound( );
	}
}
*/

void MainWindow::loadConfig( ) {

	if( ! cfg.load( "cfg.xml" ) ) {

		cfg.clear( );

/*	cfg
		.rp( )
		.tag( "cfg" ).att( "date", QDate::currentDate( ).toString( ) ).att( "time", QTime::currentTime( ).toString( ) );
*/

		cfg
		.rp( )
		.tag( "cfg" ).att( "date", QDate::currentDate( ).toString( ) ).att( "time", QTime::currentTime( ).toString( ) )
			.tag( "paths")
				.tag( "flash" ).txt( "../flash(flash_sa_29.exe" ).gat( )
				.tag( "fract" ).txt( "../fract/FrACT3.10.0d.swf").gat( )
				.tag( "data" ).txt( "../data" ).gat( 2 )
			.tag( "examinators" )
				.tag( "examinator" ).att( "id", "0000" ).att( "name", "Thomas Peschel" ).gat( 2 )
			.tag( "last-examinations" ).att( "id", "0000" );

		cfg.save( "cfg.xml" );

		emit signalNoConfigFound( );
	}
}
/*
	if( ! config.load( "config.tsv" ) ) {

		config.clearAll( );

		config.newHeader( QStringList( ) << "FLASHPLAYER" << "FRACTSWF" << "DATADIR" );

		config.addRow( QStringList( ) << "Flash Player Projector" << "FrACT*.swf" << "data directory" );

		config.save( "config.tsv" );

		ui->statusBar->showMessage( "Creating dummy config.tsv ... Please configure first!" );

		emit signalNoConfigFound( );
	}
}
*/

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

//	QStringList
//	arg;

	//arg << "http://www.michaelbach.de/fract/versions/FrACT3.10.0d.swf";
	//arg << "C:/Users/Thomas Peschel/Documents/dev/c++/life/Franziska/github.com/TPeschel/VisusTestWrapperQt/release/flash/FrACT3.10.0d.swf";
	//arg << cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->attrs( ).value( "path" ).toString( );
	//proc->setWorkingDirectory ( "C:/Users/Thomas Peschel/Documents/dev/c++/life/Franziska/github.com/TPeschel/VisusWarapperQt/release/" );
	proc->setWorkingDirectory ( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->attrs( ).value( "path" ).toString( ) );

	proc->start(
		cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ),
		QStringList( ) << cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) ); //windows

	// QTimer::singleShot( 2000, this, SLOT( slotPressA( ) ) );

	//QApplication::sendEvent( proc, &event );
//    proc->start( "flash/start.bat" ); //windows
	//    proc->startDetached ( "flashplayer", arg ); //linux
}

void MainWindow::updateExaminatorsView( ) {

 	vnames.clear( );
	nnames.clear( );
	ids.clear( );

	int
	id = 0;

	XMLTag
	* run = cfg.rp( ).in( "cfg" ).in( "examinators" ).curr( )->child( "examinator", id );

	while( run ) {

		vnames << run->attrs( ).value( "vname" ).toString( );
		nnames << run->attrs( ).value( "nname" ).toString( );
		ids   << run->attrs( ).value( "id" ).toString( );

		run = cfg.rp( ).in( "cfg" ).in( "examinators" ).curr( )->child( "examinator", ++ id );
	}

	Order
	ordVName = order( vnames, smaller< QString > ),
	ordNName = order( nnames, smaller< QString > ),
	ordId    = order( ids, smaller< QString > ),
	rnkVName = rank( vnames, smaller< QString > ),
	rnkNName = rank( nnames, smaller< QString > ),
	rnkId    = rank( ids, smaller< QString > );

	QStringList
	slVNames,
	slNNames,
	slIds;

	for( int i = 0; i < vnames.size( ); ++ i ) {

		slVNames << vnames[ ordVName[ i ] ];
		slNNames << nnames[ ordNName[ i ] ];
		slIds    << ids[ ordId[ i ] ];
	}

	vnames2id.reserve( vnames.size( ) );
	vnames2nnames.reserve( vnames.size( ) );
	nnames2vnames.reserve( nnames.size( ) );
	nnames2id.reserve( nnames.size( ) );
	id2vnames.reserve( ids.size( ) );
	id2nnames.reserve( ids.size( ) );

	vnames2id.clear( );
	vnames2nnames.clear( );
	nnames2vnames.clear( );
	nnames2id.clear( );
	id2vnames.clear( );
	id2nnames.clear( );

	for( std::size_t i = 0; i < rnkNName.size( ); ++ i ) {

		nnames2id[ i ] = rnkId[ ordNName[ i ] ];
		vnames2id[ i ] = rnkId[ ordVName[ i ] ];
		id2nnames[ i ] = rnkNName[ ordId[ i ] ];
		id2vnames[ i ] = rnkVName[ ordId[ i ] ];
		vnames2nnames[ i ] = rnkNName[ ordVName[ i ] ];
		nnames2vnames[ i ] = rnkVName[ ordNName[ i ] ];
	}

	ui->comboBoxExaminatorVName->clear( );
	ui->comboBoxExaminatorVName->addItems( slVNames );

	ui->comboBoxExaminatorNName->clear( );
	ui->comboBoxExaminatorNName->addItems( slNNames );

	ui->comboBoxExaminatorID->clear( );
	ui->comboBoxExaminatorID->addItems( slIds );

	QString
	eid = cfg.rp( ).in( "cfg" ).in( "last-session-examinator-id" ).curr( )->attrs( ).value( "id" ).toString( );

	for( int i = 0; i < ids.size( ); ++ i ) {

		if( ids[ ordId[ i ] ] == eid ) {

			ui->comboBoxExaminatorVName->setCurrentIndex( i );
			ui->comboBoxExaminatorNName->setCurrentIndex( vnames2nnames[ i ] );
			ui->comboBoxExaminatorID->setCurrentIndex( vnames2id[ i ] );

			break;
		}
	}
}

void MainWindow::updatePathsConfigView( ) {

	ui->lineEditFlashPlayer->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ) );
	ui->lineEditFractSWF->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) );
	ui->lineEditDataDir->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) );
}

void MainWindow::updateSessionView( ) {

	sessions.clear( );

	for( int i = 0; i < cfg.rp( ).in( "cfg" ).in( "session-types" ).curr( )->children( ).size( ); ++ i ) {

		Session
		s( cfg.rp( ).in( "cfg" ).in( "session-types" ).in( "type", i ).curr( )->attrs( ).value( "name" ).toString( ) );

		for( int j = 0; j < cfg.rp( ).in( "cfg" ).in( "session-types" ).in( "type", i ).curr( )->children( ).size( ); ++ j ) {

			QString
			t = cfg.rp( ).in( "cfg" ).in( "session-types" ).in( "type", i ).in( "measurement", j ).in( "type" ).curr( )->data( );

			s.addMeasurement(
				t == "VDN"
					? VDN
					: t == "VSN"
						? VSN
						: t == "VDT"
							? VDT
							: t == "VST"
								? VST
								: t == "VDA"
									? VDA
									: t == "VSA"
										? VSA
										: t == "VDC"
											? VDC
											: t == "VSC"
												? VSC
												: NO );
		}

		sessions.push_back( s );
	}

	currSession = cfg.rp( ).in( "cfg" ).in( "last-session-type" ).curr( )->attrs( ).value( "id" ).toInt( ) - 1;

	ui->comboBoxSessionType->clear( );

	for( int i = 0; i < sessions.size( ); ++ i ) {

		ui->comboBoxSessionType->addItem( sessions[ i ].name );
	}

	ui->comboBoxSessionType->setCurrentIndex( currSession );
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

void MainWindow::slotAddNewExaminator( ) {

	ui->lineEditExaminatorVName->setText( "" );

	ui->lineEditExaminatorNName->setText( "" );

	ui->lineEditExaminatorID->setText( "" );

	showWidget( ADDNEWEXAMINATOR );
}

void MainWindow::slotDoVisusTest( ) {

	ui->stackedWidget->setCurrentIndex( VISUSEXAMINATION );

	ui->pushButtonODVisus->setFocus( );
}

void MainWindow::slotExaminatorVNameChanged( int p_id ) {

	ui->comboBoxExaminatorNName->setCurrentIndex( vnames2nnames[ p_id ] );
	ui->comboBoxExaminatorID->setCurrentIndex( vnames2id[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );
}

void MainWindow::slotExaminatorNNameChanged( int p_id ) {

	ui->comboBoxExaminatorVName->setCurrentIndex( nnames2vnames[ p_id ] );
	ui->comboBoxExaminatorID->setCurrentIndex( nnames2id[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );
}

void MainWindow::slotExaminatorIDChanged( int p_id ) {

	ui->comboBoxExaminatorNName->setCurrentIndex( id2nnames[ p_id ] );
	ui->comboBoxExaminatorVName->setCurrentIndex( id2vnames[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );
}

void MainWindow::slotFinishSetup( ) {

	showWidget( lastWidget );
}

void MainWindow::slotCancelSession( ) {

	//saveSession( );

	showWidget( STARTSESSION );
}

void MainWindow::slotFinishSession( ) {

	saveSession( );

	ui->comboBoxExaminatorID->currentIndex( );

	cfg.rp( ).in( "cfg" ).in( "last-session-examinator-id" ).att( "id", currExaminatorID );

	cfg.save( "cfg.xml" );

	showWidget( TABLEVIEW );

	ui->tableWidgetExamination->show( );
}

void MainWindow::saveSession( ) {

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
	filename( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) + "/Session-" + currSIC + "-" + dt.join( "-" ) + "-Acuity_LandoltC.tsv" );

	tableSession.save( filename );
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
		msl = modeStrings[ currMeasurementType ].split( "_PLACEHOLDER_" );

		QString
		pth = cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( );

		QFile
		fileResult( pth + "/" + currSIC + "-" + dt + "-Acuity_LandoltC-" + msl[ 0 ] + "-" + msl[ 1 ] + "-Result.tsv" );

		if( fileResult.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

			QTextStream
			ts( &fileResult );

			ts <<
				"SIC" << "\t" << "DATE" << "\t" << "TIME" << "\t" << "RESULT" << "\t" <<
				"RESULT_UNIT" << "\t" << "OCULUS" << "\t" << "DETAILS" << "\t" << "TEST_NAME" <<
				"\t" << "DISTANCE" << "\t" << "DISTANCE_UNIT" << "\t" << "N_TRIALS" << "\t" << "EID" << "\t" << "COMMENT" << "\n" <<
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
				currExaminatorID << "\t" <<
				"";

			fileResult.close( );
		}

		if( 12 < row.size( ) ) {

			QFile
			fileData( pth + "/" + currSIC + "-" + dt + "-Acuity_LandoltC-" + msl[ 0 ] + "-" + msl[ 1 ] + "-Data.tsv" );

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

		switch ( currMeasurementType ) {

			case VDN : {

				//ui->pushButtonODVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VDN ].setValue( val );

				break;
			}

			case VSN : {

				//ui->pushButtonOSVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VSN ].setValue( val );

				break;
			}

			case VDT : {

				//ui->pushButtonODVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODTFVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VDT ].setValue( val );

				break;
			}

			case VST : {

				//ui->pushButtonOSVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSTFVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VST ].setValue( val );

				break;
			}

			case VDA : {

				//ui->pushButtonODVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODAVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VDA ].setValue( val );

				break;
			}

			case VSA : {

				//ui->pushButtonOSVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSAVal->setText( QString( "VA = %1  logMAR = %2" ).arg( val ).arg( -log10( val ) ) );
				measurements[ VSA ].setValue( val );

				break;
			}

			default:

				break;
		}

		ui->statusBar->clearMessage( );

		if( measurements[ VDN ].measured && measurements[ VDT ].measured && !measurements[ VDA ].measured && measurements[ VDN ].value > measurements[ VDT ].value ) {

			//ui->pushButtonODVisusAperture->setEnabled( true );
			ui->labelODAVal->setStyleSheet( "QLabel { color : red; }" );
			ui->labelODAVal->setText( "noch kein kein Wert gemessen" );

			ui->statusBar->showMessage( "Die Messung des rechten Auges (OD) ohne Messbrille ist besser als die mit Messbrille.\nBitte messen Sie nun mit Messblende!" );
		}

		if( measurements[ VSN ].measured && measurements[ VST ].measured && !measurements[ VSA ].measured && measurements[ VSN ].value > measurements[ VST ].value ) {

			ui->labelOSAVal->setStyleSheet( "QLabel { color : red; }" );
			ui->labelOSAVal->setText( "noch kein kein Wert gemessen" );

			ui->statusBar->showMessage( "Die Messung des linken Auges (OS) ohne Messbrille ist besser als die mit Messbrille.\nBitte messen Sie nun mit Messblende!" );
		}
	}
}

void MainWindow::slotNewExaminatorOK( ) {

	bool
	ok  = true;

	if( ui->lineEditExaminatorVName->text( ).isEmpty( ) ) {

		QMessageBox
		mb;

		mb.setText( "Geben Sie bitte Ihren Vornamen ein! Dieser kann auch gerne ein Spitzname." );

		mb.exec( );

		ok = false;
	}

	if( ui->lineEditExaminatorNName->text( ).isEmpty( ) ) {

		QMessageBox
		mb;

		mb.setText( "Geben Sie bitte Ihren Nachnamen ein! Dieser kann auch gerne ein Spitzname." );

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

		cfg.rp( ).in( "cfg" ).in( "examinators" ).addTag( "examinator" ).att( "id", ui->lineEditExaminatorID->text( ) ).att( "vname", ui->lineEditExaminatorVName->text( ) ).att( "nname", ui->lineEditExaminatorNName->text( ) );

		cfg.save( "cfg.xml" );

		updateExaminatorsView( );

		ui->pushButtonStartSession->setFocus( );

		showWidget( LOBBY );
	}

/*
	if( ok ) {

		currExaminatorID = ui->lineEditExaminatorID->text( ).toInt( );

		QString
		currExaminatorName = ui->lineEditExaminatorName->text( );

		currExaminatorName = ui->lineEditExaminatorName->text( );

		int
		id = tableExaminators.whereInColumnIs( currExaminatorID );

		if( 0 <= id ) {

			QMessageBox
			mb;

			mb.setText( "Diese Untersucher ID wurde bereits an " + tableExaminators.get( id, 1 ) + " vergeben. Sind Sie " + tableExaminators.get( id, 1 ) + ", dann klicken Sie auf abbrechen. Andernfalls kontaktieren Sie bitte den Studienleiter, um die Eindeutigkeit der Untersucher-IDs zu gewährleisten!" );

			mb.exec( );
		}

		else {

			tableExaminators.addRow( QStringList( { currExaminatorID, currExaminatorName } ) );

			while( ! tableExaminators.save( "examinators.tsv", true ) ) {

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
*/
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

	//updateExaminatorsView( );
}

void MainWindow::slotShowProperties( ) {

	ui->lineEditFlashPlayer->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ) );
	ui->lineEditFractSWF->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) );
	ui->lineEditDataDir->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) );

	lastWidget = SETUP;

	ui->spinBoxView->setValue( SETUP );
}

void MainWindow::slotStartSession( ) {

	tableSession.clearContent( );

	updateExaminationView( );

	for( int i = 0; i < 6; ++ i ) {

		measurements[ i ].clear( );
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

void MainWindow::slotStartFileDialogForFlashPlayer( ) {

	QString
	flashPlayerEXE = QFileDialog::getOpenFileName( this, tr( "Flash Player Projector" ), ".", tr( "Exe Datei (*.exe)" ) );

	if( flashPlayerEXE.isEmpty( ) ) {

		return;
	}

	ui->lineEditFlashPlayer->setText( flashPlayerEXE );

	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr()->setData( flashPlayerEXE );

	cfg.save( "cfg.xml" );
}

void MainWindow::slotStartFileDialogForFractSWF( ) {

	QString
	fractSWF = QFileDialog::getOpenFileName( this, tr( "FrACT*.swf" ), ".", tr( "SWF Datei (*.swf)" ) );

	if( fractSWF.isEmpty( ) ) {

		return;
	}

	ui->lineEditFractSWF->setText( fractSWF );

	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr()->setData( fractSWF );

	cfg.save( "cfg.xml" );

}

void MainWindow::slotStartFileDialogForDataDir( ) {

	QString
	dataDir = QFileDialog::getExistingDirectory( this, tr( "Daten Verzeichnis" ), tr( "." ), QFileDialog::ShowDirsOnly );

	if( dataDir.isEmpty( ) ) {

		return;
	}

	ui->lineEditDataDir->setText( dataDir );

	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr()->setData( dataDir );

	cfg.save( "cfg.xml" );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOD( ) {

	currMeasurementType = VDN;

	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOS( )
{
	currMeasurementType = VSN;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCODWithTrialFrame( )
{
	currMeasurementType = VDT;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOSWithTrialFrame( )
{
	currMeasurementType = VST;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCODWithHoleAperture( ) {

	currMeasurementType = VDA;
	startFrACT( );
}

void MainWindow::slotStartFrACTAcuity_LandoltCOSWithHoleAperture( )
{
	currMeasurementType = VSA;
	startFrACT( );
}
