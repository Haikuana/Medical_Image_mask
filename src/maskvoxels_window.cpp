#include "maskvoxels_window.h"

MaskVoxelsWindow::MaskVoxelsWindow()
{
	mark_process = NULL;
	setWindowIcon(QIcon("Resources"));

	QWidget *widget = new QWidget;
	widget->setStyleSheet("QWidget{background-color: black}");
	setCentralWidget(widget);

	image_viewer = new ImageViewer(MASKVOXELS_WINDOW);
	image_viewer->setAttribute(Qt::WA_OpaquePaintEvent);
	image_viewer->setAttribute(Qt::WA_NoSystemBackground);
	//image_viewer = viewer;

	//CreateStatusBar();

	QGridLayout *layout_main = new QGridLayout;
	layout_main->setContentsMargins(0, 0, 0, 0);
	layout_main->addWidget(image_viewer, 0, 0, -1, -1);

	widget->setLayout(layout_main);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), image_viewer, SLOT(update_whole_viewer()));
	timer->start(1);

	setWindowTitle("3D Segmented Vessel Mask");
}


void MaskVoxelsWindow::set_image_marking(MarkingProcess *data)
{
	mark_process = data;
	image_viewer->set_image_marking(data);
	//mark_process->write_former_slices_tovolume();
}

void MaskVoxelsWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
	emit window_close();
}
