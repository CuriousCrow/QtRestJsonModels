#include "cachedresttablemodel.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>


CachedRestTableModel::CachedRestTableModel(QNetworkAccessManager* network, QObject *parent)
  : AbstractRestTableModel(network, parent)
{
}

void CachedRestTableModel::tryToSubmit(qlonglong id)
{
  if (_rowIndex.indexOf(id) < 0) {
    //Запрос на удаление
    QNetworkRequest delRequest(itemUrl(id));
    QNetworkReply* reply = _network->deleteResource(delRequest);
    reply->setProperty("ID", id);
    connect(reply, SIGNAL(finished()),
            this, SLOT(processDeleteReply()));
  }
  else {
    QJsonDocument jDoc = QJsonDocument(_changes[id]);
    //Добавление
    if (id < 0) {
      QNetworkRequest createRequest(_serviceUrl);
      createRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      QNetworkReply* reply = _network->post(createRequest, jDoc.toJson());
      reply->setProperty("ID", id);
      connect(reply, SIGNAL(finished()),
              this, SLOT(processCreateReply()));
    }
    //Редактирование
    else {
      QNetworkRequest updateRequest(itemUrl(id));
      updateRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      QNetworkReply* reply = _network->put(updateRequest, jDoc.toJson());
//      qDebug() << jDoc.toJson();
      reply->setProperty("ID", id);
      connect(reply, SIGNAL(finished()),
              this, SLOT(processUpdateReply()));
    }
  }
}

void CachedRestTableModel::revert(int row)
{
  //TODO: По хорошему только полный откат
  if ((row < 0) || (row >= _items.count())) {
    return;
  }
  _changes.remove(_rowIndex.at(row));
  emit dataChanged(this->index(row, 0),
                   this->index(row, _columns.count() - 1));
}

void CachedRestTableModel::revertAll()
{
  refresh();
}

void CachedRestTableModel::deleteItem(int row)
{
  if ((row < 0) || (row >= _rowIndex.count())) {
    return;
  }

  emit beginRemoveRows(QModelIndex(), row, row);
  qlonglong id = _rowIndex.takeAt(row);
  _changes.insert(id, QJsonObject());
  emit endRemoveRows();
}

void CachedRestTableModel::addItem(QJsonObject &item)
{
  emit beforeAppend(item);
  emit beginInsertRows(QModelIndex(), _items.count(), _items.count());
  qlonglong id = tempId--;
  _rowIndex.append(id);
  _changes.insert(id, item);
  emit endInsertRows();
}

void CachedRestTableModel::processDeleteReply()
{
  qDebug() << "delete action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    qlonglong id = reply->property("ID").toLongLong();
    changeApplied(id);
    qDebug() << "record (id=" << id << ") successfully deleted";
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();
}

void CachedRestTableModel::processCreateReply()
{
  qDebug() << "create action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    QJsonDocument jDoc = QJsonDocument::fromJson(reply->readAll());
    changeApplied(reply->property("ID").toLongLong());
    qlonglong id = jDoc.object().value("id").toVariant().toLongLong();
    qDebug() << "record (id=" << id << ") successfully created";
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();
}

void CachedRestTableModel::processUpdateReply()
{
  qDebug() << "update action reply";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError){
    qlonglong id = reply->property("ID").toLongLong();
    changeApplied(id);
    qDebug() << "record (id=" << id << ") successfully updated";
  }
  else {
    qDebug() << reply->errorString();
  }
  reply->deleteLater();  
}

void CachedRestTableModel::changeApplied(qlonglong id)
{
  _changes.remove(id);
  //Все изменения получены сервером и можно перезагружать справочник
  if (_changes.isEmpty())
    refresh();
}
