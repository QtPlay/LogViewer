#ifndef LOGLINEPARSER_H
#define LOGLINEPARSER_H

#include <QObject>
#include <QMap>
#include <QChar>
#include <QVariant>
#include <QDebug>
#include <QTime>
#include <QDate>
#include <QLocale>
#include <QDateTime>
#include <QStringList>

class LogLineParser : public QObject
{
    Q_OBJECT
public:
    enum ErrorCode {
        BAD_TOKEN,
        BAD_FORMAT,
        NO_ERROR
    };

    struct ReadError {
        ErrorCode errorCode;
        QString errorText;
    };

    struct TokenDescription {
        QString regExp;
        QString format;
        QVariant::Type type;

        TokenDescription(const QString& regExp, const QString& format, QVariant::Type type) {
            this->format = format;
            this->regExp = regExp;
            this->type = type;
        }

        TokenDescription() {}
    };

    struct TokenInfo {
        QString token;
        QString format;
        QVariant::Type type;

        TokenInfo(const QString& token, const QString& format, QVariant::Type type) {
            this->token = token;
            this->format = format;
            this->type = type;
        }
    };

    explicit LogLineParser(QObject *parent = 0);
    QStringList parseLine(const QString& line);

    QString regExpFormat() const;
    QList<TokenInfo> tokenInfo();
    ReadError error() const;

    void setTokenDescription(QMap<QString, LogLineParser::TokenDescription> tokenDescription);
    void setLineFormat(const QString& lineFormat);
    void setLocale(QLocale locale);

protected:
    QString convert2RegExpFormat(const QString& format);
    QString convertSymbol2RegExpFormat(const QChar symbol);

private:
    ReadError m_readError;
    QString m_regExpFormatLine;
    QMap <QString, TokenDescription> m_mapTokenDescription;
    QList <int> m_indexCheckedLexeme;
    QList <TokenInfo> m_listTokenInfo;
    QLocale m_locale;
};

#endif // LOGLINEPARSER_H
