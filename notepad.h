#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class Notepad; }
QT_END_NAMESPACE

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    Notepad(QWidget *parent = nullptr);
    ~Notepad();

    void openFile(QFile *file);

    void saveFile(QFile *file);



private:
    Ui::Notepad *ui;

    QClipboard *clipboard = QGuiApplication::clipboard();

    int textEditScale = 0;

    QString currentFilePath = "";

    bool textChanged = false;

private:

    void interactWithFile(QString fileName, QIODevice::OpenModeFlag flag,
                           void (Notepad::*action)(QFile *file));


private slots:
    void changeColor();
    void changeFont();
    void openFileAs();
    void saveFileAs();
    void setZoom();
    void saveFile();
};
#endif // NOTEPAD_H
