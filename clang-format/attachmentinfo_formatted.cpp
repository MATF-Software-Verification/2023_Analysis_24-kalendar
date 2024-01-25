#include "../headers/attachmentinfo.h"

AttachmentInfo::AttachmentInfo(QString fileId, QString fileUrl,
                               QString mimeType, QString name, QString iconLink)
    : m_fileId(fileId), m_fileUrl(fileUrl), m_mimeType(mimeType), m_name(name),
      m_iconLink(iconLink) {}

QString AttachmentInfo::fileId() const { return m_fileId; }

QString AttachmentInfo::fileUrl() const { return m_fileUrl; }

QString AttachmentInfo::mimeType() const { return m_mimeType; }

QString AttachmentInfo::name() const { return m_name; }

QString AttachmentInfo::iconLink() const { return m_iconLink; }
