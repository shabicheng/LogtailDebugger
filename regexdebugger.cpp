#include "regexdebugger.h"
#include "ui_regexdebugger.h"
#include <QRegExp>
#include <QDebug>
#include <QDateTime>

#pragma execution_character_set("utf-8")

RegexDebugger::RegexDebugger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegexDebugger)
{
    ui->setupUi(this);
    connect(ui->checkButton, SIGNAL(clicked(bool)), this, SLOT(OnStart(bool)));
    ui->regexEdit->setAcceptRichText(false);
    ui->regexEdit->setAcceptRichText(false);
    ui->resultEdit->setReadOnly(true);
    mDefaultLogFormat = ui->logTextEdit->currentCharFormat();
    mDefaultRegFormat = ui->regexEdit->currentCharFormat();
}

RegexDebugger::~RegexDebugger()
{
    delete ui;
}

void RegexDebugger::Performance(QRegularExpression & regex, QString & log, QString & )
{
    qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
    qint64 endTime = 0;
    int i = 0;
    for (; i < 100; ++i)
    {
        regex.match(log);
        endTime = QDateTime::currentMSecsSinceEpoch();
        if (endTime - nowTime >= 1000)
        {
            ++i;
            break;
        }
    }

    ui->resultEdit->setPlainText(QString("[INFO] 匹配成功, 共匹配 [%1] 次, 耗时 [%2] ms\n").arg(i).arg(endTime - nowTime) + ui->resultEdit->toPlainText());
}

bool RegexDebugger::FindValidPrefix(QRegularExpression &, QString & log, QString & reg)
{
    QList<int> leftList;
    for (int i = 0; i < reg.size(); ++i)
    {
        if (reg[i] == '\\')
        {
            ++i;
            continue;
        }
        if (reg[i] == '(')
        {
            leftList.append(i);
        }
        else if (reg[i] == ')')
        {
            leftList.append(i+1);
        }
    }
    for (int i = leftList.size() - 1; i >= 0; --i)
    {
        if (leftList[i] == 0)
            break;
        QString subReg = reg.left(leftList[i]);
        subReg += ".*";
        QRegularExpression subRegExp(subReg);
        if (!subRegExp.isValid())
        {
            ui->progressBar->setValue(100);
            ui->resultEdit->setText(QString("[ERROR] 未知错误 : %1, %2").arg(subReg).arg(subRegExp.errorString()));
            return false;
        }
        QRegularExpressionMatch match = subRegExp.match(log);
        if (match.captured(0).size() == log.size())
        {
            ui->progressBar->setValue(100);
            QStringList result;
            for (int i = 1; i <= match.lastCapturedIndex(); ++i)
                result.append(match.captured(i));
            ui->resultEdit->setText(QString("[ERROR] 匹配失败，只有部分字段匹配\n    提取成功的字段如下 : ") + result.join(", ") + "\n匹配成功的部分日志以及正则表达式参见高亮部分.");

            // check sub regex matched log index
            subReg = reg.left(leftList[i]);
            QRegularExpression subR(subReg);
            QRegularExpressionMatch subMatch = subR.match(log);
            if (subMatch.hasMatch() && subMatch.capturedStart(0) == 0)
            {
                qDebug() << "match sub log :" << subMatch.captured(0) << endl;
                qDebug() << "match sub regex :" << subReg << endl;

                SetValidIndex(subMatch.capturedEnd(0), leftList[i]);
            }

            return true;
        }
    }
    ui->progressBar->setValue(100);
    ui->resultEdit->setText(QString("[ERROR] 正则匹配失败"));
    return false;
}

void RegexDebugger::ClearFormat()
{
    ui->logTextEdit->setCurrentCharFormat(mDefaultLogFormat);
    ui->regexEdit->setCurrentCharFormat(mDefaultRegFormat);
    {
        QTextCursor defaultCursor(ui->logTextEdit->document());
        defaultCursor.setPosition(0, QTextCursor::MoveAnchor);
        defaultCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        defaultCursor.setCharFormat(mDefaultLogFormat);
    }
    {
        QTextCursor defaultCursor(ui->regexEdit->document());
        defaultCursor.setPosition(0, QTextCursor::MoveAnchor);
        defaultCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        defaultCursor.setCharFormat(mDefaultRegFormat);
    }
}

void RegexDebugger::SetValidIndex(int logIndex, int regIndex)
{
    {
        QTextDocument * document = ui->logTextEdit->document();
        QTextCursor highlightCursor(document);
        ///QTextCursor cursor(document);
        ///cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::green);
        colorFormat.setForeground(Qt::blue);
        colorFormat.setFontUnderline(true);
        highlightCursor.setPosition(0, QTextCursor::MoveAnchor);
        highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, logIndex);
        highlightCursor.mergeCharFormat(colorFormat);
        ///cursor.endEditBlock();
    }
    {
        QTextDocument * document = ui->regexEdit->document();
        QTextCursor highlightCursor(document);
        //QTextCursor cursor(document);
        //cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::green);
        colorFormat.setForeground(Qt::blue);
        colorFormat.setFontUnderline(true);
        highlightCursor.setPosition(0, QTextCursor::MoveAnchor);
        highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, regIndex);
        highlightCursor.mergeCharFormat(colorFormat);
        //cursor.endEditBlock();
    }
}



void RegexDebugger::OnStart(bool)
{
    ClearFormat();
    //ui->resultEdit->setPlainText("");
    ui->progressBar->setValue(0);
    QString log = ui->logTextEdit->toPlainText();
    QString reg = ui->regexEdit->toPlainText();
    qDebug() << "log :" << log << "\n reg :" << reg << endl;
    QRegularExpression regExp(reg);
    if (!regExp.isValid())
    {
        ui->progressBar->setValue(100);
        ui->resultEdit->setText(QString("[ERROR] 非法正则表达式，错误信息： %1").arg(regExp.errorString()));
        this->repaint();
        return;
    }
    if (log.size() == 0)
    {
        ui->progressBar->setValue(100);
        ui->resultEdit->setText(QString("[ERROR] 日志内容不能为空"));
        this->repaint();
        return;
    }
    QRegularExpressionMatch match = regExp.match(log);
    if (match.hasMatch() && match.captured(0).size() == log.size())
    {
        ui->progressBar->setValue(100);
        QStringList result;
        for (int i = 1; i <= match.lastCapturedIndex(); ++i)
            result.append(match.captured(i));
        ui->resultEdit->setText(QString("   提取成功的字段如下 : ") + result.join(", "));
        SetValidIndex(log.size(), reg.size());
        Performance(regExp, log, reg);
        this->repaint();
        return;
    }
    if (match.capturedStart(0) == 0)
    {
        ui->progressBar->setValue(100);
        QStringList result;
        for (int i = 1; i <= match.lastCapturedIndex(); ++i)
            result.append(match.captured(i));
        ui->resultEdit->setText(QString("[ERROR] 匹配失败，只有部分字段匹配\n    提取成功的字段如下 : ") + result.join(", ") + "\n匹配成功的部分日志以及正则表达式参见高亮部分.");
        SetValidIndex(match.capturedEnd(0), reg.size());
        this->repaint();
        return;
    }
    qDebug() << "无法匹配" << endl;

    FindValidPrefix(regExp, log, reg);
    this->repaint();
}
