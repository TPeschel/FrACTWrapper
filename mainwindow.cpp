#include <iostream>
#include <QThread>
#include "mainwindow.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

QTableWidgetItem
* createTableItemAlignCenter( QString const & p_value, bool p_readonly = true ) {

	QTableWidgetItem* tmp = new QTableWidgetItem( p_value );
	tmp->setTextAlignment( Qt::AlignCenter );

	if( p_readonly ) {

		tmp->setFlags( tmp->flags( ) & ~Qt::ItemIsEditable );
	}
	else {

		tmp->setFlags( tmp->flags( ) | Qt::ItemIsEditable );
	}

	return tmp;
}

MainWindow::MainWindow( QWidget * parent ) :
QMainWindow( parent ),
ui( new Ui::MainWindow ) {

	fractRunning = false;

	ui->setupUi( this );

	currWidget = SWID_LOBBY;

	proc = nullptr;

	tableSession.newHeader(
		QStringList( ) <<
		"SIC" << "DATE" << "TIME" << "EID" << "RESULT" <<
		"RESULT_UNIT" << "OCULUS" << "OPTICAL_AID" << "TEST_NAME" <<
		"DISTANCE" << "DISTANCE_UNIT" << "N_TRIALS" << "MID" << "COMMENT" );

//    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
//    this->setWindowFlags( this->windowFlags( ) & ~( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::Window | Qt::MSWindowsFixedSizeDialogHint ) );
//    this->setFixedSize(this->width(), this->height());

	connect( this,                            SIGNAL( signalNoConfigFound( ) ),     this, SLOT( slotShowProperties( ) ) );

	connect( ui->actionEinstellungen,         SIGNAL( triggered( ) ),               this, SLOT( slotShowProperties( ) ) );
	connect( ui->actionHilfe,                 SIGNAL( triggered( ) ),               this, SLOT( slotShowHelp( ) ) );

	connect( ui->lineEditSIC,                 SIGNAL( textChanged( QString ) ),     this, SLOT( slotScanSic( QString ) ) );

	connect( ui->comboBoxExaminatorVName,     SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorVNameChanged( int ) ) );
	connect( ui->comboBoxExaminatorNName,     SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorNNameChanged( int ) ) );
	connect( ui->comboBoxExaminatorID,        SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotExaminatorIDChanged( int ) ) );

	connect( ui->pushButtonAddNewAccount,     SIGNAL( released( ) ), this, SLOT( slotAddNewExaminator( ) ) );
	connect( ui->pushButtonNewAccountOK,      SIGNAL( released( ) ), this, SLOT( slotNewExaminatorOK( ) ) );
	connect( ui->pushButtonNewAccountAbort,   SIGNAL( released( ) ), this, SLOT( slotNewExaminatorCancel( ) ) );
	connect( ui->pushButtonStartSession,      SIGNAL( released( ) ), this, SLOT( slotStartSession( ) ) );
	connect( ui->pushButtonODNO,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTOD( ) ) );
	connect( ui->pushButtonOSNO,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTOS( ) ) );
	connect( ui->pushButtonODTF,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTODTF( ) ) );
	connect( ui->pushButtonOSTF,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTOSTF( ) ) );
	connect( ui->pushButtonODHA,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTODHA( ) ) );
	connect( ui->pushButtonOSHA,              SIGNAL( released( ) ), this, SLOT( slotStartFrACTOSHA( ) ) );

	connect( ui->toolButtonFlashPlayer,       SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForFlashPlayer( ) ) );
	connect( ui->toolButtonFractSWF,          SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForFractSWF( ) ) );
	connect( ui->toolButtonDataDir,           SIGNAL( released( ) ), this, SLOT( slotStartFileDialogForDataDir( ) ) );

	connect( ui->pushButtonSetupBack,         SIGNAL( released( ) ), this, SLOT( slotFinishSetup( ) ) );
	connect( ui->pushButtonBackFromHelp,      SIGNAL( released( ) ), this, SLOT( slotFinishHelp( ) ) );
	connect( ui->pushButtonFinishSession,     SIGNAL( released( ) ), this, SLOT( slotFinishSession( ) ) );
	connect( ui->pushButtonCancelSession,     SIGNAL( released( ) ), this, SLOT( slotCancelSession( ) ) );

	connect( ui->pushButtonExitFromTabView,       SIGNAL( released( ) ), this, SLOT( close( ) ) );
	connect( ui->pushButtonNewProbandFromTabView, SIGNAL( released( ) ), this, SLOT( slotStartSession( ) ) );

	connect( ui->action_Beenden,              SIGNAL( triggered( ) ), this, SLOT( slotClose( ) ) );
	connect( ui->action_Feierabend,           SIGNAL( triggered( ) ), this, SLOT( slotClose( ) ) );

	connect( ui->tableWidgetResult, SIGNAL( itemChanged( QTableWidgetItem * ) ), this, SLOT( slotChangeComment( QTableWidgetItem * ) ) );

	ui->tableWidgetExamination->horizontalHeader( )->setSectionResizeMode( QHeaderView::Stretch );
	ui->tableWidgetResult->horizontalHeader( )->setSectionResizeMode( QHeaderView::Stretch );

	//switchWidget( SWID_LOBBY );

	loadConfig( );

	updatePathsConfigView( );

	updateExaminatorsView( );

	updateExaminationView( );
}

