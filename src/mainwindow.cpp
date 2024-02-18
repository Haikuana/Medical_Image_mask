#include "mainwindow.h"
#include <QtGui>
#include <QtWidgets/QFileDialog>


MainWindow::MainWindow()
{
	ui.setupUi(this);
	setStyleSheet("QWidget{background-image:url(Resources/3.png)}");

	mdi = new QMdiArea; 
	setCentralWidget(mdi);
	mdi->cascadeSubWindows();

	QScreen *screen = QGuiApplication::primaryScreen();
	QRect mm = screen->availableGeometry();
	int screen_width = mm.width();
	int screen_height = mm.height();
	//std::cout << screen_width << "-" << screen_height << "\n";

	//setGeometry(20, 20, 1900, 1000);
	this->setGeometry(0.02*screen_width, 0.04*screen_height, 0.95*screen_width, 0.95*screen_height);

	//showFullScreen();

	//pre-read
	int image_w, image_h;
	QString foutname;
	vector<string> operation_premax_file;
	getFiles("./Input/Mask/*.png", operation_premax_file);
	QString filename_ = QString::fromStdString(operation_premax_file[0]);
	QImage im_;
	im_.load(filename_);
	image_w = im_.width();
	image_h = im_.height();
	QString new_ = filename_.split("/").last().split(".").at(0).split("_").first();
	QString fullname("Input/Mask/");
	fullname.append(new_);
	fullname.append("_");
	slice_size = operation_premax_file.size();
	foutname = fullname;

	mark_process = NULL;
	former_compoundsize = 5;

	CreateActions();
	CreateMenus();
	CreateLayout();
	CreateToolBars();

	every5_window = new Every5Window();
	//every5_window->setMaximumSize(0.95*0.3*screen_width, 0.3*screen_height);
	every5_window->setAttribute(Qt::WA_DeleteOnClose);
	mdi->addSubWindow(every5_window);

	ground_window = new GroundWindow();
	//ground_window->setMaximumSize(0.95*0.3*screen_width, 0.3*screen_height);
	ground_window->setAttribute(Qt::WA_DeleteOnClose);
	mdi->addSubWindow(ground_window);

	allmask_window = new AllmaskWindow(image_w,image_h,slice_size,foutname);
	//allmask_window->setMaximumSize(0.95*0.3*screen_width, 0.3*screen_height);
	allmask_window->setAttribute(Qt::WA_DeleteOnClose);
	mdi->addSubWindow(allmask_window);

	maskvoxels_window = new MaskVoxelsWindow();
	//maskvoxels_window->setMaximumSize(0.95*0.3*screen_width, 0.3*screen_height);
	maskvoxels_window->setAttribute(Qt::WA_DeleteOnClose);
	mdi->addSubWindow(maskvoxels_window);

	operation_window = new OperationWindow();
	//operation_window->setMinimumSize(0.4*screen_width, 0.4*screen_height);
	operation_window->setAttribute(Qt::WA_DeleteOnClose);
	//operation_window->setMinimumSize(0.5*screen_width, 0.5*screen_height);
	mdi->addSubWindow(operation_window);

	mdi->tileSubWindows();
	/*QGridLayout *window_lay = new QGridLayout;
	window_lay->addWidget(operation_window, 0, 0,2,1);
	window_lay->addWidget(maskvoxels_window, 0, 1);
	window_lay->addWidget(allmask_window, 0, 2);
	window_lay->addWidget(ground_window, 1, 1);
	window_lay->addWidget(every5_window, 1, 2);
	window_lay->setColumnStretch(0, 3);
	window_lay->setColumnStretch(1, 2);
	window_lay->setColumnStretch(2, 2);
	mdi->setLayout(window_lay);*/

	connect(operation_window, SIGNAL(window_close()), this, SLOT(operation_window_close()));
	connect(ground_window, SIGNAL(window_close()), this, SLOT(ground_window_close()));
	connect(allmask_window, SIGNAL(window_close()), this, SLOT(allmask_window_close()));
	connect(every5_window, SIGNAL(window_close()), this, SLOT(every5_window_close()));
	connect(maskvoxels_window, SIGNAL(window_close()), this, SLOT(maskvoxels_window_close()));

	setWindowTitle("Cerebral Vasculature Labeling Tool");
	setWindowIcon(QIcon("Resources/windowicon.png"));
}

MainWindow::~MainWindow()
{
	if (mark_process)
		delete mark_process;
}

