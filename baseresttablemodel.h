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


/** Basic table model for handling entity list through REST API
  Add and Delete operations are not cached by the model (and therefore cannot be reverted simply)
  and applied only after having received server response.
  Modify operations would be cached inside model and then can be either submitted, or reverted.
*/
class BaseRestTableModel : public AbstractRestTableModel
{
  Q_OBJECT
public:
  BaseRestTableModel(QNetworkAccessManager* network, QObject* parent = 0);
  /* Submit row changes to REST service */
  void tryToSubmit(qlonglong id);
  /* Revert local row changes */
  void revert(int row);
  /* Delete row */
  void deleteItem(int row);
  /* Add row */
  void addItem(QJsonObject &item);
private slots:
  /* Response handlers slots */
  void processDeleteReply();
  void processCreateReply();
  void processUpdateReply();
public slots:
  virtual bool submit();
};

#endif // BASERESTTABLEMODEL_H