MainWindow::~MainWindow( ) {

	delete ui;
}

void
MainWindow::loadConfig( ) {

	if( ! cfg.load( "cfg.xml" ) ) {

		cfg.clear( );

		QString
		cdir  = QDir::currentPath( ),
 		flash = QDir( cdir ).filePath( "flash/flashplayer_29_sa.exe" ),
//		flash = "https://fpdownload.macromedia.com/pub/flashplayer/updaters/29/flashplayer_29_sa.exe",
		fract = QDir( cdir ).filePath( "fract/FrACT3.10.0d.swf" ),
//		fract = "http://www.michaelbach.de/fract/versions/FrACT3.10.0d.swf",
		data  = QDir( cdir ).filePath( "data" );

		cfg.rp( )
		.tag( "cfg" ).att( "date", QDate::currentDate( ).toString( ) ).att( "time", QTime::currentTime( ).toString( ) )
			.tag( "paths" )
				.tag( "flash" ).txt( flash ).gat( )
				.tag( "fract" ).txt( fract ).gat( )
				.tag( "data" ).txt( data );

		cfg.rp( )
			.tag( "list-examinators" ).att( "size", "8" )
				.tag( "item" ).att( "i", "0" )
					.tag( "id" ).txt( "xxxx" ).gat( )
					.tag( "vname" ).txt( "Thomas" ).gat( )
					.tag( "nname" ).txt( "Peschel" ).gat( 3 )
			.tag( "last-examinator-id" ).txt( "xxxx" ).gat( )
			.tag( "last-examination-id" ).txt( "0" );

		cfg.save( "cfg.xml" );

		emit signalNoConfigFound( );
	}

	currExaminatorID = cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).curr( )->data( ).toLong( );
	examinationId = cfg.rp( ).in( "cfg" ).in( "last-examination-id" ).curr( )->data( ).toLong( );
}

void
MainWindow::prepareSession( ) {

//	currExamination = 0;

//	updateSessionSequence( );

	tableSession.clearContent( );

	updateExaminationView( );

	updateTableWidgetResults( );

	for( int i = 0; i < 6; ++ i ) {

		measurements[ i ].clear( );
	}

	ui->lineEditSIC->setEnabled( true );
	ui->lineEditSIC->clear( );
	ui->lineEditSIC->setFocus( );

	ui->labelODVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODVal->setText( "noch kein Wert gemesen" );
	ui->pushButtonODNO->setEnabled( true );

	ui->labelOSVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSVal->setText( "noch kein Wert gemesen" );
	ui->pushButtonOSNO->setEnabled( true );

	ui->labelODTFVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODTFVal->setText( "noch kein Wert gemesen" );
	ui->pushButtonODTF->setEnabled( true );

	ui->labelOSTFVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSTFVal->setText( "noch kein Wert gemesen" );
	ui->pushButtonOSTF->setEnabled( true );

	ui->labelODAVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelODAVal->setText( "keine Messung erforderlich" );
	ui->pushButtonODHA->setEnabled( true );
	ui->labelODAVal->setStyleSheet( "color : black" );

	ui->labelOSAVal->setStyleSheet( "QLabel { color : black }" );
	ui->labelOSAVal->setText( "keine Messung erforderlich" );
	ui->pushButtonOSHA->setEnabled( true );
	ui->labelOSAVal->setStyleSheet( "color : black" );

//	ui->stackedWidget->setCurrentIndex( STARTSESSION );

//	ui->stackedWidget->setCurrentIndex( VISUSEXAMINATION );

//	ui->pushButtonSessionSequence->setFocus( );
}