void MainWindow::CreateActions()
{
	//file
	openimageAction = new QAction(tr("Read data..."), this);
	openimageAction->setIcon(QIcon("Resources/open.png"));
	openimageAction->setStatusTip(tr("Open an existing mesh file"));
	connect(openimageAction, SIGNAL(triggered()), this, SLOT(read_image()));

	writeCsliceAction = new QAction(tr("Save Current Slice..."), this);
	writeCsliceAction->setIcon(QIcon("Resources/save.png"));
	//writeCsliceAction->setStatusTip(tr("write an existing mesh file"));
	connect(writeCsliceAction, SIGNAL(triggered()), this, SLOT(write_current_slice()));

	writesliceAction = new QAction(tr("Save All Slices..."), this);
	writesliceAction->setIcon(QIcon("Resources/save.png"));
	connect(writesliceAction, SIGNAL(triggered()), this, SLOT(write_all_slices()));

	//window
	operation_window_Action = new QAction(tr("Operation Window"), this);
	operation_window_Action->setCheckable(true);
	operation_window_Action->setStatusTip(tr("Show or hide the Operation window's"));
	connect(operation_window_Action, SIGNAL(toggled(bool)), this, SLOT(set_window_visible(bool)));

	ground_window_Action = new QAction(tr("Ground Window"), this);
	ground_window_Action->setCheckable(true);
	ground_window_Action->setStatusTip(tr("Show or hide the spline basis function window's"));
	connect(ground_window_Action, SIGNAL(toggled(bool)), this, SLOT(set_window_visible(bool)));

	allmask_window_Action = new QAction(tr("Allmask Window"), this);
	allmask_window_Action->setCheckable(true);
	allmask_window_Action->setStatusTip(tr("Show or hide the spline basis function window's"));
	connect(allmask_window_Action, SIGNAL(toggled(bool)), this, SLOT(set_window_visible(bool)));

	every5_window_Action = new QAction(tr("Every5 Window"), this);
	every5_window_Action->setCheckable(true);
	every5_window_Action->setStatusTip(tr("Show or hide the spline basis function window's"));
	connect(every5_window_Action, SIGNAL(toggled(bool)), this, SLOT(set_window_visible(bool)));

	maskvoxels_window_Action = new QAction(tr("3DMask Window"), this);
	maskvoxels_window_Action->setCheckable(true);
	maskvoxels_window_Action->setStatusTip(tr("Show or hide the spline basis function window's"));
	connect(maskvoxels_window_Action, SIGNAL(toggled(bool)), this, SLOT(set_window_visible(bool)));
}

void MainWindow::CreateMenus()
{
	fileMenu = menuBar()->addMenu(tr("Read"));
	fileMenu->addAction(openimageAction);

	fileMenu = menuBar()->addMenu(tr("Write"));
	fileMenu->addAction(writeCsliceAction);
	fileMenu->addAction(writesliceAction);
}

