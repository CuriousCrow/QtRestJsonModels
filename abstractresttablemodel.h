#ifndef ABSTRACTRESTTABLEMODEL_H
#define ABSTRACTRESTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QIcon>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QTimer>
#include <QMap>

class ParamContainer {
public:
  ParamContainer();
  void setParam(QString name, QVariant value);
  QUrlQuery urlQuery();
private:
  QMap<QString, QVariant> paramMap;
};

/** Abstract json model column */
class AbstractJsonColumn {
public:
  AbstractJsonColumn(QString name, QString caption = "");
  virtual QVariant data(int row, int role, const QJsonObject& jObj) const = 0;
  QString name;
  QString caption;
  bool offlineRefresh = false;
protected:
  QVariant jsonToVar(QJsonValue val) const;
  QVariant jsonToVar(QJsonObject jObj, QString key) const;
  qreal round(qreal val, int precision) const;
  virtual QString boolToStr(bool value) const;
};

/** Base column implementation */
class BaseJsonColumn : public AbstractJsonColumn {
public:
  BaseJsonColumn(QString name, QString caption = "");
  virtual QVariant data(int row, int role, const QJsonObject& jObj) const;
};

class AbstractRestTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  AbstractRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
  /* Base endpoint URL for basic rest operations */
  virtual void setServiceUrl(QUrl url, bool andRefresh = true);
  /* Specify original id field name (by default "id") */
  void setIdField(QString name);
  /* Register new model column */
  void registerColumn(AbstractJsonColumn* column);

  void registerColumn(QString name, QString caption = "");

  void setParam(QString name, QVariant value);

  void setDefaultIcon(QIcon icon);
  /* Is any unsaved changes in the model */
  bool isDirty();
  /* Submit all uncommited changes */
  void tryToSubmitAll();
  /* Submit one model row by id value */
  virtual void tryToSubmit(qlonglong id) = 0;
  /* Revert row uncommited changes */
  void revertRow(int row);
  /* Revert all uncommited changes */
  virtual void revertAll();
  /* Delete model row */
  virtual void deleteItem(int row) = 0;
  /* Add new model row */
  virtual void addItem(QJsonObject &item) = 0;

  virtual void modifyRowData(QJsonObject &item);

  void saveToFile(QString filename);

  bool loadFromFile(QString filename, bool clearOldData);

  void startOfflineRefreshTimer(int msecs);

  int columnIdxByName(QString name) const;

  void setIconColumnIdx(int idx);

  qlonglong idByRow(int row) const;

  int rowById(qlonglong id) const;
protected:
  QNetworkAccessManager* _network;
  QUrl _serviceUrl;
  QList<AbstractJsonColumn*> _columns;
  QList<qlonglong> _rowIndex;
  QHash<qlonglong, QJsonObject> _items;
  QHash<qlonglong, QJsonObject> _changes;
  ParamContainer _params;
  QString _idField = "id";
  QIcon _defaultIcon;
  int _iconColumnIdx = -1;
  QTimer* _offlineRefreshTimer;

  void clear();
  QUrl itemUrl(qlonglong itemId);
  QString columnNameByIdx(int col) const;
  QVariant jsonToVar(QJsonValue val) const;
signals:
  void beforeUpdate(QJsonObject &jObj, QString colName);
  void beforeAppend(QJsonObject &jObj);
public slots:
  /* Request and refresh all model data */
  void refresh();
  /* Refresh model calculated data (without a request the server) */
  void offlineRefresh();
  void refreshColumn(int col);
protected slots:
  /* Response handler slots */
  virtual void processLoadReply();
  void processDeleteReply();
  void processCreateReply();
  void processUpdateReply();
public:
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant cellData(int row, int column, int role = Qt::DisplayRole) const;
  QVariant cellData(int row, QString column, int role = Qt::DisplayRole) const;
  QJsonObject rowJsonObject(int row) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // ABSTRACTRESTTABLEMODEL_H