void
MainWindow::saveSession( ) {

	updateExaminationView( );

	QDate
	d = QDate::currentDate( );

	QTime
	t = QTime::currentTime( );

	QStringList
	dt;

	dt << d.toString( "yyyyMMdd" ) << t.toString( "HHmmss" );

	QString
	filename( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) + "/" + currSIC + "-" + dt.join( "-" ) + "-SESSION" + ".tsv" );
//	filename( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) + "/Session-" + currSIC + "-" + dt.join( "-" ) + ".tsv" );

	tableSession.save( filename );
}

void
MainWindow::switchWidget( StackedWidgetID p_newId ) {

	widgetHistory.push_back( currWidget );

	currWidget = p_newId;

	ui->stackedWidget->setCurrentIndex( currWidget );
}

void
MainWindow::switchWidgetBack( ) {

	StackedWidgetID
	swid =  widgetHistory[  widgetHistory.size( ) - 1 ];

	widgetHistory.pop_back( );

	currWidget = swid;

	ui->stackedWidget->setCurrentIndex( swid );
}

void
MainWindow::startFrACT( ) {

	QApplication::clipboard( )->clear( QClipboard::Clipboard );

	if( proc )

		delete proc;

	proc = new QProcess( );

	QObject::connect( proc, SIGNAL( finished( int ) ), this, SLOT( slotFrACTFinished( int ) ) );

	proc->setWorkingDirectory ( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) );

	proc->start(
		cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ),
		QStringList( ) << cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) ); //windows

	fractRunning = true;
}

void
MainWindow::updateExaminatorsView( ) {

 	vnames.clear( );
	nnames.clear( );
	ids.clear( );

	int
	sz = cfg.rp( ).in( "cfg" ).in( "list-examinators" ).curr( )->children( ).size( );
	//sz = cfg.rp( ).in( "cfg" ).in( "list-examinators" ).curr( )->attrs( ).value( "size" ).toInt( );

	for( int i = 0; i < sz; ++ i ) {

		ids    << cfg.rp( ).in( "cfg" ).in( "list-examinators" ).in( "item", i ).in( "id" ).curr( )->data( ),
		vnames << cfg.rp( ).in( "cfg" ).in( "list-examinators" ).in( "item", i ).in( "vname" ).curr( )->data( ),
		nnames << cfg.rp( ).in( "cfg" ).in( "list-examinators" ).in( "item", i ).in( "nname" ).curr( )->data( );
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

	ui->comboBoxExaminatorID->blockSignals( true );
	ui->comboBoxExaminatorVName->blockSignals( true );
	ui->comboBoxExaminatorNName->blockSignals( true );

	ui->comboBoxExaminatorVName->clear( );
	ui->comboBoxExaminatorVName->addItems( slVNames );

	ui->comboBoxExaminatorNName->clear( );
	ui->comboBoxExaminatorNName->addItems( slNNames );

	ui->comboBoxExaminatorID->clear( );
	ui->comboBoxExaminatorID->addItems( slIds );

	currExaminatorID = cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).curr( )->data( );

	for( int i = 0; i < ids.size( ); ++ i ) {

		if( ids[ ordId[ i ] ] == currExaminatorID ) {

			ui->comboBoxExaminatorID->setCurrentIndex( i );
			ui->comboBoxExaminatorVName->setCurrentIndex( id2vnames[ i ] );
			ui->comboBoxExaminatorNName->setCurrentIndex( id2nnames[ i ] );
			ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + currExaminatorID + "]" );

			break;
		}
	}

	ui->comboBoxExaminatorID->blockSignals( false);
	ui->comboBoxExaminatorVName->blockSignals( false );
	ui->comboBoxExaminatorNName->blockSignals( false );
}

