#ifndef FINDMATCHESWINDOW_H
#define FINDMATCHESWINDOW_H

#include <QDialog>

namespace Ui {
class FindMatchesWindow;
}

class FindMatchesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FindMatchesWindow(QWidget *parent = nullptr);
    ~FindMatchesWindow();

    QString text;
    int currentSearchPosition = 0;

signals:
    void selectText(int, int);

private slots:
    void on_pushButton_clicked();

private:
    Ui::FindMatchesWindow *ui;
};

#endif // FINDMATCHESWINDOW_H
