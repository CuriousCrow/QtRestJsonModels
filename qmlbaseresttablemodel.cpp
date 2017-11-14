#include "qmlbaseresttablemodel.h"


QmlBaseRestTableModel::QmlBaseRestTableModel(QNetworkAccessManager *network, QObject *parent)
    : BaseRestTableModel(network, parent)
{
}

void QmlBaseRestTableModel::set(int row, const QVariantMap &mapObj)
{
    foreach(QString key, mapObj.keys()) {
        setData(index(row, columnIdxByName(key)), mapObj.value(key));
    }
    tryToSubmitAll();
}

QVariantMap QmlBaseRestTableModel::get(int row)
{
    return rowJsonObject(row).toVariantMap();
}

QmlBaseRestTableModel::append(const QVariantMap &mapObj)
{
    QJsonObject jObj = QJsonObject::fromVariantMap(mapObj);
    addItem(jObj);
    tryToSubmitAll();
}

QmlBaseRestTableModel::remove(int row)
{
    removeRow(row);
}

QVariant QmlBaseRestTableModel::data(const QModelIndex &index, int role) const
{
    if (role >= Qt::UserRole) {
        int col = role - Qt::UserRole;
        return data(this->index(index.row(), col), Qt::DisplayRole);
    }
    return BaseRestTableModel::data(index, role);
}

QHash<int, QByteArray> QmlBaseRestTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    for(int i=0; i<_columns.count(); i++) {
        roles[Qt::UserRole + i] = qPrintable(_columns.at(i)->name);
    }
    return roles;
}
