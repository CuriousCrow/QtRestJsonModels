#include "readonlyresttablemodel.h"


ReadOnlyRestTableModel::ReadOnlyRestTableModel(QNetworkAccessManager *network, QObject *parent)
  : AbstractRestTableModel(network, parent)
{

}

void ReadOnlyRestTableModel::setRefreshMode(ReadOnlyRestTableModel::RefreshMode mode)
{
  _refreshMode = mode;
}

void ReadOnlyRestTableModel::processLoadReply()
{
  //Жесткое обновление модели с сигналом modelReset
  if (_refreshMode == Hard || _firstRefresh) {
    AbstractRestTableModel::processLoadReply();
    _firstRefresh = false;
    return;
  }
  //Мягкое построковое обновление модели
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  QJsonDocument jDoc = QJsonDocument::fromJson(reply->readAll());
  foreach(QJsonValue item, jDoc.array()){
    qlonglong id = item.toObject().value(_idField).toVariant().toLongLong();
    if (_rowIndex.contains(id)) {
      int row = _rowIndex.indexOf(id);
      _items.insert(id, item.toObject()); //Возможно нужен swap

      emit dataChanged(this->index(row, 0),
                       this->index(row, _columns.count() - 1));
    }
  }
  reply->deleteLater();
}


Qt::ItemFlags ReadOnlyRestTableModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ReadOnlyRestTableModel::tryToSubmit(qlonglong id)
{
  //Read only model don't need submit methods
}

void ReadOnlyRestTableModel::deleteItem(int row)
{
  //Read only model don't need submit methods
}

void ReadOnlyRestTableModel::addItem(QJsonObject &item)
{
  //Read only model don't need submit methods
}
