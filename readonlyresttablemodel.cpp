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
  //Hard refresh (emits modelReset signal)
  if (_refreshMode == Hard || _firstRefresh) {
    AbstractRestTableModel::processLoadReply();
    _firstRefresh = false;
    return;
  }
  //Soft refresh
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
  Q_UNUSED(index)
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ReadOnlyRestTableModel::tryToSubmit(qlonglong id)
{
  Q_UNUSED(id)
  //Read only model doesn't need submit methods
}

void ReadOnlyRestTableModel::deleteItem(int row)
{
  Q_UNUSED(row)
  //Read only model doesn't need submit methods
}

void ReadOnlyRestTableModel::addItem(QJsonObject &item)
{
  Q_UNUSED(item)
  //Read only model doesn't need submit methods
}
