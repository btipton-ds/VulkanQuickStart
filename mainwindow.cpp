#include <iostream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVulkanWindow>
#include <QOpenGLWindow>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVulkanWindow* pVW = new QVulkanWindow(windowHandle());
    QOpenGLWindow* pOGLW = new QOpenGLWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionExit_triggered()
{
    void* p = nullptr;
    size_t size = sizeof(p);
    cout << size << "\n";
}

