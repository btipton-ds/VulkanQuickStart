#ifndef VULKANWIDGET_H
#define VULKANWIDGET_H

#include <QObject>
#include <QWidget>
#include <QVulkanWindow>

class VulkanWindow;

class VulkanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VulkanWidget(QWidget *parent = nullptr, VulkanWindow *w = nullptr);

public slots:
    void onVulkanInfoReceived(const QString &text);
    void onFrameQueued(int colorValue);
    void onGrabRequested();

private:
    VulkanWindow *m_window;
};

class VulkanRenderer
{
public:
    VulkanRenderer(VulkanWindow *w);

};

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    QVulkanWindowRenderer *createRenderer() override;

signals:
    void vulkanInfoReceived(const QString &text);
    void frameQueued(int colorValue);
};



#endif // VULKANWIDGET_H
