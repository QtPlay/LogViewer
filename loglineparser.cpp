#include "loglineparser.h"

LogLineParser::LogLineParser(QObject *parent) : QObject(parent)
{
    m_errorCode = NO_ERROR;
}

QStringList LogLineParser::parseLine(const QString &line)
{
    QRegExp regExp(m_regExpFormatLine);
    if (!regExp.exactMatch(line)) {
        m_errorCode = BAD_TOKEN;
        return QStringList();
    }

    regExp.indexIn(line);
    QStringList parsedValues = regExp.capturedTexts();
    parsedValues.removeFirst();

    if (values.size() != m_listTokenInfo.size()) {
        m_errorCode = BAD_TOKEN;
        return QStringList();
    }

    return std::make_tuple(values, RESULT_OK);
}


QList<LogLineParser::TokenInfo> LogLineParser::tokenInfo()
{
    return m_listTokenInfo;
}

LogLineParser::ErrorCode LogLineParser::errorCode()
{
    return m_errorCode;
}

void LogLineParser::setTokenDescription(QMap<QString, LogLineParser::TokenDescription> tokenDescription)
{
    m_mapTokenDescription = tokenDescription;
}

void LogLineParser::setLineFormat(const QString &lineFormat)
{
    m_regExpFormatLine = convert2RegExpFormat(lineFormat);
}

QString LogLineParser::convert2RegExpFormat(const QString &format)
{
    QString outputFormat;
    QString lexeme;
    foreach (QChar currentSymbol, format) {
        bool resetLexeme = false;
        if (!lexeme.isEmpty()) {
            lexeme.append(currentSymbol);
            if (lexeme.contains(m_mapTokenDescription)) {
                outputFormat.append(m_mapTokenDescription.value(lexeme).regExp);
                m_listTokenInfo.append(TokenInfo(lexeme, m_mapTokenDescription.value(lexeme).format, m_mapTokenDescription.value(lexeme).type));
                lexeme.clear();
            } else {
                resetLexeme = true;
            }
        }

        if (resetLexeme) {
            for (auto it = lexeme.cbegin(); it != lexeme.cend(); ++it) {
                outputFormat.append(convertSymbol2RegExpFormat((*it)));
            }
            lexeme.clear();
        }

        if (currentSymbol == '%') {
            lexeme.append(currentSymbol);
            continue;
        } else {
            outputFormat.append(convertSymbol2RegExpFormat(currentSymbol));
        }
    }
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
