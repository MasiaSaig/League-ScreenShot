#ifndef LOGFILE_H
#define LOGFILE_H

#include <QFile>
#include <QIODevice>
#include <QDebug>

class LogFile
{
public:
    LogFile();
    ~LogFile();

public:
    inline static LogFile& instance(){
        return *m_instance.get();
    }

    template<typename T>
    LogFile& operator<< (const T message){
        if(m_file->isOpen()){
            m_fileStream << message;
            m_fileStream.flush();
        }else{
            qDebug() << "Log file not open.";
        }
        return *this;
    }

private:
    QTextStream m_fileStream;
    QFile *m_file;

    static std::unique_ptr<LogFile> m_instance;

private:
    LogFile(const LogFile&) = delete;
    void operator=(const LogFile&) = delete;
};

#endif // LOGFILE_H
