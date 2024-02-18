#include "viewer.h"

using namespace std;
using namespace qglviewer;

#define MAX_CHAR       128

ImageViewer::ImageViewer(int window_typ)
{
	window_type = window_typ;

	mark_process = NULL;

	do_draw_image = true;
	do_draw_image_marks = true;

	do_rotate_scene = false;
	do_translation_scene = true;

	do_adding = false;
	do_deleting = false;
	current_index_.clear();

	current_position_ofselect = QPoint(0, 0);
	brush_width = QPoint(1,1);
	brush_size = 2;

	setAutoFillBackground(false);
	bBox = Bbox_3(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
}

ImageViewer::~ImageViewer()
{
	if (!constraints)
	{
		delete constraints;
		constraints = NULL;
	}
}

void ImageViewer::set_scene(Bbox_3 &box)
{
	Point_3 p1(box.xmax(), box.ymax(), box.zmax());
	Point_3 p2(box.xmin(), box.ymin(), box.zmin());
	Vector_3 v(p2 - p1);
	v = v / 2;
	Point_3 center = p1 + v;
	radius = sqrt(v.squared_length());
	setSceneCenter(qglviewer::Vec(center.x(), center.y(), center.z()));
	//setSceneCenter(qglviewer::Vec(0,0,0));
	setSceneRadius(0.7*radius);
	showEntireScene();
	//std::cout << "set scene \n";
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
		QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.end();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//camera()->getProjectedCoordinatesOf();

	if (mark_process)
	{
		if (window_type == OPERATION_WINDOW)
		{
			double q1, q2, q3, q4;
			camera()->frame()->getOrientation(q1, q2, q3, q4);
			vector<double> orient = { q1, q2, q3, q4 };
			double p1, p2, p3;
			camera()->frame()->getPosition(p1, p2, p3);
			vector<double> pos = { p1,p2,p3 };
			vector<vector<double>> pos_orient;
			pos_orient.push_back(pos);
			pos_orient.push_back(orient);
			mark_process->SetCameraPosAndOrientation(pos_orient);
		}
		else if (window_type != ALLMASK_WINDOW && window_type != MASKVOXELS_WINDOW)
		{
			vector<vector<double>>  pos_orientation;
			mark_process->GetCameraPosAndOrientation(pos_orientation);
			if (!pos_orientation.empty())
			{
				camera()->frame()->setPosition(pos_orientation[0][0], pos_orientation[0][1], pos_orientation[0][2]);
				//camera()->frame()->setOrientation(Quaternion(pos_orientation[1][0], pos_orientation[1][1], pos_orientation[1][2], pos_orientation[1][3]));
				do_rotate_scene = false;
				do_translation_scene = false;

				mark_process->is_fixed_model_view() = true;
			}
		}
	}
	
	if (do_rotate_scene || window_type == ALLMASK_WINDOW ||
		window_type == MASKVOXELS_WINDOW)
	{
		constraints->setRotationConstraintType(AxisPlaneConstraint::FREE);
	}
	else
	{
		constraints->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
	}
	if (do_translation_scene)
	{
		constraints->setTranslationConstraintType(AxisPlaneConstraint::FREE);
	}
	else
	{
		constraints->setTranslationConstraintType(AxisPlaneConstraint::FORBIDDEN);
	}

	dir = qglviewer::Vec(0.5, 0.5, 0.5);
	constraints->setRotationConstraintDirection(dir);
	constraints->setTranslationConstraintDirection(dir);

	// Classical 3D drawing, usually performed by paintGL().
	preDraw();
	draw();
	postDraw();

	// Restore OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//swapBuffers();
}


void ImageViewer::draw()
{
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	//draw something
	glDisable(GL_LIGHTING);

	//image
	if (do_draw_image&& mark_process != NULL)
	{
		if (window_type == OPERATION_WINDOW)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			vector<Pixel> ima_1 = mark_process->get_current_ground_slice();
			draw_image(ima_1,0);
			
			set<int> ima_ = mark_process->get_current_marked_pixels();
			draw_pixels(ima_,0);

			glDisable(GL_BLEND);
		}
		else if (window_type == GROUND_WINDOW)
		{
			vector<Pixel> ima_ = mark_process->get_current_ground_slice();
			draw_image(ima_,0);

			set<int> ima_1 = mark_process->get_current_marked_pixels();
			draw_pixels(ima_1,0);
		}
		else if (window_type == EVERY5_WINDOW)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			vector<Pixel> ima_ = mark_process->get_current_every5_slice();
			draw_image(ima_,0);

			set<int> ima_1 = mark_process->get_compound_mask();
			draw_pixels(ima_1,0,1);

			glDisable(GL_BLEND);	
		}
		else if(window_type == MASKVOXELS_WINDOW)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_LIGHTING);
			//glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			//drawSphere( 0.5, 20, 20, 0, 0, 0);
			//draw_3dquad(0, 0, 0, 0.5, 0.5);

			vector<set<int>> former_masks = mark_process->get_former_masks();
			draw_3dpixels(former_masks);

			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_LIGHTING);
		}
	}

	if (selected_pixel_x != -1 && selected_pixel_y != -1)
	{
		double m_dx = IMAGEWIDTHSIZE;
		double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
		double m_dy = pixel_l * image_hei / 2.0;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		double x = -m_dx + (double(selected_pixel_x) + 0.5)*pixel_l;
		double y = -m_dy + (double(selected_pixel_y) + 0.5)*pixel_l;
		draw_quad(x, y, pixel_l);
	}

	if (do_adding || do_deleting || do_automatic_fill)
	{
		//PickVert();
		//PickSquare();
		DrawXORRect();
	}

	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void ImageViewer::init()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Restore previous viewer state.
	restoreStateFromFile();

	setMouseBinding(Qt::ControlModifier, Qt::RightButton, NO_CLICK_ACTION);
	setMouseBinding(Qt::ControlModifier, Qt::LeftButton, NO_CLICK_ACTION);

	constraints = new WorldConstraint();
	camera()->frame()->setConstraint(constraints);
	camera()->setOrientation(Quaternion());
	camera()->setZNearCoefficient(0.0001);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void ImageViewer::keyPressEvent(QKeyEvent *e)
{
	const Qt::KeyboardModifiers modifiers = e->modifiers();
	bool handled = false;
	if ((e->key() == Qt::Key_R) && (modifiers == Qt::ControlModifier))
	{
		handled = true;
		do_rotate_scene = !do_rotate_scene;
	}
	else if ((e->key() == Qt::Key_T) && (modifiers == Qt::ControlModifier))
	{
		handled = true;
		do_translation_scene = !do_translation_scene;
	}
	if (!handled)
	{
		QGLViewer::keyPressEvent(e);
	}

	dir = qglviewer::Vec(0.5, 0.5, 0.5);
	//constraints->setTranslationConstraintDirection(dir);
	constraints->setRotationConstraintDirection(dir);

	update();
}

