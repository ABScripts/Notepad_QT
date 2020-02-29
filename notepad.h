#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include "findmatcheswindow.h"

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

    void sopenFile(QFile *file);
    void ssaveFile(QFile *file);
    void changeInfoBar(int scale, QString safeMoedEnabled);

private slots:
    void changeFontFor();

    void selectSomeText(int position, int eposition);
    void changeBackgroundColor();
    void onSafeDataMode();
    void setFindWindow();
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
    Ui::FindMatchesWindow *findWindow{nullptr};
    QClipboard *clipboard = QGuiApplication::clipboard();
    int mTextEditScale = 0;
    QString mCurrentFilePath = "";
    bool mIsTextChanged = false;
};
#endif // NOTEPAD_H
