#ifndef RELATIONSIGN_H
#define RELATIONSIGN_H

#include <QString>
#include <QStringList>

class RelationSign
{
public:
    enum Sign {
        NE,
        LE,
        L,
        GE,
        G,
        E,
        NOT_VALID
    };

public:
    RelationSign();

    static QString convert2TextValue(RelationSign::Sign sign);
    static RelationSign::Sign fromText(const QString& textValue);
    static RelationSign::Sign fromNumber(short value);
    static QStringList toStringList();
};

template <class T>
class CompareValue {
public:
    static bool compare(T firstValue, T secondValue, RelationSign::Sign sign) {
        switch (sign) {
        case RelationSign::NE:
            return firstValue != secondValue;
        case RelationSign::LE:
            return firstValue <= secondValue;
        case RelationSign::L:
            return firstValue < secondValue;
        case RelationSign::GE:
            return firstValue >= secondValue;
        case RelationSign::G:
            return firstValue > secondValue;
        case RelationSign::E:
            return firstValue == secondValue;
        }
    }

private:
    CompareValue();
};

#endif // RELATIONSIGN_H
