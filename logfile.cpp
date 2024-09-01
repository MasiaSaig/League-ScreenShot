#include "logfile.h"

std::unique_ptr<LogFile> LogFile::m_instance = std::make_unique<LogFile>();

LogFile::LogFile()
{
    m_file = new QFile("logs.log");
    if(!m_file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODeviceBase::Text)){
        qDebug() << "Couldn't open log file.";
    }
    m_fileStream.setDevice(m_file);
    m_fileStream.setEncoding(QStringEncoder::Utf16);
}

LogFile::~LogFile() {
    m_file->close();
    delete m_file;
}
