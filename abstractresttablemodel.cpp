#include "abstractresttablemodel.h"
#include <QFile>

AbstractRestTableModel::AbstractRestTableModel(QNetworkAccessManager *network, QObject *parent)
  : QAbstractTableModel(parent)
{
  _network = network;
  _offlineRefreshTimer = new QTimer(this);
  connect(_offlineRefreshTimer, SIGNAL(timeout()),
          this, SLOT(offlineRefresh()));
}

void AbstractRestTableModel::setServiceUrl(QUrl url, bool andRefresh)
{
  _serviceUrl = url;
  //TODO: Перенести в более правильное место
  if (_iconColumnIdx < 0) {
    _iconColumnIdx = columnIdxByName("name");
  }
  if (andRefresh)
    refresh();
}

void AbstractRestTableModel::setIdField(QString name)
{
  _idField = name;
}

void AbstractRestTableModel::registerColumn(AbstractJsonColumn* column)
{
  _columns.append(column);
}

void AbstractRestTableModel::registerColumn(QString name, QString caption)
{
  registerColumn(new BaseJsonColumn(name, caption));
}

void AbstractRestTableModel::setParam(QString name, QVariant value)
{
  _params.setParam(name, value);
}

void AbstractRestTableModel::setDefaultIcon(QIcon icon)
{
  _defaultIcon = icon;
}

bool AbstractRestTableModel::isDirty()
{
  return !_changes.isEmpty();
}

void AbstractRestTableModel::refresh()
{
  _serviceUrl.setQuery(_params.urlQuery());
  QNetworkReply* reply = _network->get(QNetworkRequest(_serviceUrl));
  connect(reply, SIGNAL(finished()),
          this, SLOT(processLoadReply()));
}

void AbstractRestTableModel::offlineRefresh()
{
  int rowCount = _rowIndex.count();
  if (rowCount = 0)
    return;
  int colIdx = 0;
  foreach(AbstractJsonColumn* col, _columns) {
    if (col->offlineRefresh) {
      emit dataChanged(index(0, colIdx), index(rowCount-1, colIdx));
    }
    colIdx++;
  }
}

void AbstractRestTableModel::refreshColumn(int col)
{
  emit dataChanged(index(0, col), index(_rowIndex.count()-1, col));
}

void AbstractRestTableModel::tryToSubmitAll()
{
  foreach (qlonglong key, _changes.keys()) {
    tryToSubmit(key);
  }
}

void AbstractRestTableModel::revert(int row)
{
  //При необходимости реализовать в потомке
}

void AbstractRestTableModel::revertAll()
{
  emit beginResetModel();
  _changes.clear();
  emit endResetModel();
}

void AbstractRestTableModel::modifyRowData(QJsonObject &item)
{
  qlonglong id = item.value(_idField).toVariant().toLongLong();
  if (!_rowIndex.contains(id))
    return;
  int row = _rowIndex.indexOf(id);
  _changes.insert(id, item);
  emit dataChanged(index(row, 0), index(row, columnCount()-1));
}

void AbstractRestTableModel::startOfflineRefreshTimer(int msecs)
{
  _offlineRefreshTimer->start(msecs);
}

int AbstractRestTableModel::columnIdxByName(QString name) const
{
  for(int i=0; i<_columns.count(); i++) {
    if (_columns.at(i)->name == name) {
      return i;
    }
  }
  return -1;
}

void AbstractRestTableModel::setIconColumnIdx(int idx)
{
  _iconColumnIdx = idx;
}

void AbstractRestTableModel::clear()
{
  _items.clear();
  _rowIndex.clear();
  _changes.clear();
}

QUrl AbstractRestTableModel::itemUrl(qlonglong itemId)
{
  QUrl url(_serviceUrl);
  url.setPath(url.path() + "/" + QString::number(itemId));
  return url;
}

QString AbstractRestTableModel::columnNameByIdx(int col) const
{
  return _columns.at(col)->name;
}

qlonglong AbstractRestTableModel::idByRow(int row) const
{
  return _rowIndex[row];
}

int AbstractRestTableModel::rowById(qlonglong id) const
{
  return _rowIndex.indexOf(id);
}

QVariant AbstractRestTableModel::jsonToVar(QJsonValue val) const
{
  if (val.type() == QJsonValue::String) {
    QString strVal = val.toString();
    QDateTime dtVal = QDateTime::fromString(strVal, Qt::ISODate);
    if (dtVal.isValid())
      return dtVal;
  }
  else if (val.type() == QJsonValue::Double) {
    int intVal = val.toInt(0);
    if (intVal != 0)
      return intVal;
  }
  return val.toVariant();
}

//TODO: Надо добавить Role
QVariant AbstractRestTableModel::cellData(int row, int column, int role) const
{
  if (row < 0 || column < 0)
    return QVariant();
  qlonglong itemId = idByRow(row);
  QJsonObject item = _changes.contains(itemId) ? _changes[itemId] : _items[itemId];
  return _columns.at(column)->data(row, role, item);
}

QVariant AbstractRestTableModel::cellData(int row, QString column, int role) const
{
  int colIdx = columnIdxByName(column);
  return cellData(row, colIdx, role);
}

QJsonObject AbstractRestTableModel::rowJsonObject(int row) const
{
  if (row < 0 || row >= rowCount()) {
    qCritical() << "Row is out of bounds";
    return QJsonObject();
  }
  qlonglong itemId = idByRow(row);
  return _changes.contains(itemId) ? _changes[itemId] : _items[itemId];
}

