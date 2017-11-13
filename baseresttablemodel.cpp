#include "baseresttablemodel.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

BaseRestTableModel::BaseRestTableModel(QNetworkAccessManager* network, QObject *parent)
  : AbstractRestTableModel(network, parent)
{
}

void BaseRestTableModel::tryToSubmit(qlonglong id)
{
  QJsonDocument jDoc = QJsonDocument(_changes[id]);
  QNetworkRequest updateRequest(itemUrl(id));
  updateRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QNetworkReply* reply = _network->put(updateRequest, jDoc.toJson());
  reply->setProperty("ID", id);
  connect(reply, SIGNAL(finished()),
          this, SLOT(processUpdateReply()));  
}

void BaseRestTableModel::revert(int row)
{
  if ((row < 0) || (row >= _items.count())) {
    return;
  }
  _changes.remove(_rowIndex.at(row));
  emit dataChanged(this->index(row, 0),
                   this->index(row, _columns.count() - 1));
}

void BaseRestTableModel::deleteItem(int row)
{
  if ((row < 0) || (row >= _items.count())) {
    return;
  }

  qlonglong id = idByRow(row);
  QNetworkRequest delRequest(itemUrl(id));
  QNetworkReply* reply = _network->deleteResource(delRequest);
  reply->setProperty("ID", id);
  connect(reply, SIGNAL(finished()),
          this, SLOT(processDeleteReply()));
}

void BaseRestTableModel::addItem(QJsonObject &item)
{
  emit beforeAppend(item);
  QJsonDocument jDoc = QJsonDocument(item);
  QNetworkRequest createRequest(_serviceUrl);
  createRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QNetworkReply* reply = _network->post(createRequest, jDoc.toJson());
  connect(reply, SIGNAL(finished()),
          this, SLOT(processCreateReply()));
}

void BaseRestTableModel::processDeleteReply()
{
  qDebug() << "delete action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    qlonglong id = reply->property("ID").toLongLong();
    int row = _rowIndex.indexOf(id);
    emit beginRemoveRows(QModelIndex(), row, row);
    _rowIndex.removeAt(row);
    _items.remove(id);
    _changes.remove(id);
    emit endRemoveRows();
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();
}

void BaseRestTableModel::processCreateReply()
{
  qDebug() << "create action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    QJsonDocument jDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject jObj = jDoc.object();
    emit beginInsertRows(QModelIndex(), _items.count(), _items.count());
    qlonglong id = jObj.value(_idField).toVariant().toLongLong();
    _rowIndex.append(id);
    _items.insert(id, jObj);
    emit endInsertRows();
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();
}

void BaseRestTableModel::processUpdateReply()
{
  qDebug() << "update action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    qlonglong id = reply->property("ID").toLongLong();
    int row = _rowIndex.indexOf(id);
    _items.insert(id, _changes.take(id)); //Возможно нужен swap

    emit dataChanged(this->index(row, 0),
                     this->index(row, _columns.count() - 1));
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();
}


bool BaseRestTableModel::submit()
{
  tryToSubmitAll();
}
