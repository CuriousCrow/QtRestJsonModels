#include "basejsonaction.h"
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUuid>

BaseJsonAction::BaseJsonAction(QNetworkAccessManager *nam, QString baseUrl)
  : QObject()
{
  _network = nam;
  _baseUrl = baseUrl;
}

void BaseJsonAction::setParam(QString name, QVariant value)
{
  _params.insert(name, value);
}

void BaseJsonAction::sendAction(QString action, QJsonDocument &jDoc)
{
  QUrl url(_baseUrl + action + paramStr());
  QNetworkRequest request(url);
  QNetworkReply* reply = _network->post(request, jDoc.toJson());
  reply->setProperty("action", action);
  connect(reply, SIGNAL(finished()),
          this, SLOT(replyFinished()));
}

void BaseJsonAction::replyFinished()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  bool result = (reply->error() == QNetworkReply::NoError);
  QString action = reply->property("action").toString();
  QJsonDocument jDoc = QJsonDocument::fromJson(reply->readAll());
  emit actionResult(action, result, jDoc);
}

QString BaseJsonAction::paramStr()
{
  if (_params.isEmpty())
    return "";
  QString res = "?";
  foreach (QString key, _params.keys()) {
    res = res.append(key + "=" + _params.value(key).toString() + "&");
  }
  res = res.left(res.length()-1);
  return res;
}
