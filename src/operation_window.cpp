#include "operation_window.h"

OperationWindow::OperationWindow()
{
	mark_process = NULL;
	setWindowIcon(QIcon("Resources"));

	QWidget *widget = new QWidget;
	widget->setStyleSheet("QWidget{background-color: black}");
	setCentralWidget(widget);

	image_viewer = new ImageViewer(OPERATION_WINDOW);
	image_viewer->setAttribute(Qt::WA_OpaquePaintEvent);
	image_viewer->setAttribute(Qt::WA_NoSystemBackground);
	//image_viewer = viewer;

	QGridLayout *layout_main = new QGridLayout;
	//image_groupBox->setMaximumWidth(300);
	//brush_groupBox->setMaximumWidth(300);
	//layout_main->addWidget(image_groupBox, 0, 0);
	//layout_main->addWidget(brush_groupBox, 0, 1);
	layout_main->setContentsMargins(0, 0, 0, 0);
	layout_main->addWidget(image_viewer,0,0,-1,-1);
	//layout_main->setRowStretch(0, 1);
	//layout_main->setRowStretch(1, 10);

	//layout_main->setColumnStretch(0, 1);
	//layout_main->setColumnStretch(1, 3);
	//layout_main->setColumnStretch(2, 10);

	widget->setLayout(layout_main);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), image_viewer, SLOT(update_whole_viewer()));
	timer->start(1);

	setWindowTitle("Slice");

}

void OperationWindow::set_image_marking(MarkingProcess *data)
{
	mark_process = data;
	//updateStatusBar();
	image_viewer->set_image_marking(data);
}

void OperationWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
	emit window_close();
}

//void OperationWindow::CreateStatusBar()
//{
//	ImagewLabel = new QLabel();
//	statusBar()->addPermanentWidget(ImagewLabel);
//
//	updateStatusBar();
//}

//void OperationWindow::updateStatusBar()
//{
//	if (mark_process != NULL)
//	{
//		QString size_ = "image size: ";
//		QImage *image_ = mark_process->get_input_image();
//		if (!image_)
//			return;
//		int width = image_->width();
//		int height = image_->height();
//		if (height == -1)
//		{
//			size_ = "";
//		}
//		else
//		{
//			size_ += QString("%1x%2").arg(width).arg(height);
//		}
//		ImagewLabel->setText(size_);
//	}
//}

//void OperationWindow::updateDrawImage(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image", 3000);
//	}
//}
//
//void OperationWindow::updateDrawImageGrid(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image grid", 3000);
//	}
//}
//
//void OperationWindow::updateDrawImageFeature(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image feature", 3000);
//	}
//}