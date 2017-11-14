#ifndef QMLBASERESTTABLEMODEL_H
#define QMLBASERESTTABLEMODEL_H

#include <QObject>
#include "baseresttablemodel.h"

class QmlBaseRestTableModel : public BaseRestTableModel
{
    Q_OBJECT
public:
    QmlBaseRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
    Q_INVOKABLE void set(int row, const QVariantMap &mapObj);
    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE append(const QVariantMap &mapObj);
    Q_INVOKABLE remove(int row);
    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;
};

#endif // QMLBASERESTTABLEMODEL_H