void
MainWindow::updatePathsConfigView( ) {

	ui->lineEditFlashPlayer->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ) );
	ui->lineEditFractSWF->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) );
	ui->lineEditDataDir->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) );
}

void
MainWindow::updateExaminationView( ) {

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

void
MainWindow::updateTableWidgetResults( ) {

	Row
	hdr;

	for( int i = 0; i < tableSession.cols( ); ++ i ) {

		hdr << tableSession.header( ).at( i );
	}

	ui->tableWidgetResult->clear( );
	ui->tableWidgetResult->setRowCount( tableSession.rows( ) );
	ui->tableWidgetResult->setColumnCount( tableSession.cols( ) );
	ui->tableWidgetResult->setHorizontalHeaderLabels( hdr );
	ui->tableWidgetResult->horizontalHeader( )->setSectionResizeMode( QHeaderView::ResizeToContents );

	for( int r = 0; r < tableSession.rows( ); ++ r ) {

		for( int c = 0; c < tableSession.cols( ); ++ c ) {

			QTableWidgetItem
			* item = createTableItemAlignCenter( tableSession.get( r, c ), c + 1 != tableSession.cols( ) );

			ui->tableWidgetResult->setItem( r, c, item );
		}
	}
}

void
MainWindow::slotAddNewExaminator( ) {

	ui->lineEditExaminatorVName->setText( "" );

	ui->lineEditExaminatorNName->setText( "" );

	ui->lineEditExaminatorID->setText( "" );

	switchWidget( SWID_ADDNEWEXAMINATOR );
}

void
MainWindow::slotChangeComment( QTableWidgetItem * p_item ) {

	tableSession.set( p_item->row( ), p_item->column( ), p_item->text( ) );
}

void
MainWindow::slotCancelSession( ) {

	if( messageBox( ) ) {

		return;
	}

	ui->lineEditSIC->clear( );
	ui->lineEditSIC->setEnabled( true );
	ui->lineEditSIC->setFocus( );

	switchWidget( SWID_STARTSESSION );
}

void
MainWindow::slotExaminatorVNameChanged( int p_id ) {

	ui->comboBoxExaminatorNName->setCurrentIndex( vnames2nnames[ p_id ] );

	ui->comboBoxExaminatorID->setCurrentIndex( vnames2id[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );

//	cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).txt( currExaminatorID );
}

void
MainWindow::slotExaminatorNNameChanged( int p_id ) {

	ui->comboBoxExaminatorVName->setCurrentIndex( nnames2vnames[ p_id ] );

	ui->comboBoxExaminatorID->setCurrentIndex( nnames2id[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );

//	cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).txt( currExaminatorID );
}

void
MainWindow::slotExaminatorIDChanged( int p_id ) {

	ui->comboBoxExaminatorNName->setCurrentIndex( id2nnames[ p_id ] );

	ui->comboBoxExaminatorVName->setCurrentIndex( id2vnames[ p_id ] );

	ui->labelHelloMsg->setText( "Hallo\n" + ui->comboBoxExaminatorVName->currentText( ) + " " + ui->comboBoxExaminatorNName->currentText( ) + " [" + ui->comboBoxExaminatorID->currentText( ) + "]" );

	currExaminatorID = ui->comboBoxExaminatorID->currentText( );

	//	cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).txt( currExaminatorID );
}

void
MainWindow::slotFinishHelp( ) {

	switchWidgetBack( );
}

void
MainWindow::slotFinishSetup( ) {

	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->setData( ui->lineEditFlashPlayer->text( ) );
	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->setData( ui->lineEditFractSWF->text( ) );
	cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr()->setData( ui->lineEditDataDir->text( ) );

	cfg.save( "cfg.xml" );

	switchWidgetBack( );
}

bool
MainWindow::messageBox( ) {

	if( fractRunning ) {

		QMessageBox
		mb;

		mb.setText( "Das Programm FrACT ist noch geöffnet. Das heisst, es läuft noch eine Messung. Beenden Sie diese bitte, bevor Sie eine neue Messung starten!" );

		mb.setInformativeText( "Wechseln Sie dazu bitte via Alt-TAB in den Flashplayer, [Weisses 'F' auf rotem Untergrund.] oder bewegen Sie die Maus rechts aus dem Bildschirm und druecken Sie dann die linke Maustaste! Nun kann das Programm 'FrACT' wie gewohnt mit 'x' beendet werden." );

		mb.exec( );

		return true;
	}

	return false;
}

void
MainWindow::slotFinishSession( ) {

	if( messageBox( ) ) {

		return;
	}

	saveSession( );

	cfg.rp( ).in( "cfg" ).in( "last-examinator-id" ).txt( currExaminatorID );
	cfg.rp( ).in( "cfg" ).in( "last-examination-id" ).txt( QString( "%1" ).arg( examinationId ) );

	cfg.save( "cfg.xml" );

	switchWidget( SWID_TABLEVIEW );

//	ui->tableWidgetExamination->show( );
}

void
MainWindow::slotFrACTFinished( int p_exitCode ) {

	ui->statusBar->clearMessage( );

	std::cout << "exit code: " << p_exitCode << std::endl;

	if( p_exitCode != 0 )

		return;

	fractRunning = false;

	QClipboard
	*cb = QApplication::clipboard( );

	QString
	txt = cb->text( QClipboard::Clipboard );

	std::cout << "txt: " << txt.toStdString( ) << std::endl;

	Row
	row = txt.split( QRegExp( "[\t\n\r]" ), QString::SkipEmptyParts );

	if( 0 < row.size( ) && row.at( 0 )[ 0 ] == '2' ) {

		++examinationId;

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
		fileResult( pth + "/" + currSIC + "-" + dt + "-" + row.at( 4 ) + "-" + fileNameStrings[ currMeasurementType ] + QString( "-%1" ).arg( examinationId ) + "-RESULT.tsv" );

		if( fileResult.open( QIODevice::WriteOnly | QIODevice::Text ) ) {

			QTextStream
			ts( &fileResult );

			ts <<
				"SIC" << "\t" << "DATE" << "\t" << "TIME" << "\t" << "EID" << "\t" << "RESULT" << "\t" <<
				"RESULT_UNIT" << "\t" << "OCULUS" << "\t" << "OPTICAL_AID" << "\t" << "TEST_NAME" <<
				"\t" << "DISTANCE" << "\t" << "DISTANCE_UNIT" << "\t" << "N_TRIALS" << "\t" << "MID" <<  "\t" << "COMMENT" << "\n" <<
				currSIC << "\t" <<
				row.at( 0 ) << "\t" <<
				row.at( 1 ) << "\t" <<
				currExaminatorID << "\t" <<
				row.at( 2 ) << "\t" <<
				row.at( 3 ) << "\t" <<
				msl[ 0 ] << "\t" <<
				msl[ 1 ] << "\t" <<
				row.at( 4 ) << "\t" <<
				row.at( 5 ) << "\t" <<
				row.at( 6 ) << "\t" <<
				row.at( 7 ) << "\t" <<
				QString( "%1" ).arg( examinationId ) << "\t" << "";

			fileResult.close( );
		}

		if( 12 < row.size( ) ) {

			QFile
			fileData( pth + "/" + currSIC + "-" + dt +"-" + row.at( 4 ) + "-" + fileNameStrings[ currMeasurementType ] + QString( "-%1" ).arg( examinationId ) + "-DATA.tsv" );

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
			currSIC <<			//sic
			row.at( 0 ) <<		//date
			row.at( 1 ) <<		//time
			currExaminatorID << //examinator id
			row.at( 2 ) <<		//result
			row.at( 3 ) <<		//res_unit
			msl[ 0 ] <<			//oculus
			msl[ 1 ] <<			//opt aid
			row.at( 4 ) <<		//test type
			row.at( 5 ) <<		//distance
			row.at( 6 ) <<		//distance unit
			row.at( 7 ) <<		//n_trials
			QString( "%1" ).arg( examinationId ) << //measurement id
			"" );									//comment

		double
		val = row.at( 2 ).toDouble( );

		switch ( currMeasurementType ) {

			case MT_OD_NO : {

				//ui->pushButtonODVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonODNO->setEnabled( false );
				//ui->lineEditCommentOD->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			case MT_OS_NO : {

				//ui->pushButtonOSVisus->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonOSNO->setEnabled( false );
				//ui->lineEditCommentOS->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			case MT_OD_TF : {

				//ui->pushButtonODVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODTFVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonODTF->setEnabled( false );
				//ui->lineEditCommentODTF->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			case MT_OS_TF : {

				//ui->pushButtonOSVisusTrialFrame->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSTFVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSTFVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonOSTF->setEnabled( false );
				//ui->lineEditCommentOSTF->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			case MT_OD_HA : {

				//ui->pushButtonODVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelODAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelODAVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonODHA->setEnabled( false );
				//ui->lineEditCommentODA->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			case MT_OS_HA : {

				//ui->pushButtonOSVisusAperture->setStyleSheet( "QPushButton { color : green; }" );
				ui->labelOSAVal->setStyleSheet( "QLabel { color : green; }" );
				ui->labelOSAVal->setText( row.at( 2 ) + " " + row.at( 3 ) );
				//ui->pushButtonOSHA->setEnabled( false );
				//ui->lineEditCommentOSA->setEnabled( true );
				measurements[ currMeasurementType ].setValue( val );

				break;
			}

			default:

				break;
		}

		updateTableWidgetResults( );

		if( measurements[ MT_OD_NO ].measured && measurements[ MT_OD_TF ].measured && ! measurements[ MT_OD_HA ].measured ) {

			if( measurements[ MT_OD_NO ].value > measurements[ MT_OD_TF ].value ) {

				ui->statusBar->showMessage( "Die Messung ohne Messbrille war besser als die mit. Führen Sie nun bitte eine Messung mit Lochblende durch!" );
				ui->labelODAVal->setText( "Messung erforderlich" );
				ui->labelODAVal->setStyleSheet( "color : red" );
			}
			else {

				ui->statusBar->clearMessage( );
				ui->labelODAVal->setText( "keine Messung erforderlich" );
				ui->labelODAVal->setStyleSheet( "color : black" );
			}
		}

		if( measurements[ MT_OS_NO ].measured && measurements[ MT_OS_TF ].measured && ! measurements[ MT_OS_HA ].measured ) {

			if( measurements[ MT_OS_NO ].value > measurements[ MT_OS_TF ].value ) {

				ui->statusBar->showMessage( "Die Messung ohne Messbrille war besser als die mit. Führen Sie nun bitte eine Messung mit Lochblende durch!" );
				ui->labelOSAVal->setText( "Messung erforderlich" );
				ui->labelOSAVal->setStyleSheet( "color : red" );
			}
			else {

				ui->statusBar->clearMessage( );
				ui->labelOSAVal->setText( "keine Messung erforderlich" );
				ui->labelOSAVal->setStyleSheet( "color : black" );
			}
		}
	}
}

void
MainWindow::slotNewExaminatorOK( ) {

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

		currExaminatorID = ui->lineEditExaminatorID->text( );

		int
		sz = cfg.rp( )
			.in( "cfg" )
				.in( "list-examinators" ).curr( )->children( ).size( );

		cfg.rp( )
			.in( "cfg" )
				.in( "list-examinators" ).att( "size", QString( "%1" ).arg( sz + 1 ) )
					.tag( "item" ).att( "i", QString( "%1" ).arg( sz ) )
						.tag( "id" ).txt( currExaminatorID ).gat( )
						.tag( "vname" ).txt( ui->lineEditExaminatorVName->text( ) ).gat( )
						.tag( "nname" ).txt( ui->lineEditExaminatorNName->text( ) ).gat( 3 )
				.in( "last-examinator-id" ).txt( currExaminatorID );

		cfg.save( "cfg.xml" );

		updateExaminatorsView( );

		ui->pushButtonStartSession->setFocus( );

		switchWidget( SWID_LOBBY );
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

void
MainWindow::slotNewExaminatorCancel( ) {

	ui->stackedWidget->setCurrentIndex( SWID_LOBBY );
}

void
MainWindow::slotScanSic( QString const & p_sic ) {

	QRegularExpression re( "^LI\\d{7}[\\d{1}xX]", QRegularExpression::CaseInsensitiveOption );
	QRegularExpressionMatch m = re.match( p_sic );

	if( m.hasMatch( ) ) {

		currSIC = m.captured( ).toUpper( );

		ui->lineEditSIC->setEnabled( false );

		this->setWindowTitle( "FrACT Wrapper - aktuelle Untersuchung - UID: " + currExaminatorID + " - SIC: " + currSIC );

		prepareSession( );

		switchWidget( SWID_EXAMINATION );
	}
}

void
MainWindow::slotShowLobby( ) {

	switchWidget( SWID_LOBBY );
}

void
MainWindow::slotShowProperties( ) {

	ui->lineEditFlashPlayer->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "flash" ).curr( )->data( ) );
	ui->lineEditFractSWF->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "fract" ).curr( )->data( ) );
	ui->lineEditDataDir->setText( cfg.rp( ).in( "cfg" ).in( "paths" ).in( "data" ).curr( )->data( ) );

 	switchWidget( SWID_SETUP );
}

