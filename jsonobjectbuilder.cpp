#include "jsonobjectbuilder.h"

JsonObjectBuilder::JsonObjectBuilder()
{

}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QVariant value)
{
    _values.insert(field, value);
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, qlonglong value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, int value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QString value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QDate value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QTime value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QDateTime value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, float value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, double value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, QUuid value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

JsonObjectBuilder &JsonObjectBuilder::add(QString field, bool value)
{
    _values.insert(field, QVariant(value));
    return *this;
}

QJsonObject JsonObjectBuilder::build()
{
    return QJsonObject::fromVariantMap(this->_values);
}
