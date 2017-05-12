#ifndef TYPECONVERSION_H
#define TYPECONVERSION_H

#include <QString>
#include <QVariant>

class TypeConversion
{
public:
    TypeConversion(){}
};

class TypeConversionQVariantToSqlite: public TypeConversion
{
public:
    static QString convert(QVariant::Type sourceType) {
        switch (sourceType) {
        case QVariant::Int:
            return QString("integer");
        case QVariant::Double:
            return QString("real");
        case QVariant::String:
            return QString("text");
        case QVariant::Date:
            return QString("integer");
        case QVariant::Time:
            return QString("integer");
        case QVariant::DateTime:
            return QString("integer");
        default:
            return QString("text");
        }
    }
};

#endif // TYPECONVERSION_H