void
MainWindow::slotShowHelp( ) {

	switchWidget( SWID_HELP );
}

void
MainWindow::slotStartSession( ) {

	prepareSession( );

	switchWidget( SWID_STARTSESSION );
}

void
MainWindow::slotStartFileDialogForFlashPlayer( ) {

	QString
	flashPlayerEXE = QFileDialog::getOpenFileName( this, tr( "Flash Player Projector" ), ".", tr( "Exe Datei (*.exe)" ) );

	if( flashPlayerEXE.isEmpty( ) ) {

		return;
	}

	ui->lineEditFlashPlayer->setText( flashPlayerEXE );
}

void
MainWindow::slotStartFileDialogForFractSWF( ) {

	QString
	fractSWF = QFileDialog::getOpenFileName( this, tr( "FrACT*.swf" ), ".", tr( "SWF Datei (*.swf)" ) );

	if( fractSWF.isEmpty( ) ) {

		return;
	}

	ui->lineEditFractSWF->setText( fractSWF );
}

void
MainWindow::slotStartFileDialogForDataDir( ) {

	QString
	dataDir = QFileDialog::getExistingDirectory( this, tr( "Daten Verzeichnis" ), tr( "." ), QFileDialog::ShowDirsOnly );

	if( dataDir.isEmpty( ) ) {

		return;
	}

	ui->lineEditDataDir->setText( dataDir );
}

