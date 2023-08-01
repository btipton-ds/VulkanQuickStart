#include "vulkanwidget.h"

VulkanWidget::VulkanWidget(VulkanWindow *w, QWidget *parent)
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

