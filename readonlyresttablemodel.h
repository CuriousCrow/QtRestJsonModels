#ifndef READONLYRESTTABLEMODEL_H
#define READONLYRESTTABLEMODEL_H

#include "abstractresttablemodel.h"

/** Simplified read-only REST table model
 * Implements soft row-based refresh algorithm (works with stable tables)
 */
class ReadOnlyRestTableModel : public AbstractRestTableModel
{
  enum RefreshMode {
    Soft,
    Hard
  };

public:
  ReadOnlyRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
  void setRefreshMode(RefreshMode mode);
private:
  RefreshMode _refreshMode = Soft;
  bool _firstRefresh = true;
protected slots:
  /* Response handler slots */
  virtual void processLoadReply();

  // QAbstractItemModel interface
public:
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

  // AbstractRestTableModel interface
public:
  virtual void tryToSubmit(qlonglong id);
  virtual void deleteItem(int row);
  virtual void addItem(QJsonObject &item);
};

#endif // READONLYRESTTABLEMODEL_H