void AbstractRestTableModel::processLoadReply()
{
  emit beginResetModel();
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  QJsonDocument jDoc = QJsonDocument::fromJson(reply->readAll());
  clear();
  foreach(QJsonValue item, jDoc.array()){
    qlonglong id = item.toObject().value(_idField).toVariant().toLongLong();
    _rowIndex.append(id);
    _items.insert(id, item.toObject());
  }
  reply->deleteLater();
  emit endResetModel();
}

void AbstractRestTableModel::processDeleteReply()
{
  //Необходимо реализовать в потомках
}

void AbstractRestTableModel::processCreateReply()
{
  //Необходимо реализовать в потомках
}

void AbstractRestTableModel::processUpdateReply()
{
  //Необходимо реализовать в потомках
}

int AbstractRestTableModel::rowCount(const QModelIndex &parent) const
{
  return _rowIndex.count();
}

int AbstractRestTableModel::columnCount(const QModelIndex &parent) const
{
  return _columns.count();
}

QVariant AbstractRestTableModel::data(const QModelIndex &index, int role) const
{
  {
    if (!index.isValid())
      return QVariant();

    if (role == Qt::DecorationRole && !_defaultIcon.isNull() && (index.column() == _iconColumnIdx)) {
        return _defaultIcon;
    }
    //Для того, чтобы всегда можно было получить значение ID
    else if (role == Qt::UserRole) {
      return idByRow(index.row());
    }
    else {
      return cellData(index.row(), index.column(), role);
    }
  }
}

bool AbstractRestTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  {
    if (!index.isValid()) {
      return false;
    }
    if (role == Qt::EditRole) {
      qlonglong id = _rowIndex[index.row()];
      QJsonObject jObj;
      if (_changes.contains(id)) {
        jObj = _changes[id];
      }
      else {
        jObj = QJsonObject(_items[id]);
      }
      QVariant oldValue = jObj.value(columnNameByIdx(index.column()));
      QVariant newValue = value;
      if (newValue == oldValue)
        return true;
      jObj.insert(columnNameByIdx(index.column()), QJsonValue::fromVariant(value));
      emit beforeUpdate(jObj, columnNameByIdx(index.column()));
      _changes.insert(id, jObj);
      emit dataChanged(index, index);
    }
    return true;
  }
}

Qt::ItemFlags AbstractRestTableModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant AbstractRestTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{\
  if (section < 0 || section >= _columns.count())
    return QVariant();
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      return _columns.at(section)->caption;
    }
  }
  return QVariant();
}

void AbstractRestTableModel::saveToFile(QString filename)
{
  QJsonArray jRows;
  for(int i=0; i<rowCount(); i++) {
    QJsonObject rowObj = rowJsonObject(i);
    jRows.append(QJsonValue(rowObj));
  }
  QFile file(filename);
  file.open(QFile::WriteOnly);
  file.write(QJsonDocument(jRows).toJson());
  file.close();
}

bool AbstractRestTableModel::loadFromFile(QString filename, bool clearOldData)
{
  QFile file(filename);
  if (!file.exists())
    return false;
  file.open(QFile::ReadOnly);
  QJsonDocument jDoc = QJsonDocument::fromJson(file.readAll());
  QJsonArray jArray = jDoc.array();
  if (clearOldData) {
    //Удаление всех строк
    while (rowCount() > 0)
      deleteItem(0);
  }
  foreach(QJsonValue jVal, jArray) {
    QJsonObject jRow = jVal.toObject();
    jRow.remove(_idField);
    if (clearOldData) {
      addItem(jRow);
    }
    else {
      modifyRowData(jRow);
    }
  }
  return true;
}

AbstractJsonColumn::AbstractJsonColumn(QString name, QString caption)
{
  this->name = name;
  this->caption = caption.isEmpty() ? name : caption;
}

BaseJsonColumn::BaseJsonColumn(QString name, QString caption)
  : AbstractJsonColumn(name, caption)
{
  offlineRefresh = true;
}

QVariant BaseJsonColumn::data(int row, int role, const QJsonObject &jObj) const
{  
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    QVariant val = jsonToVar(jObj.value(name));
    if ((role == Qt::DisplayRole) && (val.type() == QVariant::Bool)) {
      return boolToStr(val.toBool());
    }
    return val;
  }
  return QVariant();
}

QVariant AbstractJsonColumn::jsonToVar(QJsonValue val) const
{
  if (val.type() == QJsonValue::String) {
    QString strVal = val.toString();
    QDateTime dtVal = QDateTime::fromString(strVal, Qt::ISODate);
    if (dtVal.isValid())
      return dtVal;
  }
  else if (val.type() == QJsonValue::Double) {
    int intVal = val.toInt(0);
    if (intVal != 0)
      return intVal;
  }
  else if (val.type() == QJsonValue::Bool) {
    return val.toBool();
  }
  return val.toVariant();
}

QVariant AbstractJsonColumn::jsonToVar(QJsonObject jObj, QString key) const
{
  return jsonToVar(jObj.value(key));
}

qreal AbstractJsonColumn::round(qreal val, int precision) const
{
  int tempInt = 1;
  for(int i=0; i<precision; i++) {
    tempInt *= 10;
  }
  return qRound(val*tempInt)*1./tempInt;
}

QString AbstractJsonColumn::boolToStr(bool value) const
{
  //Добавить функцию для перевода
  return value ? "Да" : "Нет";
}

ParamContainer::ParamContainer()
{
}

void ParamContainer::setParam(QString name, QVariant value)
{
  if (value.isValid())
    paramMap.insert(name, value);
  else
    paramMap.remove(name);
}

QUrlQuery ParamContainer::urlQuery()
{
  QUrlQuery query;
  foreach (QString key, paramMap.keys()) {
    query.addQueryItem(key, paramMap.value(key).toString());
  }
  return query;
}
