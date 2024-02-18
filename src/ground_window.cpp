#include "ground_window.h"

GroundWindow::GroundWindow()
{
	mark_process = NULL;
	setWindowIcon(QIcon("Resources"));

	QWidget *widget = new QWidget;
	widget->setStyleSheet("QWidget{background-color: black}");
	setCentralWidget(widget);

	image_viewer = new ImageViewer(GROUND_WINDOW);
	image_viewer->setAttribute(Qt::WA_OpaquePaintEvent);
	image_viewer->setAttribute(Qt::WA_NoSystemBackground);
	//image_viewer = viewer;

	//CreateLayout();
	//CreateStatusBar();

	QGridLayout *layout_main = new QGridLayout;
	//image_groupBox->setFixedWidth(200);
	//layout_main->addWidget(image_groupBox, 0, 0);
	layout_main->setContentsMargins(0, 0, 0, 0);
	layout_main->addWidget(image_viewer, 0, 0, -1, -1);
	//layout_main->setRowStretch(0, 1);
	//layout_main->setRowStretch(1, 5);

	widget->setLayout(layout_main);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), image_viewer, SLOT(update_whole_viewer()));
	timer->start(1);

	setWindowTitle("Slice with Solid Label");
}

//void GroundWindow::CreateLayout()
//{
//	int length = 75;
//	int width = 55;
//
//	//display box
//	image_groupBox = new QGroupBox(tr("Input Image"));
//	QGraphicsDropShadowEffect *e2 = new QGraphicsDropShadowEffect(this);
//	e2->setOffset(2, 2);
//	e2->setBlurRadius(0.5);
//	image_groupBox->setGraphicsEffect(e2);
//
//	QCheckBox *image_ = new QCheckBox(tr("image"));
//	image_->setChecked(false);
//	image_->setStyleSheet("QCheckBox{background:rgb(250,250,250);font:bold;font-size:14px;border-radius:3px}");
//	QGraphicsDropShadowEffect *e5 = new QGraphicsDropShadowEffect(this);
//	e5->setOffset(2, 2);
//	e5->setBlurRadius(0.5);
//	image_->setGraphicsEffect(e5);
//	image_->setFixedSize(length, width);
//	image_->setIcon(QIcon("Resources/imageicon.png"));
//	connect(image_, SIGNAL(toggled(bool)), image_viewer, SLOT(check_image_view(bool)));
//	connect(image_, SIGNAL(toggled(bool)), this, SLOT(updateDrawImage(bool)));
//
//	QCheckBox *image_g = new QCheckBox(tr("image grid"));
//	image_g->setChecked(false);
//	image_g->setStyleSheet("QCheckBox{background:rgb(250,250,250);font:bold;font-size:14px;border-radius:3px}");
//	QGraphicsDropShadowEffect *e4 = new QGraphicsDropShadowEffect(this);
//	e4->setOffset(2, 2);
//	e4->setBlurRadius(0.5);
//	image_g->setGraphicsEffect(e4);
//	image_g->setFixedSize(length, width);
//	image_g->setIcon(QIcon("Resources/grids.png"));
//	connect(image_g, SIGNAL(toggled(bool)), image_viewer, SLOT(check_image_grids_view(bool)));
//	connect(image_g, SIGNAL(toggled(bool)), this, SLOT(updateDrawImageGrid(bool)));
//
//	QCheckBox *image_f = new QCheckBox(tr("f"));
//	image_f->setChecked(false);
//	image_f->setStyleSheet("QCheckBox{background:rgb(250,250,250);font:bold;font-size:14px;border-radius:3px}");
//	QGraphicsDropShadowEffect *e14 = new QGraphicsDropShadowEffect(this);
//	e14->setOffset(2, 2);
//	e14->setBlurRadius(0.5);
//	image_f->setGraphicsEffect(e14);
//	image_f->setWhatsThis(tr("image feature"));
//	image_f->setFixedSize(length, width);
//	image_f->setIcon(QIcon("Resources/feature.png"));
//	connect(image_f, SIGNAL(toggled(bool)), image_viewer, SLOT(check_image_feature_view(bool)));
//	connect(image_f, SIGNAL(toggled(bool)), this, SLOT(updateDrawImageFeature(bool)));
//
//	QGridLayout *dis_lay = new QGridLayout;
//	dis_lay->addWidget(image_, 0, 0);
//	dis_lay->addWidget(image_g, 0, 1);
//	dis_lay->addWidget(image_f, 1, 0);
//	//dis_lay->addStretch(5);
//	image_groupBox->setLayout(dis_lay);
//}

void GroundWindow::set_image_marking(MarkingProcess *data)
{
	mark_process = data;
	//updateStatusBar();
	image_viewer->set_image_marking(data);
}

void GroundWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
	emit window_close();
}

//void GroundWindow::CreateStatusBar()
//{
//	ImagewLabel = new QLabel();
//	statusBar()->addPermanentWidget(ImagewLabel);
//
//	updateStatusBar();
//}

//void GroundWindow::updateStatusBar()
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

//void GroundWindow::updateDrawImage(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image", 3000);
//	}
//}
//
//void GroundWindow::updateDrawImageGrid(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image grid", 3000);
//	}
//}
//
//void GroundWindow::updateDrawImageFeature(bool bv)
//{
//	if (bv)
//	{
//		statusBar()->showMessage("draw image feature", 3000);
//	}
//}