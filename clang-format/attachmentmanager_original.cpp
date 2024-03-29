#include "../headers/attachmentmanager.h"
#include <QOAuthHttpServerReplyHandler>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QMimeType>
#include <QMimeDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include "../headers/attachmentinfo.h"

AttachmentManager::AttachmentManager(){

    m_auth = new OAuth2();
    m_auth->getGoogleDriveAuthorization();
    m_auth->setInformation();
    auto replyHandler = new QOAuthHttpServerReplyHandler(m_auth);
    m_auth->getGoogle()->setReplyHandler(replyHandler);
    connect(m_auth->getGoogle(), &QOAuth2AuthorizationCodeFlow::tokenChanged, this, &AttachmentManager::onTokenChanged);
    m_auth->refreshAccessToken();
}

void AttachmentManager::grantAccess()
{
    connect(m_auth->getGoogle(), &QOAuth2AuthorizationCodeFlow::granted, this, &AttachmentManager::onGranted);
    m_auth->getGoogle()->grant();
}


void AttachmentManager::uploadAttachment(QFile* file)
{

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::RelatedType);
    QString boundary = multiPart->boundary();

    file->open(QIODevice::ReadOnly);
     QFileInfo fileInfo(*file);
     QString fileName = fileInfo.baseName();

     QMimeDatabase dataBase;
     QString mimeType = dataBase.mimeTypeForData(file).name();


     QHttpPart metaDataPart;
     metaDataPart.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json; charset=UTF-8"));
     metaDataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg("metaData")));
     metaDataPart.setBody(QString("{\"name\": \"%1\",\n\"mimeType\": \"%2\"\n}").arg(fileName).arg(mimeType).toUtf8());

     QHttpPart filePart;
     filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
     filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg("fileContent")));
     filePart.setBodyDevice(file);

     file->setParent(multiPart);

     multiPart->append(metaDataPart);
     multiPart->append(filePart);


     QString token = m_auth->getGoogle()->token();
     QNetworkRequest request(QUrl("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart&fields=webViewLink,mimeType,name,id,iconLink"));
     request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
     request.setRawHeader("Content-Type", (QString("multipart/related; charset=UTF-8; boundary=%1").arg(boundary)).toUtf8());

     QNetworkAccessManager *manager= new QNetworkAccessManager;
     m_reply = manager->post(request, multiPart);
     multiPart->setParent(m_reply); // delete the multiPart with the reply

     connect(m_reply, &QNetworkReply::finished, this, &AttachmentManager::onFinished);
     connect(m_reply, &QNetworkReply::finished, m_reply, &QNetworkReply::deleteLater);
}

void AttachmentManager::onGranted()
{
    qDebug() << "Access Granted!";
    QString fileName = QDir::currentPath() + "/../calendar/resources/tokenDrive.json";
    QFile file(fileName);
    QString string = QString("{\"refreshToken\" : \"%2\" }").arg(m_auth->getGoogle()->refreshToken());
    if(file.open(QIODevice::ReadWrite)){
        file.write(string.toUtf8());
    }

    emit accessGrantedSignal();
}

void AttachmentManager::onFinished()
{
    qDebug() << "GOOGLE DRIVE POST REQUEST FINISHED. Error? " << (m_reply->error() != QNetworkReply::NoError);
    QString strReply = (QString) m_reply->readAll();
    qDebug() << strReply;

    QJsonObject jsonObject = QJsonObject(QJsonDocument::fromJson(strReply.toUtf8()).object());
    attachmnets.push_back(new AttachmentInfo(jsonObject["id"].toString(), jsonObject["webViewLink"].toString(), jsonObject["mimeType"].toString(), jsonObject["name"].toString(), jsonObject["iconLink"].toString()));
    emit fileUploaded();

}

void AttachmentManager::onTokenChanged()
{
    qDebug() << "Drive access token changed";
    qDebug() << m_auth->getGoogle()->token();
}

OAuth2 *AttachmentManager::auth() const
{
    return m_auth;
}

void AttachmentManager::setAuth(OAuth2 *auth)
{
    m_auth = auth;
}

