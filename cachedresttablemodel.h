#ifndef CACHEDRESTTABLEMODEL_H
#define CACHEDRESTTABLEMODEL_H

#include <QAbstractTableModel>
#include "abstractresttablemodel.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>


/** Табличная модель для работы с REST API
  Действия добавления, удаления строк и редактирования ячеек кэшируются моделью
  и могут быть как отосланы на сервер (submit), так и откачены (revert).
*/
class CachedRestTableModel : public AbstractRestTableModel
{
  Q_OBJECT
public:
  CachedRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
  /* Отправка на сервер изменений строки */
  void tryToSubmit(qlonglong id);
  /* Откат изменений строки */
  void revert(int row);
  /* Откат всех кэшированных изменений */
  void revertAll();
  /* Удаление строки */
  void deleteItem(int row);
  /* Добавление строки */
  void addItem(QJsonObject &item);
private:
  int tempId = -1;
private slots:
  /* Слоты обработки ответов от сервера */
  void processDeleteReply();
  void processCreateReply();
  void processUpdateReply();

  void changeApplied(qlonglong id);
};

#endif // CACHEDRESTTABLEMODEL_H
