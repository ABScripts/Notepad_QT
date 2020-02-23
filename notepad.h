#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>

#include <QClipboard>
#include <QGuiApplication>

QT_BEGIN_NAMESPACE
namespace Ui {
class Notepad;
}
class QFile;

QT_END_NAMESPACE

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    Notepad(QWidget *parent = nullptr);
    ~Notepad();

    void openFile(QFile *file);
    void saveFile(QFile *file);

private slots:
    void changeColor();
    void changeFont();
    void openFileAs();
    void saveFileAs();
    void setZoom();
    void saveFile();

private:
    void interactWithFile(const QString &fileName, QIODevice::OpenModeFlag flag,
                          void (Notepad::*action)(QFile *file));
private:
    Ui::Notepad *ui{nullptr};
    QClipboard *clipboard = QGuiApplication::clipboard();
    int mTextEditScale = 0;
    QString mCurrentFilePath = "";
    bool mIsTextChanged = false;
};
#endif // NOTEPAD_H
