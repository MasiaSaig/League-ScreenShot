#include "mainwindow.h"

#ifndef QT_NO_SYSTEMTRAYICON
#include <QDialog>
#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) //QWidget *parent
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
{
        // ui->setupUi(this);

    // Gdi Plus initialization
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

    const QString iconPath = ":/icon.png";
    createActions();
    createTrayIcon(iconPath);

    // creating directory path browsing button
    infoLabel = new QLabel("League of Legends Screenshot directory path");
    pathDirectoryLabel = new QLabel("Directory:");
    pathDirectoryButton = new QPushButton();    // "Browse Directory Path"
    pathDirectoryButton->setIcon(pathDirectoryButton->style()->standardIcon(QStyle::SP_DirHomeIcon));
    // TODO: load path from file
    pathDirectoryEdit = new QLineEdit("H:/Riot Games/League of Legends/Screenshots");

    // connecting signals
    connect(pathDirectoryButton, SIGNAL(clicked()), SLOT(browse()));
    connect(pathDirectoryEdit, SIGNAL(editingFinished()), SLOT(onEditingFinished()));

    // creating layouts
    vLayout = new QVBoxLayout();
    vLayout->addWidget(infoLabel);

    hLayout = new QHBoxLayout();
    hLayout->addWidget(pathDirectoryLabel);
    hLayout->addWidget(pathDirectoryEdit);
    hLayout->addWidget(pathDirectoryButton);

    vLayout->addLayout(hLayout);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vLayout);


    trayIcon->show();
    setWindowTitle(tr("League Screen Show"));
    setWindowIcon(QIcon(iconPath));
    setFixedSize(400, 64);

    // fileMonitoringController.changeDirectory("H:/Riot Games/League of Legends/Screenshots");
    // fileMonitoringController.runDirectoryMonitoring();
    // m_dirMonitoringController = new DirectoryMonitoringController();    // (wchar_t*)"H:\\Riot Games\\League of Legends\\Screenshots\\"
    // m_dirMonitoringThread = new QThread();
    // m_dirMonitoringController->moveToThread(m_dirMonitoringThread);
    // connect( m_dirMonitoringController, &DirectoryMonitoringController::error, this, &MainWindow::errorString);
    // connect( m_dirMonitoringThread, &QThread::started, m_dirMonitoringController, &DirectoryMonitoringController::runDirectoryMonitoring);
    // connect( m_dirMonitoringController, &DirectoryMonitoringController::finished,  m_dirMonitoringThread, &QThread::quit);

    // m_dirMonitoringThread->start();

    QString temp = "H:/Riot Games/League of Legends/Screenshots/";
    m_dirPath = new DirectoryPath(temp);
    qDebug() << "dirPath addres: " << m_dirPath;
    qDebug() << "dirPath path: " << QString::fromWCharArray(m_dirPath->getDirPath());
    m_dirMonitoringController = new DirectoryMonitoringController(m_dirPath);
    qDebug() << "Starting threads.";
    m_dirMonitoringController->start();

}

MainWindow::~MainWindow()
{
    qDebug() << "Cleaning up MainWindow. Closing threads.";
    m_dirMonitoringController->quit();
    m_dirMonitoringController->wait();
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void MainWindow::errorString(QString err){
    qDebug() << err;
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("&Settings"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}
void MainWindow::createTrayIcon(const QString &iconPath)
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(iconPath));
}

void MainWindow::onEditingFinished()
{
    qDebug() << "Editing Finished, path: " << pathDirectoryEdit->text();
    if(pathDirectoryEdit->text().back() == '/'){
        // m_directory = pathDirectoryEdit->text();
        m_dirPath->setDirPath(pathDirectoryEdit->text());
    }else{
        // m_directory = pathDirectoryEdit->text() + '/';
        m_dirPath->setDirPath(pathDirectoryEdit->text() + '/');
    }
}

void MainWindow::browse()
{
    // QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath());
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                          "/home",
                                                          QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    // check if no directory was selected or directory selection was canceled
    if(directory.isEmpty()){
        return;
    }
    pathDirectoryEdit->setText(directory + '/');
    qDebug() << "Browse, setting path:" << directory;
    m_dirPath->setDirPath(directory + '/');
}


#endif
