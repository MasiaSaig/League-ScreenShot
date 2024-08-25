#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON
#include <QMainWindow>
#include <QDialog>
#include <QFileDialog>
// #include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE

class MainWindow :  public QMainWindow //public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    // ~MainWindow();

    // void setVisible(bool visible) override;

private slots:
    void browse();
    void onEditingFinished();

private:
    void createActions();
    void createTrayIcon(const QString &iconPath);

    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    QLabel *infoLabel;
    QLabel *pathDirectoryLabel;
    QLineEdit *pathDirectoryEdit;
    QPushButton *pathDirectoryButton;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QString *dir;

private:
    QString m_directory;
    // Ui::MainWindow *ui;
};

#endif // QT_NO_SYSTEMTRAYICON
#endif // MAINWINDOW_H
