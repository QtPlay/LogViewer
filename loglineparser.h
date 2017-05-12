#ifndef LOGLINEPARSER_H
#define LOGLINEPARSER_H

#include <QObject>
#include <QMap>
#include <QChar>
#include <QVariant>
#include <QStringList>

class LogLineParser : public QObject
{
    Q_OBJECT
public:
    enum ResultCode {
        RESULT_OK,
        RESULT_BAD_FORMAT
    };

    struct TokenDescription {
        QString regExp;
        QString format;
        QVariant::Type typeVariable;

        TokenDescription(const QString& regExp, const QString& format, QVariant::Type typeVariable) {
            this->format = format;
            this->regExp = regExp;
            this->typeVariable = typeVariable;
        }

        TokenDescription() {}
    };

    struct TokenInfo {
        QString value;
        QVariant::Type typeVariable;

        TokenInfo(const QString& value, QVariant::Type typeVariable) {
            this->value = value;
            this->typeVariable = typeVariable;
        }
    };

    explicit LogLineParser(const QString formatLine, QObject *parent = 0);
    std::tuple<QStringList, ResultCode> parseLine(const QString& line);
    QList <QVariant::Type> getListVariableTypeToken() const;

protected:
    void initialize();

private:
    QString m_regExpFormatLine;
    QString m_formatLine;
    QMap <QChar, TypeVariableForRegExp> m_mapTokenInfo;
    QList <QVariant::Type> m_variableTypeToken;
};

#endif // LOGLINEPARSER_H