void ImageViewer::mousePressEvent(QMouseEvent *e)
{
	if (window_type == OPERATION_WINDOW)
	{
		setMouseBinding(Qt::NoModifier, Qt::LeftButton, NO_CLICK_ACTION);
		if ((e->buttons() == Qt::LeftButton) && (e->modifiers() == Qt::NoModifier))
		{
			do_adding = true;
			current_position_ofselect = e->pos();
			PickSquare(brush_size);
			mark_process->add_new_selected_pixels(current_index_);
			//update();
			//repaint();
			repaint(current_position_ofselect.x() - brush_width.x() / 2,
				current_position_ofselect.y() - brush_width.x() / 2, brush_width.x(), brush_width.x());

			return;
		}
	}
	if (window_type == OPERATION_WINDOW
		&& (e->buttons() == Qt::LeftButton) && (e->modifiers() == Qt::ControlModifier))
	{
		do_automatic_fill = true;
		setCursor(Qt::CrossCursor);
		current_position_ofselect = e->pos();
		PickRegion();
		mark_process->add_new_selected_pixels(current_index_);
		update();

		return;
	}
	if (window_type == OPERATION_WINDOW
		&& (e->buttons() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
	{
		do_deleting = true;
		current_position_ofselect = e->pos();
		PickSquare(3 * brush_size);
		mark_process->remove_selected_pixles(current_index_);
		repaint(current_position_ofselect.x() - brush_width.x() / 2,
			current_position_ofselect.y() - brush_width.x() / 2, brush_width.x(), brush_width.x());

		return;
	}

	QGLViewer::mousePressEvent(e);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *e)
{
	if (window_type == OPERATION_WINDOW)
	{
		setMouseBinding(Qt::NoModifier, Qt::LeftButton, NO_CLICK_ACTION);
		if ((e->buttons() == Qt::LeftButton) && (e->modifiers() == Qt::NoModifier))
		{
			do_adding = true;

			setCursor(Qt::CrossCursor);
			current_position_ofselect = e->pos();
			PickSquare(brush_size);
			mark_process->add_new_selected_pixels(current_index_);
			repaint(current_position_ofselect.x() - brush_width.x() / 2,
				current_position_ofselect.y() - brush_width.x() / 2, brush_width.x(), brush_width.x());
			return;
		}
	}
	if (window_type == OPERATION_WINDOW
		&& (e->buttons() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
	{
		do_deleting = true;

		setCursor(Qt::ForbiddenCursor);
		current_position_ofselect = e->pos();
		PickSquare(3 * brush_size);
		//std::cout << current_position_ofselect.x() << current_position_ofselect.y() << std::endl;
		mark_process->remove_selected_pixles(current_index_);
		//repaint();
		repaint(current_position_ofselect.x() - brush_width.x() / 2,
			current_position_ofselect.y() - brush_width.x() / 2, brush_width.x(), brush_width.x());
		return;
	}


	QGLViewer::mouseMoveEvent(e);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *e)
{
	current_position_ofselect = e->pos();

	if (do_automatic_fill)
	{
		do_automatic_fill = false;
		selected_pixel_x = -1;
		selected_pixel_y = -1;
	}

	if (do_adding)
	{
		//test
		/*std::cout << "selected points size:" << current_index_.size() << "consist of ";
		for (int i = 0; i < current_index_.size(); i++)
		{
			std::cout << current_index_[i] << "-";
		}
		std::cout << std::endl;*/
		PickSquare(brush_size);
		mark_process->add_new_selected_pixels(current_index_);

		do_adding = false;
	}

	if (do_deleting)
	{
		PickSquare(3 * brush_size);
		mark_process->remove_selected_pixles(current_index_);
		do_deleting = false;
	}

	setCursor(Qt::ArrowCursor);
	QGLViewer::mouseReleaseEvent(e);
}

void ImageViewer::wheelEvent(QWheelEvent *e)
{
	if (window_type == OPERATION_WINDOW && (e->modifiers() == Qt::ControlModifier))
	{
		QPoint numDegrees = e->angleDelta() / 8;
		if (!numDegrees.isNull()) {
			QPoint numSteps = numDegrees / 15;
			int brush_size_new = brush_size+ numSteps.y();
			brush_size = std::max(brush_size_new, 2);
			brush_size = std::min(brush_size, 8);

			emit mark_process->brush_changed(brush_size);
		}
		update();

		return;
	}

	QGLViewer::wheelEvent(e);
}

void ImageViewer::set_image_marking(MarkingProcess *data)
{
	mark_process = data;
	image_hei = mark_process->get_image_height();
	image_wid = mark_process->get_image_width();
	//OutFileName = mark_process->get_output_filename();

	bBox = Bbox_3(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
	set_scene(bBox);
	update();
}

void ImageViewer::DrawXORRect()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width(), height(), 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_COLOR_LOGIC_OP);
	//glLogicOp(GL_XOR);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_LOOP);
	glVertex2f(current_position_ofselect.x() - brush_width.x() / 2, current_position_ofselect.y() - brush_width.x() / 2);
	glVertex2f(current_position_ofselect.x() + brush_width.x() / 2, current_position_ofselect.y() - brush_width.x() / 2);
	glVertex2f(current_position_ofselect.x() + brush_width.x() / 2, current_position_ofselect.y() + brush_width.x() / 2);
	glVertex2f(current_position_ofselect.x() - brush_width.x() / 2, current_position_ofselect.y() + brush_width.x() / 2);
	glEnd();
	//std::cout << "brush: " << brush_width.x() << "\n";

	//glDisable(GL_LOGIC_OP);

	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void ImageViewer::PickSquare(int brush_size_)
{
	current_index_.clear();

	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;

	//brush width
	double in0[3] = { 0,0,0 };
	double inp[3] = { pixel_l,pixel_l,0 };
	double inc[3] = { -m_dx,-m_dy,0 };
	double out0[3] = { 0,0,0 };
	double outp[3] = { 0,0,0 };
	double outc[3] = { 0,0,0 };
	camera()->getProjectedCoordinatesOf(in0, out0);
	camera()->getProjectedCoordinatesOf(inp, outp);
	camera()->getProjectedCoordinatesOf(inc, outc);

	int width_ = abs(int(outp[0] - out0[0] + 0.5)*brush_size_);
	brush_width = { width_  ,width_ };

	//pick pixels	
	int from_x, to_x,from_y,to_y;
	if (brush_size_ %2 == 0)
	{
		int min_x = int((current_position_ofselect.x() - outc[0]) / (outp[0] - out0[0]) + 0.5);
		int min_y = int((current_position_ofselect.y() - outc[1]) / (outp[1] - out0[1]) + 0.5);

		from_x = min_x-1 - (brush_size_ - 2) / 2;
		to_x = min_x  + (brush_size_ - 2) / 2;
		from_y = min_y-1 - (brush_size_ - 2) / 2;
		to_y = min_y + (brush_size_ - 2) / 2;
	}
	else
	{
		int min_x = int((current_position_ofselect.x() - outc[0]) / (outp[0] - out0[0]));
		int min_y = int((current_position_ofselect.y() - outc[1]) / (outp[1] - out0[1]));

		from_x = min_x - (brush_size_ - 1) / 2;
		to_x = min_x  + (brush_size_ - 1) / 2;
		from_y = min_y - (brush_size_ - 1) / 2;
		to_y = min_y  + (brush_size_ - 1) / 2;
	}
	for (int i = from_x;i<=to_x;i++)
	{
		for (int j = from_y;j<=to_y;j++)
		{
			if (i>=0&&i<image_wid&&j>=0&&j<image_hei)
			{
				int index = i*image_hei + j;
				current_index_.push_back(index);
			}	
		}
	}
}

void ImageViewer::PickRegion()
{
	current_index_.clear();

	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;

	//brush width
	double in0[3] = { 0,0,0 };
	double inp[3] = { pixel_l,pixel_l,0 };
	double inc[3] = { -m_dx,-m_dy,0 };
	double out0[3] = { 0,0,0 };
	double outp[3] = { 0,0,0 };
	double outc[3] = { 0,0,0 };
	camera()->getProjectedCoordinatesOf(in0, out0);
	camera()->getProjectedCoordinatesOf(inp, outp);
	camera()->getProjectedCoordinatesOf(inc, outc);

	int width_ = abs(int(outp[0] - out0[0] + 0.5)*brush_size);
	brush_width = { width_  ,width_ };

	//pick pixels	
	int x = int((current_position_ofselect.x() - outc[0]) / (outp[0] - out0[0]));
	int y = int((current_position_ofselect.y() - outc[1]) / (outp[1] - out0[1]));
	selected_pixel_x = x;
	selected_pixel_y = y;

	cv::Mat image, mask;
	int ffillMode = 1;
	int loDiff = 30, upDiff = 30;
	int connectivity = 4;
	int newMaskVal = 255;

	QString file_ = mark_process->get_current_ground_file_name();
	image = cv::imread(file_.toStdString(), 1);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(128);
	for (int row = 1; row < mask.rows - 1; row++)
	{
		for (int col = 1; col < mask.cols - 1; col++)
		{
			if (image.at<cv::Vec3b>(row - 1, col - 1)[0] > 0 ||
				image.at<cv::Vec3b>(row - 1, col - 1)[1] > 0 ||
				image.at<cv::Vec3b>(row - 1, col - 1)[2] > 0)
			{
				mask.at<char>(row, col) = 0;
			}

		}
	}
	//imshow("mask1", mask);	

	cv::Point seed = cv::Point(x, image_hei - 1 - y);
	//std::cout << "seed" << x << "," << y << "\n";
	int lo = loDiff;
	int up = upDiff;
	int flags = connectivity + (newMaskVal << 8) +
		(ffillMode == 1 ? cv::FLOODFILL_FIXED_RANGE : 0);
	cv::Rect ccomp;

	cv::Scalar newVal = 120;

	floodFill(image, mask, seed, newVal, &ccomp, cv::Scalar(lo, lo, lo),
		cv::Scalar(up, up, up), flags);
	//cv::imshow("mask", mask);

	uchar *edgeImg = mask.data;
	for (int row = 1; row < mask.rows - 1; row++)
	{
		for (int col = 1; col < mask.cols - 1; col++)
		{
			if (edgeImg[row*mask.cols + col] == 255)
			{
				char s = mask.at<char>(row, col);
				//std::cout << "num: " << a << " ";
				int index = (col - 1)*image_hei + (image_hei - row);
				current_index_.push_back(index);
			}

		}
	}
	std::cout << "num selected: " << current_index_.size() << "\n";
	if (current_index_.size() > 1000)
	{
		current_index_.clear();
	}
}


void ImageViewer::draw_image(vector<Pixel> image_,bool is_3d)
{
	if (image_.empty())
	{
		return;
	}
	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0;i<image_.size();i++)
	{
		double x = -m_dx + (double(image_[i].row) + 0.5)*pixel_l;
		double y = -m_dy + (double(image_[i].col) + 0.5)*pixel_l;
		int cor = image_[i].gray_value;
		glColor4d(cor / 255.0, cor / 255.0, cor / 255.0, 1.0);
		draw_quad(x, y, pixel_l);
		
	}
}

void ImageViewer::draw_pixels(set<int> pixels,bool is_3d,int type_)
{
	if (pixels.empty())
	{
		return;
	}

#if 1
	//single color
	set<int> pixels_temp;
	for (auto it = pixels.begin(); it != pixels.end(); it++)
	{
		int id_ = *it % (image_hei*image_wid);
		pixels_temp.insert(id_);
	}
#endif

	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#if 1
	for (auto it = pixels_temp.begin(); it != pixels_temp.end(); it++)
	{
		int id_ = *it;
#else
	for (auto it = pixels.begin(); it != pixels.end(); it++)
	{
		int id_ = *it;
		if (type_ == 1)
		{
			id_ = *it % (image_hei*image_wid);
}
#endif
		double x = -m_dx + (double(id_ / image_hei) + 0.5)*pixel_l;
		double y = -m_dy + (double(id_ % image_hei) + 0.5)*pixel_l;
		glColor4d(0.8, 0.0, 0.0,0.4);
		draw_quad(x, y, pixel_l);
	}
	//int k = 0;
	//for (unsigned int i = 0; i < lw; i++)
	//{
	//	double x = -m_dx + (double(i) + 0.5)*pixel_l;
	//	for (unsigned int j = 0; j < lh; j++)
	//	{
	//		double y = -m_dy + (double(j) + 0.5)*pixel_l;
	//		list<int>::iterator res = std::find(pixels.begin(),pixels.end(),k);
	//		if (res != pixels.end())
	//		{
	//			glColor3d(0,0.8,0);
	//			draw_quad(x, y, pixel_l);
	//		}
	//		k++;
	//	}
	//}
}

void ImageViewer::draw_quad(double x, double y, double pisel_l)
{
	glBegin(GL_POLYGON);
	glVertex3d(x - pisel_l * 0.5, y - pisel_l * 0.5, 0.0);
	glVertex3d(x + pisel_l * 0.5, y - pisel_l * 0.5, 0.0);
	glVertex3d(x + pisel_l * 0.5, y + pisel_l * 0.5, 0.0);
	glVertex3d(x - pisel_l * 0.5, y + pisel_l * 0.5, 0.0);
	glEnd();
}

void ImageViewer::draw_3dpixels(vector<set<int>> pixels)
{
	if (pixels.empty())
	{
		return;
	}

	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;
	double z_pixel_l = 4.0*pixel_l;
	glPolygonMode(GL_FRONT, GL_FILL);
	for (int i = 0; i < pixels.size(); i++)
	{
		double half_boxZ = pixels.size()*z_pixel_l*0.5;
		double z = i*z_pixel_l + 0.5*z_pixel_l - half_boxZ;
		for (auto it = pixels[i].begin(); it != pixels[i].end(); it++)
		{
			double x = -m_dx + (double(*it / image_hei) + 0.5)*pixel_l;
			double y = -m_dy + (double(*it % image_hei) + 0.5)*pixel_l;
			glColor3d(0.8, 0.0, 0.0);
			draw_3dquad(x, y, z, pixel_l, z_pixel_l);
			//drawSphere(pixel_l/2.0,10,10,x,y,z);
		}
	}
}

void ImageViewer::drawSphere(double r, int lats, int longs,
	double fx, double fy, double fz)
{
	int i, j;
	for (i = 0; i <= lats; i++)
	{
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++)
		{
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(fx + r*x * zr0, fy + r*y * zr0, fz + r*z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(fx + r*x * zr1, fy + r*y * zr1, fz + r*z1);
		}
		glEnd();
	}
}

void ImageViewer::draw_3dquad(double x, double y, double z,
	double pixel_l, double z_pixels_l)
{
#if 1
	glBegin(GL_QUADS);
	glColor3d(0.8, 0.0, 0.0);
	glNormal3f(0.0F, 0.0F, -1.0F);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);

	glNormal3f(0.0F, 0.0F, 1.0F);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);

	glColor3d(0.6, 0.0, 0.0);
	glNormal3f(-1.0F, 0.0F, 0.0F);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);

	glNormal3f(1.0F, 0.0F, 0.0F);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);

	glColor3d(0.4, 0.0, 0.0);
	glNormal3f(0.0F, -1.0F, 0.0F);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y - pixel_l * 0.5, z + z_pixels_l*0.5);

	glNormal3f(0.0F, 1.0F, 0.0F);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z - z_pixels_l*0.5);
	glVertex3d(x + pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);
	glVertex3d(x - pixel_l * 0.5, y + pixel_l * 0.5, z + z_pixels_l*0.5);
	glEnd();
#endif
}


void ImageViewer::set_brush_size(int a)
{
	if (!mark_process)
	{
		return;
	}

	brush_size = a;
	update();
}

void ImageViewer::update_whole_viewer()
{
	if (!mark_process)
	{
		return;
	}
	repaint();
}

void ImageViewer::check_image_view(bool bv)
{
	if (!mark_process)
	{
		return;
	}
	do_draw_image = bv;
	update();
}

void ImageViewer::check_image_feature_view(bool bv)
{
	if (!mark_process)
	{
		return;
	}
	do_draw_image_marks = bv;
	update();
}

