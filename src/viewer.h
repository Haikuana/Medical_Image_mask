#pragma once
#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "markingprocess.h"
#include <QGLWidget>
#include <QPaintEvent>
#include <QVector3D>
#include <qglviewer.h>
#include <manipulatedCameraFrame.h>

class ImageViewer :public QGLViewer
{
	Q_OBJECT
public:
	ImageViewer(int);
	~ImageViewer();

	void set_image_marking(MarkingProcess *data);
	void set_brush_size(int);

protected:
	virtual void draw();
	virtual void init();
	virtual void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	virtual void keyPressEvent(QKeyEvent *);

private:
	void set_scene(Bbox_3 &box);

	void draw_image(vector<Pixel> image_,bool);
	void draw_pixels(set<int> pixels, bool,int type_ = 0);//type 1 for 3d index pixel
	void draw_quad(double x, double y, double pisel_l);

	void draw_3dpixels(vector<set<int>> pixels);
	void draw_3dquad(double x, double y, double z, double pisel_l, double zpisel_l);
	void drawSphere(double r, int lats, int longs, double x, double y, double z);

	void DrawXORRect();
	void PickSquare(int );
	void PickRegion();

	public slots:
	void update_whole_viewer();
	void check_image_view(bool);
	void check_image_feature_view(bool);

private:
	MarkingProcess					*mark_process;
	QString							OutFileName;

	int								image_wid;
	int								image_hei;
	bool							do_draw_image;
	bool							do_draw_image_marks;

	//viewer
	int								window_type;
	bool							do_rotate_scene;
	bool							do_translation_scene;
	qglviewer::AxisPlaneConstraint	*constraints;
	qglviewer::Vec					dir;
	double							radius;
	Bbox_3							bBox;

	//select
	vector<int>						current_index_;
	QPoint							current_position_ofselect;
	int								brush_size;//number of pixels
	QPoint							brush_width;
	bool							do_adding;
	bool							do_deleting;

	bool							do_automatic_fill;

	int								selected_pixel_x;
	int								selected_pixel_y;
};

#endif