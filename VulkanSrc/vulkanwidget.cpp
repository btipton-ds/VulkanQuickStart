#include "vulkanwidget.h"

VulkanWidget::VulkanWidget(QWidget *parent, VulkanWindow *w)
    : QWidget{parent}
{

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

