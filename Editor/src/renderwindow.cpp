/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "renderwindow.h"
#include "mainwindow.h"
#include <scene/scene.h>
#include <scene/sceneobject.h>
#include <scene/components/camera.h>
#include <scene/renderer.h>
#include <opengl/glrenderer.h>
#include <iomanip>
#include <sstream>
#include <string>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStyle>
#include <QDesktopServices>

// Pads a number with leading zeroes
std::string ZeroPadNumber(int num, unsigned int width = 5) {
    std::ostringstream ss;
    ss << std::setw(width) << std::setfill('0') << num;
    std::string result = ss.str();
    if (result.length() > width) {
        result.erase(0, result.length() - width);
    }
    return result;
}

RenderWindow::RenderWindow(QWidget *parent) :
    QDockWidget("Render View", parent),
    rendering_(false),
    first_view_(true)
{
    QWidget* new_render_widget = new QWidget(parent);
    new_render_widget->setMinimumSize(400,300);
    new_render_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* new_layout = new QVBoxLayout();
    new_layout->setMargin(0);
    new_layout->setSpacing(0);
    new_render_widget->setLayout(new_layout);

    QScrollArea* container = new QScrollArea(new_render_widget);
    container->setWidget(&render_view_);
    new_layout->addWidget(container);
    setWidget(new_render_widget);
    connect(this, &RenderWindow::topLevelChanged, &render_view_, &RenderView::ContextChanged);
    hide();
}

RenderWindow::~RenderWindow() {
}

void RenderWindow::closeEvent(QCloseEvent *event) {
    // Asynchronously closes the window and notifies rendering thread to stop
    rendering_ = false;
    QDockWidget::closeEvent(event);
    render_view_.Cancel();
}

int RenderWindow::exec(Scene& scene, const AnimationSettings& settings)
{
    switch (settings.Mode)
    {
    case AnimationSettings::AS_NORMAL:
        return exec_normal(scene, settings);
        break;

    case AnimationSettings::AS_DIFF:
        return exec_diff(scene, settings);
        break;

    default:
        break;
    }

    return 0;
}

int RenderWindow::exec_normal(Scene& scene, const AnimationSettings& settings) {
    setWindowTitle(QString::fromStdString("Rendering complete!"));
    return 0;
}


int RenderWindow::exec_diff(Scene& scene, const AnimationSettings& settings)
{
    return 0;
}