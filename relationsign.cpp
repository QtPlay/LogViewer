#include "relationsign.h"

RelationSign::RelationSign()
{

}

QString RelationSign::convert2TextValue(RelationSign::Sign sign)
{
    switch (sign) {
    case L:
        return QString("<");
    case E:
        return QString("=");
    case NE:
        return QString("!=");
    case LE:
        return QString("<=");
    case G:
        return QString(">");
    case GE:
        return QString(">=");
    default:
        return QString();
    }
}

RelationSign::Sign RelationSign::fromText(const QString &textValue)
{
    if (textValue == "<") return L;
    else if (textValue == "=") return E;
    else if (textValue == "!=") return NE;
    else if (textValue == "<=") return LE;
    else if (textValue == ">") return G;
    else if (textValue == ">=") return GE;
    else return NOT_VALID;
}

RelationSign::Sign RelationSign::fromNumber(short value)
{

}

QStringList RelationSign::toStringList()
{
    QStringList values;
    values << convert2TextValue(L);
    values << convert2TextValue(E);
    values << convert2TextValue(NE);
    values << convert2TextValue(LE);
    values << convert2TextValue(G);
    values << convert2TextValue(GE);
    return values;
}
