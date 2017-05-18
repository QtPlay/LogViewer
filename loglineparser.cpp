#include "loglineparser.h"

LogLineParser::LogLineParser(QObject *parent) : QObject(parent)
{
    m_readError.errorCode = NO_ERROR;
    m_locale = QLocale();
}

QStringList LogLineParser::parseLine(const QString &line)
{
    m_readError.errorCode = NO_ERROR;
    m_readError.errorText.clear();

    QRegExp regExp(m_regExpFormatLine);
    if (!regExp.exactMatch(line)) {
        m_readError.errorCode = BAD_TOKEN;
        m_readError.errorText.append(regExp.errorString());
        return QStringList();
    }

    regExp.indexIn(line);
    QStringList parsedValues = regExp.capturedTexts();
    parsedValues.removeFirst();

    if (parsedValues.size() != m_listTokenInfo.size()) {
        m_readError.errorCode = BAD_TOKEN;
        m_readError.errorText.append("Count parsed values not equal count description token");
    } else {
        for (auto it = m_indexCheckedLexeme.cbegin(); it != m_indexCheckedLexeme.cend(); ++it) {
            TokenInfo tokenInfo = m_listTokenInfo.at((*it));
            switch (tokenInfo.type) {
            case QVariant::Time:
                if (!m_locale.toTime(parsedValues.at((*it)), tokenInfo.format).isValid()) {
                    m_readError.errorCode = BAD_FORMAT;
                    m_readError.errorText.append(QString("Bad time format (%1, %2)\n").arg(parsedValues.value((*it))).arg(tokenInfo.format));
                }
                break;
            case QVariant::Date:
                if (!m_locale.toDate(parsedValues.at((*it)), tokenInfo.format).isValid()) {
                    m_readError.errorCode = BAD_FORMAT;
                    m_readError.errorText.append(QString("Bad date format (%1, %2)\n").arg(parsedValues.value((*it))).arg(tokenInfo.format));
                }
                break;
            case QVariant::DateTime:
                if (!m_locale.toDateTime(parsedValues.at((*it)), tokenInfo.format).isValid()) {
                    m_readError.errorCode = BAD_FORMAT;
                    m_readError.errorText.append(QString("Bad datetime format (%1, %2)\n").arg(parsedValues.value((*it))).arg(tokenInfo.format));
                }
                break;
            default:
                break;
            }
        }
    }

    return parsedValues;
}

QString LogLineParser::regExpFormat() const
{
    return m_regExpFormatLine;
}


QList<LogLineParser::TokenInfo> LogLineParser::tokenInfo()
{
    return m_listTokenInfo;
}

LogLineParser::ReadError LogLineParser::error() const
{
    return m_readError;
}

void LogLineParser::setTokenDescription(QMap<QString, LogLineParser::TokenDescription> tokenDescription)
{
    m_mapTokenDescription = tokenDescription;
}

void LogLineParser::setLineFormat(const QString &lineFormat)
{
    m_listTokenInfo.clear();
    m_indexCheckedLexeme.clear();
    m_regExpFormatLine = convert2RegExpFormat(lineFormat);
}

void LogLineParser::setLocale(QLocale locale)
{
    m_locale = QLocale(locale);
}

QString LogLineParser::convert2RegExpFormat(const QString &format)
{
    QString outputFormat;
    QString lexeme;
    foreach (QChar currentSymbol, format) {
        bool resetLexeme = false;
        if (!lexeme.isEmpty()) {
            lexeme.append(currentSymbol);
            if (m_mapTokenDescription.contains(lexeme)) {
                outputFormat.append(m_mapTokenDescription.value(lexeme).regExp);

                QVariant::Type typeLexeme = m_mapTokenDescription.value(lexeme).type;
                if (typeLexeme == QVariant::Time || typeLexeme == QVariant::Date || typeLexeme == QVariant::DateTime) {
                    m_indexCheckedLexeme << m_listTokenInfo.size();
                }

                m_listTokenInfo.append(TokenInfo(lexeme, m_mapTokenDescription.value(lexeme).format, m_mapTokenDescription.value(lexeme).type));
                lexeme.clear();
                continue;
            } else {
                resetLexeme = true;
            }
        }

        if (resetLexeme) {
            for (auto it = lexeme.cbegin(); it != lexeme.cend(); ++it) {
                outputFormat.append(convertSymbol2RegExpFormat((*it)));
            }
            lexeme.clear();
            continue;
        }

        if (currentSymbol == '%') {
            lexeme.append(currentSymbol);
            continue;
        } else {
            outputFormat.append(convertSymbol2RegExpFormat(currentSymbol));
        }
    }
    return outputFormat;
}

QString LogLineParser::convertSymbol2RegExpFormat(const QChar symbol)
{
    switch (symbol.toLatin1()) {
    case ' ':
        return QString("\\s+");
    default:
        return QString("\\").append(symbol);
    }
}