void MainWindow::CreateLayout()
{
	int length = 80;
	int width = 35;

	//display box
	image_groupBox = new QGroupBox(tr("View"));
	QGraphicsDropShadowEffect *e2 = new QGraphicsDropShadowEffect(this);
	e2->setOffset(2, 2);
	e2->setBlurRadius(0.5);
	image_groupBox->setGraphicsEffect(e2);

	QCheckBox *image_ = new QCheckBox(tr("Image"));
	image_->setChecked(false);
	image_->setStyleSheet("QCheckBox{background:rgb(250,250,250);font:bold;font-size:14px;border-radius:3px}");
	QGraphicsDropShadowEffect *e5 = new QGraphicsDropShadowEffect(this);
	e5->setOffset(2, 2);
	e5->setBlurRadius(0.5);
	image_->setGraphicsEffect(e5);
	image_->setFixedSize(length, width);
	//image_->setIcon(QIcon("Resources/imageicon.png"));
	//connect(image_, SIGNAL(toggled(bool)), image_viewer, SLOT(check_image_view(bool)));
	//connect(image_, SIGNAL(toggled(bool)), this, SLOT(updateDrawImage(bool)));

	QCheckBox *image_f = new QCheckBox(tr("Mask"));
	image_f->setChecked(false);
	image_f->setStyleSheet("QCheckBox{background:rgb(250,250,250);font:bold;font-size:14px;border-radius:3px}");
	QGraphicsDropShadowEffect *e14 = new QGraphicsDropShadowEffect(this);
	e14->setOffset(2, 2);
	e14->setBlurRadius(0.5);
	image_f->setGraphicsEffect(e14);
	image_f->setWhatsThis(tr("image feature"));
	image_f->setFixedSize(length, width);
	//image_f->setIcon(QIcon("Resources/feature.png"));
	//connect(image_f, SIGNAL(toggled(bool)), image_viewer, SLOT(check_image_feature_view(bool)));
	//connect(image_f, SIGNAL(toggled(bool)), this, SLOT(updateDrawImageFeature(bool)));

	QGridLayout *dis_lay = new QGridLayout;
	dis_lay->setContentsMargins(0, 0, 0, 2);
	dis_lay->addWidget(image_, 0, 0);
	dis_lay->addWidget(image_f, 0, 1);
	image_groupBox->setLayout(dis_lay);

	////////////////////////////////////////////////////
	//brush size box
	brush_groupBox = new QGroupBox(tr("Brush Size"));
	QGraphicsDropShadowEffect *e7 = new QGraphicsDropShadowEffect(this);
	e7->setOffset(2, 2);
	e7->setBlurRadius(0.5);
	brush_groupBox->setGraphicsEffect(e7);

	lineEdit = new QLineEdit("2 Pixels", this);
	lineEdit->setAlignment(Qt::AlignCenter);
	lineEdit->setMaximumWidth(200);
	lineEdit->setStyleSheet("QLineEdit{font:bold;font-size:14px;color:rgb(62, 70, 240);border:1px solid rgb(62,70,200);border-radius:4px;selection-color:pink}");
	QGraphicsDropShadowEffect *e3 = new QGraphicsDropShadowEffect(this);
	e3->setOffset(1, 1);
	e3->setBlurRadius(0.5);
	e3->setColor(QColor(62, 70, 240));
	lineEdit->setGraphicsEffect(e3);
	lineEdit->setReadOnly(true);

	slider = new QSlider(Qt::Horizontal, this);
	slider->setMaximumWidth(350);
	slider->setMinimum(1);
	slider->setMaximum(8);
	slider->setValue(2);
	slider->setCursor(QCursor(Qt::ClosedHandCursor));
	slider->setStyleSheet("QSlider{background:rgb(250,250,250);border-radius:3px}");
	QGraphicsDropShadowEffect *e16 = new QGraphicsDropShadowEffect(this);
	e16->setOffset(1, 1);
	e16->setBlurRadius(0.5);
	e16->setColor(QColor(62, 70, 240));
	slider->setGraphicsEffect(e16);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue(int)));

	QGridLayout *brush_lay = new QGridLayout;
	brush_lay->setContentsMargins(0, 0, 0, 2);
	brush_lay->addWidget(lineEdit, 0, 0);
	brush_lay->addWidget(slider, 0, 1, 1, 3);
	brush_lay->setColumnStretch(0, 1);
	brush_lay->setColumnStretch(1, 2);
	brush_groupBox->setLayout(brush_lay);

	////////////////////////////////////////////////////
	//slice box
	slice_groupBox = new QGroupBox(tr("Slice Index"));
	QGraphicsDropShadowEffect *e007 = new QGraphicsDropShadowEffect(this);
	e007->setOffset(2, 2);
	e007->setBlurRadius(0.5);
	slice_groupBox->setGraphicsEffect(e007);

	slice_lineEdit = new QLineEdit("Current: 10", this);
	slice_lineEdit->setAlignment(Qt::AlignCenter);
	slice_lineEdit->setMaximumWidth(200);
	slice_lineEdit->setStyleSheet("QLineEdit{font:bold;font-size:14px;color:rgb(62, 70, 240);border:1px solid rgb(62,70,200);border-radius:4px;selection-color:pink}");
	QGraphicsDropShadowEffect *e003 = new QGraphicsDropShadowEffect(this);
	e003->setOffset(1, 1);
	e003->setBlurRadius(0.5);
	e003->setColor(QColor(62, 70, 240));
	slice_lineEdit->setGraphicsEffect(e003);
	slice_lineEdit->setReadOnly(true);

	slice_slider = new QSlider(Qt::Horizontal, this);
	slice_slider->setMinimumWidth(450);
	slice_slider->setMinimum(1);
	slice_slider->setMaximum(slice_size);
	slice_slider->setValue(10);
	slice_slider->setCursor(QCursor(Qt::ClosedHandCursor));
	slice_slider->setStyleSheet("QSlider{background:rgb(250,250,250);border-radius:3px}");
	QGraphicsDropShadowEffect *e016 = new QGraphicsDropShadowEffect(this);
	e016->setOffset(1, 1);
	e016->setBlurRadius(0.5);
	e016->setColor(QColor(62, 70, 240));
	slice_slider->setGraphicsEffect(e016);
	connect(slice_slider, SIGNAL(valueChanged(int)), this, SLOT(setSliceLineEditValue(int)));

	QPushButton *compound_ = new QPushButton(tr("Volume Refresh"), this);
	QGraphicsDropShadowEffect *e38 = new QGraphicsDropShadowEffect(this);
	e38->setOffset(2, 2);
	e38->setBlurRadius(0.5);
	compound_->setMaximumWidth(200);
	compound_->setGraphicsEffect(e38);
	compound_->setCursor(QCursor(Qt::ClosedHandCursor));
	compound_->setStyleSheet("QPushButton{background:rgb(200,200,200);font: Microsoft YaHei;font-size:15px;color:rgb(55,100,255);\
	border:1px solid rgb(55,100,255);border-radius:3px}");
	connect(compound_, SIGNAL(clicked()), this, SLOT(rebuild_mask_volume()));

	QGridLayout *slice_lay = new QGridLayout;
	slice_lay->setContentsMargins(0, 0, 0, 2);
	slice_lay->addWidget(slice_lineEdit, 0, 0);
	slice_lay->addWidget(slice_slider, 0, 1, 1, 6);
	slice_lay->addWidget(compound_, 0, 7);
	slice_lay->setColumnStretch(0, 1);
	slice_lay->setColumnStretch(1, 5);
	slice_lay->setColumnStretch(2, 2);
	slice_groupBox->setLayout(slice_lay);

	////////////////////////////////////////////////////
	//compound box
	compound_groupBox = new QGroupBox(tr("Number of MIP Slices"));
	QGraphicsDropShadowEffect *e017 = new QGraphicsDropShadowEffect(this);
	e017->setOffset(2, 2);
	e017->setBlurRadius(0.5);
	compound_groupBox->setGraphicsEffect(e017);

	compound_lineEdit = new QLineEdit("5", this);
	compound_lineEdit->setAlignment(Qt::AlignCenter);
	compound_lineEdit->setMaximumWidth(200);
	compound_lineEdit->setStyleSheet("QLineEdit{font:bold;font-size:14px;color:rgb(62, 70, 240);border:1px solid rgb(62,70,200);border-radius:4px;selection-color:pink}");
	QGraphicsDropShadowEffect *e013 = new QGraphicsDropShadowEffect(this);
	e013->setOffset(1, 1);
	e013->setBlurRadius(0.5);
	e013->setColor(QColor(62, 70, 240));
	compound_lineEdit->setGraphicsEffect(e013);
	compound_lineEdit->setReadOnly(true);

	compound_slider = new QSlider(Qt::Horizontal, this);
	compound_slider->setMaximumWidth(450);
	compound_slider->setMinimum(2);
	compound_slider->setMaximum(20);
	compound_slider->setValue(5);
	compound_slider->setCursor(QCursor(Qt::ClosedHandCursor));
	compound_slider->setStyleSheet("QSlider{background:rgb(250,250,250);border-radius:3px}");
	QGraphicsDropShadowEffect *e026 = new QGraphicsDropShadowEffect(this);
	e026->setOffset(1, 1);
	e026->setBlurRadius(0.5);
	e026->setColor(QColor(62, 70, 240));
	compound_slider->setGraphicsEffect(e026);
	connect(compound_slider, SIGNAL(valueChanged(int)), this, SLOT(setCompoundLineEditValue(int)));

	QPushButton *update_ = new QPushButton(tr("Update"), this);
	QGraphicsDropShadowEffect *e308 = new QGraphicsDropShadowEffect(this);
	e308->setOffset(2, 2);
	e308->setBlurRadius(0.5);
	update_->setMaximumWidth(150);
	update_->setGraphicsEffect(e308);
	update_->setCursor(QCursor(Qt::ClosedHandCursor));
	update_->setStyleSheet("QPushButton{background:rgb(200,200,200);font: Microsoft YaHei;font-size:15px;color:rgb(55,100,255);\
	border:1px solid rgb(55,100,255);border-radius:3px}");
	connect(update_, SIGNAL(clicked()), this, SLOT(updateCompoundNumber()));

	QGridLayout *compound_lay = new QGridLayout;
	compound_lay->setContentsMargins(0, 0, 0, 2);
	compound_lay->addWidget(compound_lineEdit, 0, 0);
	compound_lay->addWidget(compound_slider, 0, 1, 1, 6);
	compound_lay->addWidget(update_, 0, 7);
	compound_lay->setColumnStretch(0, 1);
	compound_lay->setColumnStretch(1, 5);
	compound_lay->setColumnStretch(2, 2);
	compound_groupBox->setLayout(compound_lay);
}

