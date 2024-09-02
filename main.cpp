#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/* TODO: improvements
 - creating a Screenshots folder if it doesn't exist
 - checking if an actual '\League of Legends\Screenshots\' folder was selected, not just any directory
 - optimize DirectoryPath (allow passing wchar_t* not just QString)
 - optimize size of this project (staic linking mabe)
 - automate searching for '\League of Legends\Screenshots\' folder at the start
 - run this program together with League of Legends, mabe create like a bash script (alternative, run on startup)
*/
