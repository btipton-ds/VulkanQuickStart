#include "vulkanwidget.h"
#include <QVulkanInstance>
#include <QVulkanWindow>
#include <memory>
//#include <vk_app.h>

using namespace std;

shared_ptr<QVulkanInstance> VulkanWidget::s_pQVI;
shared_ptr<QVulkanWindow> VulkanWidget::s_pQVW;

VulkanWidget::VulkanWidget(QWidget *parent)
    : QWidget{parent}
{
    if (!s_pQVI) {
        s_pQVI = make_shared<QVulkanInstance>();
        s_pQVI->setLayers({ "VK_LAYER_KHRONOS_validation" });
        if (!s_pQVI->create()) {
            qFatal("Failed to create Vulkan instance: %d", s_pQVI->errorCode());
            s_pQVI = nullptr;
        }
    }

    m_window = new VulkanWindow;
    m_window->setVulkanInstance(s_pQVI.get());

    QObject::connect(m_window, &VulkanWindow::frameQueued, this, &VulkanWidget::onFrameQueued);
}

void VulkanWidget::onVulkanInfoReceived(const QString &text)
{

}

void VulkanWidget::onFrameQueued(int colorValue)
{

}

void VulkanWidget::onGrabRequested()
{

}

/**************************************************************/

VulkanRenderer::VulkanRenderer(VulkanWindow *w)
{

}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    return nullptr;
}