void MainWindow::read_image()
{
	if (!mark_process)
		mark_process = new MarkingProcess();
	else
	{
		delete mark_process;
		mark_process = new MarkingProcess();
	}
	mark_process->load_allimage();

	connect(mark_process, SIGNAL(brush_changed(int)), this, SLOT(setLineEditValueInv(int)));

	//std::cout << __FUNCTION__ << ": process images done" << endl;
	operation_window->set_image_marking(mark_process);
	ground_window->set_image_marking(mark_process);
	allmask_window->set_image_marking(mark_process);
	every5_window->set_image_marking(mark_process);
	maskvoxels_window->set_image_marking(mark_process);
	return;
}


void MainWindow::setSliceLineEditValue(int)
{
	int pos = slice_slider->value();
	QString str = QString("Current: %1 ").arg(pos);
	slice_lineEdit->setText(str);

	if (mark_process != NULL)
	{
		mark_process->set_current_slice_index(pos - 1);
	}
}

void MainWindow::setLineEditValue(int)
{
	int pos = slider->value();
	QString str = QString("%1 Pixels").arg(pos);
	lineEdit->setText(str);

	operation_window->viewer()->set_brush_size(pos);
}

void MainWindow::setLineEditValueInv(int size_)
{
	QString str = QString("%1 Pixels").arg(size_);
	lineEdit->setText(str);
	slider->setValue(size_);
}

