#include "../headers/oauth2.h"
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QUrlQuery>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QFileDialog>
#include <QTimer>

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{
     m_google = new QOAuth2AuthorizationCodeFlow(this);
}

QOAuth2AuthorizationCodeFlow *OAuth2::getGoogle() const
{
    return m_google;
}

void OAuth2::getGoogleCalendarAuthorization()
{
    authorize("https://www.googleapis.com/auth/calendar");
}

void OAuth2::getGoogleDriveAuthorization()
{
    authorize("https://www.googleapis.com/auth/drive");
}

OAuth2::~OAuth2()
{
    delete this;
}

void OAuth2::refreshAccessTokenAtBeginning(QString fileName)
{
    QString name = QDir::currentPath() + "/../calendar/resources/" + fileName;
    QFile file(name);
    QByteArray val;

    file.open(QIODevice::ReadOnly);
    val = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();
    this->getGoogle()->setRefreshToken(object["refreshToken"].toString());

    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this->getGoogle(),&QOAuth2AuthorizationCodeFlow::refreshAccessToken);
    timer->start(1);

}

bool OAuth2::loggedIn(QString fileName)
{
    QString name = QDir::currentPath() + "/../calendar/resources/" + fileName;
    QFile file(name);
    return (file.size() == 0 ? false : true);
}

void OAuth2::refreshAccessToken()
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this->getGoogle(),&QOAuth2AuthorizationCodeFlow::refreshAccessToken);
    //3600000  = 1 sat
    timer->start(3600000);
}

void OAuth2::authorize(QString scope)
{
    m_google->setScope(scope);
    
    connect(m_google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](QUrl url){
        QUrlQuery query(url);

        query.addQueryItem("prompt", "consent");
        query.addQueryItem("access_type", "offline");
        url.setQuery(query);
        qDebug() << url;

        QDesktopServices::openUrl(url.toString());
    });
}

void OAuth2::setInformation()
{
    QByteArray val;
    QString fileName = QDir::currentPath() + "/../calendar/resources/credentials.json";
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        val = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();

    const auto settingsObject = object["installed"].toObject();
    const QUrl authUri(settingsObject["auth_uri"].toString());
    const auto clientId = settingsObject["client_id"].toString();
    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const auto clientSecret(settingsObject["client_secret"].toString());

    const auto redirectUris = settingsObject["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString());

    m_google->setAuthorizationUrl(authUri);
    m_google->setClientIdentifier(clientId);
    m_google->setAccessTokenUrl(tokenUri);
    m_google->setClientIdentifierSharedKey(clientSecret);
}

