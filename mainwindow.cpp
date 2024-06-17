#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QMessageBox>
#include <fstream>
#include <vector>
#include <map>

using namespace cv;
using namespace cv::dnn;
using namespace std;

vector<std::string> classes;
VideoCapture cap;
Net net;

const string modelConfiguration = "C:/Program Files/OpenCV/yolov4.cfg";
const string modelWeights = "C:/Program Files/OpenCV/yolov4.weights";
const string classesFile = "C:/Program Files/OpenCV/coco.names";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this)) {
    ui->setupUi(this);


    label = new QLabel(this);
    label->setGeometry(10, 10, 300, 20);
    label->setText("Tespit edilen nesneler: ");

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startVideo);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopVideo);
    connect(timer, &QTimer::timeout, this, &MainWindow::processFrameAndUpdateGUI);

    // YOLO model dosyalarını yükle
    net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    // Sınıf isimlerini yükle
    std::ifstream ifs(classesFile);
    std::string line;
    while (getline(ifs, line)) {
        classes.push_back(line);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::startVideo() {
    cap.open(0);
    if (!cap.isOpened()) {
        QMessageBox::critical(this, "Hata", "Kamera açılamadı!");
        return;
    }
    timer->start(30);
}

void MainWindow::stopVideo() {
    timer->stop();
    cap.release();
    ui->videoLabel->clear();
    label->clear(); // QLabel içeriğini temizle
}

void MainWindow::processFrameAndUpdateGUI() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;

    std::vector<std::string> detectedObjects = detectObjects(frame);

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->videoLabel->setPixmap(QPixmap::fromImage(img));

    // Tespit edilen nesneleri QLabel'a yaz
    std::string label_text = "Tespit edilen nesneler: ";
    std::map<std::string, int> objectCount;
    for (const auto& obj : detectedObjects) {
        objectCount[obj]++;
    }
    for (const auto& pair : objectCount) {
        label_text += pair.first + " (" + std::to_string(pair.second) + ") ";
    }
    label->setText(QString::fromStdString(label_text));
}

std::vector<std::string> MainWindow::detectObjects(cv::Mat &frame) {
    Mat blob;
    dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);
    net.setInput(blob);

    vector<cv::Mat> outs;
    net.forward(outs, net.getUnconnectedOutLayersNames());

    vector<int> classIds;
    vector<float> confidences;
    vector<cv::Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i) {
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > 0.5) {
                int centerX = static_cast<int>(data[0] * frame.cols);
                int centerY = static_cast<int>(data[1] * frame.rows);
                int width = static_cast<int>(data[2] * frame.cols);
                int height = static_cast<int>(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back(static_cast<float>(confidence));
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    vector<std::string> detectedObjects;
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        cv::Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame);
        detectedObjects.push_back(classes[classIds[idx]]);
    }
    return detectedObjects;
}

void MainWindow::drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame) {
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0), 2);
    std::string label = cv::format("%.2f", conf);
    if (!classes.empty()) {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ": " + label;
    }
    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = std::max(top, labelSize.height);
    cv::rectangle(frame, cv::Point(left, top - labelSize.height),
                  cv::Point(left + labelSize.width, top + baseLine), cv::Scalar(255, 255, 255), cv::FILLED);
    cv::putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
}
