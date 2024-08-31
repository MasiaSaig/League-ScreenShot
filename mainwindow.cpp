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

#include "logfile.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    LogFile::instance() << "Initializing Main Window.\n";

    // preparing Directory Controller class
    m_dirPath = new DirectoryPath();
    if(!m_dirPath->loadDirPathFromFile()){
        // set default directory path
        m_dirPath->setDirPath( "C:/Riot Games/League of Legends/Screenshots/" );
    }

    // creating a new thread
    m_dirMonitoringController = new DirectoryMonitoringController(m_dirPath);

    // Gdi Plus initialization
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);


    // Creating UI
    const QString iconPath = ":/icon.png";
    createActions();
    createTrayIcon(iconPath);

    // creating directory path browsing button
    infoLabel = new QLabel("League of Legends Screenshot directory path");
    pathCorrectionLabel = new QLabel("(Incorrect)");
    pathDirectoryLabel = new QLabel("Directory:");
    pathDirectoryButton = new QPushButton();
    pathDirectoryButton->setIcon(pathDirectoryButton->style()->standardIcon(QStyle::SP_DirHomeIcon));
    pathDirectoryEdit = new QLineEdit(m_dirPath->getQStringDirPath());

    // connecting signals
    connect(pathDirectoryButton, SIGNAL(clicked()), SLOT(browse()));
    connect(pathDirectoryEdit, SIGNAL(editingFinished()), SLOT(onEditingFinished()));

    // creating layouts
    hLayoutTop = new QHBoxLayout();
    hLayoutTop->addWidget(infoLabel, 0);
    hLayoutTop->addWidget(pathCorrectionLabel, 0, Qt::AlignRight);

    hLayoutBottom = new QHBoxLayout();
    hLayoutBottom->addWidget(pathDirectoryLabel);
    hLayoutBottom->addWidget(pathDirectoryEdit);
    hLayoutBottom->addWidget(pathDirectoryButton);

    vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayoutTop);
    vLayout->addLayout(hLayoutBottom);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vLayout);

    // system tray icon
    trayIcon->show();
    setWindowTitle(tr("League Screen Show"));
    setWindowIcon(QIcon(iconPath));
    setFixedSize(400, 64);

    updateUI();


    // starting new thread, to monitor directory
    LogFile::instance() << "Starting Thread.\n";
    m_dirMonitoringController->start();
}

MainWindow::~MainWindow()
{
    // closing thread and cleaning up data
    LogFile::instance() << "Cleaning up MainWindow and closing threads.\n";
    m_dirMonitoringController->quit();
    // using terminate is discouraged :/
    // mabe dont use terminate, instead try to move a file to idk invoke an event to close the loop? :shrug:
    // m_dirMonitoringController->terminate();
    m_dirMonitoringController->wait();
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    delete m_dirMonitoringController;
    delete m_dirPath;

    delete quitAction; delete restoreAction; delete minimizeAction;
    delete pathDirectoryButton; delete pathDirectoryEdit; delete pathDirectoryLabel;
    delete infoLabel;
    delete pathCorrectionLabel;
    delete hLayoutBottom;
    delete vLayout;
}

void MainWindow::updateUI()
{
    QPalette textColorPalette;
    if(m_dirPath->isDirPathCorrect()){
        textColorPalette.setColor(QPalette::WindowText, Qt::green);
        pathCorrectionLabel->setText("(Correct)");
    }else{
        textColorPalette.setColor(QPalette::WindowText, Qt::red);
        pathCorrectionLabel->setText("(Incorrect)");
    }
    pathCorrectionLabel->setPalette(textColorPalette);
}

void MainWindow::createActions()
{
    LogFile::instance() << "Setting up actions.\n";
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("&Settings"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::createTrayIcon(const QString &iconPath)
{
    LogFile::instance() << "Creating system tray icon.\n";
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
    LogFile::instance() << "Editing Finished, path: " << pathDirectoryEdit->text() << '\n';
    QString path_temp = pathDirectoryEdit->text();
    if(path_temp.back() != '/'){
        path_temp += '/';
    }

    // if directory handler isnt correct, dont save it to file
    if(!m_dirPath->setDirPath(path_temp)){
        QMessageBox::information(this, "Error on path editing.", "Unable to set, given directory path. Check given path again, if the folder exists.");
    }else{
        m_dirPath->saveDirPathToFile(this);
    }
    updateUI();
}

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"),
        "/home",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    // check if no directory was selected or directory selection was canceled
    if(directory.isEmpty()){
        LogFile::instance() << "Directory browsing canceled.\n";
        return;
    }
    directory += '/';   // append at the end
    pathDirectoryEdit->setText(directory);
    LogFile::instance() << "Directory retrived from browsing folders: " << directory << '\n';
    m_dirPath->setDirPath(directory);
    updateUI();

    m_dirPath->saveDirPathToFile(this);
}


#endif