void
MainWindow::slotStartFrACTOD( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OD_NO;

	startFrACT( );
}

void
MainWindow::slotStartFrACTOS( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OS_NO;

	startFrACT( );
}

void
MainWindow::slotStartFrACTODTF( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OD_TF;

	startFrACT( );
}

void
MainWindow::slotStartFrACTOSTF( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OS_TF;

	startFrACT( );
}

void
MainWindow::slotStartFrACTODHA( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OD_HA;

	startFrACT( );
}

void
MainWindow::slotStartFrACTOSHA( ) {

	if( messageBox( ) )

		return;

	currMeasurementType = MT_OS_HA;

	startFrACT( );
}

void
MainWindow::slotClose( ) {

	if( messageBox( ) ) {

		return;
	}

	close( );
}

void
MainWindow::showEvent( QShowEvent *ev ) {

	 QMainWindow::showEvent( ev );

	 showEventHelper( );
}

void
MainWindow::showEventHelper( ) {

	QDateTime
	dt = QDateTime::currentDateTime( );

	QDate
	ende( 2019, 3, 1 );

	int
	timeleft = dt.date().daysTo( ende );

	setWindowTitle( QString( "FractWrapper - Ihre Lizens zur freien Nutzung dieses Programmes endet in %1 Tagen. Weitere Informationen finden Sie in der Hilfe." ).arg( timeleft ) );

	setWindowState( this->windowState( ) | Qt::WindowMaximized );

	if( timeleft <= 0 ) {

		ui->tabWidgetHelp->setCurrentIndex( 2 );

		slotShowHelp( );

		QTimer::singleShot( 20000, this, SLOT( slotCloseImmediately( ) ) );

		QMessageBox
		mb;

		mb.setText( "Ihre Lizens ist abgelaufen. Bei Fragen wenden Sie sich bitte an: Peschel.Thomas@GMX.Net!" );

		mb.exec( );
	}
}

void
MainWindow::slotCloseImmediately( ) {

	exit( -1 );
}
