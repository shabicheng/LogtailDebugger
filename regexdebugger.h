#ifndef REGEXDEBUGGER_H
#define REGEXDEBUGGER_H

#include <QWidget>
#include <QRegExp>
#include <QTextCharFormat>
#include <QRegularExpression>

namespace Ui {
class RegexDebugger;
}

class RegexDebugger : public QWidget
{
    Q_OBJECT

public:
    explicit RegexDebugger(QWidget *parent = 0);
    ~RegexDebugger();


public slots:
    void OnStart(bool);

public:
    bool FindValidPrefix(QRegularExpression & regExp, QString & log, QString & reg);

    void SetValidIndex(int logIndex, int regIndex);

    void ClearFormat();

    void Performance(QRegularExpression & regExp, QString & log, QString & reg);

private:
    Ui::RegexDebugger *ui;
    QTextCharFormat mDefaultLogFormat;
    QTextCharFormat mDefaultRegFormat;
};

#endif // REGEXDEBUGGER_H
