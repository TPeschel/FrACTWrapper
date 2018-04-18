#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi( this );

//    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
//    this->setWindowFlags( this->windowFlags( ) & ~( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::Window | Qt::MSWindowsFixedSizeDialogHint ) );
//    this->setFixedSize(this->width(), this->height());

    connect( ui->actionEinstellungen, SIGNAL( triggered( ) ), this, SLOT( slotShowProperties( ) ) );
    connect( ui->comboBox, SIGNAL( activated( QString ) ), this, SLOT( slotExaminatorChanged( QString ) ) );

    ui->statusBar->showMessage( "Scannen Sie jetzt bitte die SIC des Porbanden!" );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotShowProperties( ) {

    ui->spinBox->setValue( 4 );
}

void MainWindow::slotExaminatorChanged( QString p_name ) {

    ui->label_22->setText( "Hallo " + p_name + "." );
    ui->pushButton_16->setText( "Klick mich," );
    ui->label_24->setText( "um eine neue Session zu starten!" );
}
