#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
   void startVideo();
   void stopVideo();
   void processFrameAndUpdateGUI();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QLabel *label;
    std::vector<std::string> detectedObjects;
    std::vector<std::string> detectObjects(cv::Mat &frame);
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);
};
#endif // MAINWINDOW_H