void MainWindow::rebuild_mask_volume()
{
	if (mark_process != NULL)
	{
		allmask_window->rebuild_mask_volume();
	}
}

void MainWindow::setCompoundLineEditValue(int)
{
	int pos = compound_slider->value();
	QString str = QString("%1 ").arg(pos);
	compound_lineEdit->setText(str);
}

void MainWindow::updateCompoundNumber()
{
	int pos = compound_slider->value();
	if (mark_process != NULL)
	{
		if (pos == former_compoundsize)
		{
			mark_process->update_compound_mask(pos);
		}
		else
		{
			mark_process->update_compound_size(pos);
			mark_process->update_compound_mask(pos);
		}
		former_compoundsize = pos;
	}
}

void MainWindow::CreateToolBars()
{
	//this->toolButtonStyle();
	brushTool = this->addToolBar(tr("Brush Size"));
	brushTool->addWidget(brush_groupBox);
	brushTool->setFloatable(false);

	compoundTool = this->addToolBar(tr("Number of MIP Slices"));
	compoundTool->addWidget(compound_groupBox);
	compoundTool->setFloatable(false);

	sliceTool = this->addToolBar(tr("Slice Index"));
	sliceTool->addWidget(slice_groupBox);
	sliceTool->setFloatable(false);
}

void MainWindow::write_current_slice()
{
	if (!mark_process)
	{
		return;
	}
	mark_process->write_current_slice();
}

void MainWindow::write_all_slices()
{
	if (!mark_process)
	{
		return;
	}
	mark_process->write_all_slices();
}

void MainWindow::set_window_visible(bool bVisible)
{
	QObject *send = sender();
	if (send == operation_window_Action)
	{
		mdi->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(operation_window));
	}
	else if (send == ground_window_Action)
	{
		mdi->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(ground_window));
	}
	else if (send == allmask_window_Action)
	{
		mdi->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(allmask_window));
	}
	else if (send == every5_window_Action)
	{
		mdi->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(every5_window));
	}
	else
	{
		mdi->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(maskvoxels_window));
	}
}

void MainWindow::operation_window_close()
{
	delete operation_window;
	operation_window = NULL;
}

void MainWindow::ground_window_close()
{
	delete ground_window;
	ground_window = NULL;
}

void MainWindow::allmask_window_close()
{
	delete allmask_window;
	allmask_window = NULL;
}

void MainWindow::every5_window_close()
{
	delete every5_window;
	every5_window = NULL;
}

void MainWindow::maskvoxels_window_close()
{
	delete maskvoxels_window;
	maskvoxels_window = NULL;
}


