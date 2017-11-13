#ifndef BASEJSONACTION_H
#define BASEJSONACTION_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>

class BaseJsonAction : public QObject
{
  Q_OBJECT
public:
  BaseJsonAction(QNetworkAccessManager* nam, QString baseUrl);
  void setParam(QString name, QVariant value);
  void sendAction(QString action, QJsonDocument& jDoc);
private slots:
  void replyFinished();
signals:
  void actionResult(QString action, bool success, QJsonDocument& jDoc);
private:
  QNetworkAccessManager* _network;
  QString _baseUrl;
  QMap<QString, QVariant> _params;
  QString paramStr();
};

#endif // BASEJSONACTION_H
