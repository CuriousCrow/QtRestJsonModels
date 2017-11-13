#ifndef BASERESTTABLEMODEL_H
#define BASERESTTABLEMODEL_H

#include <QAbstractTableModel>
#include "abstractresttablemodel.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>


/** Базовая табличная модель для работы с REST API
  Действия добавления и удаления строк не кэшируются моделью
  и отображаются моделью только после получения ответа от сервера.
  Изменения значений ячеек кэшируются и могут быть
  как отосланы на сервер (submit), так и откачены (revert).
*/
class BaseRestTableModel : public AbstractRestTableModel
{
  Q_OBJECT
public:
  BaseRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
  /* Отправка на сервер изменений строки */
  void tryToSubmit(qlonglong id);
  /* Откат изменений строки */
  void revert(int row);
  /* Удаление строки */
  void deleteItem(int row);
  /* Добавление строки */
  void addItem(QJsonObject &item);
private slots:
  /* Слоты обработки ответов от сервера */
  void processDeleteReply();
  void processCreateReply();
  void processUpdateReply();
public slots:
  virtual bool submit();
};

#endif // BASERESTTABLEMODEL_H
