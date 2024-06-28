#ifndef JSONOBJECTBUILDER_H
#define JSONOBJECTBUILDER_H

#include <QVariantMap>
#include <QJsonObject>



class JsonObjectBuilder
{
public:
    JsonObjectBuilder();
    JsonObjectBuilder& add(QString field, QVariant value);
    JsonObjectBuilder& add(QString field, qlonglong value);
    JsonObjectBuilder& add(QString field, int value);
    JsonObjectBuilder& add(QString field, QString value);
    JsonObjectBuilder& add(QString field, QDate value);
    JsonObjectBuilder& add(QString field, QTime value);
    JsonObjectBuilder& add(QString field, QDateTime value);
    JsonObjectBuilder& add(QString field, float value);
    JsonObjectBuilder& add(QString field, double value);
    JsonObjectBuilder& add(QString field, QUuid value);
    JsonObjectBuilder& add(QString field, bool value);
    QJsonObject build();

protected:
    QVariantMap _values;
};

#endif // JSONOBJECTBUILDER_H
