#pragma once
#ifndef ALLMASKWINDOW_H
#define ALLMASKWINDOW_H

#include "viewer.h"
#include "markingprocess.h"

#include <QString>
#include <QStringList>
#include <QtWidgets>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QBitmap>

#include "vtkStringArray.h"
#include "QVTKWidget.h"
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkPNGReader.h>
#include <vtkvolume16reader.h>
#include <vtkPointData.h>
#include <vtkSLCReader.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include "vtkCubeSource.h"
#include "vtkProperty.h"
#include "vtkLight.h"

#include <vtkCommand.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkCallbackCommand.h>
#include <vtkMath.h>
#include <vtkTimerLog.h>
class QVTKOpenGLWidget;


class QLabel;
class QPushButton;
class QCheckBox;
class QGroupBox;
class QComboBox;

class AllmaskWindow : public QMainWindow
{
	Q_OBJECT

public:
	AllmaskWindow(int image_w, int image_h,int, QString fname);
	void set_image_marking(MarkingProcess *data);
	void rebuild_mask_volume();

signals:
	void window_close();

protected:
	void closeEvent(QCloseEvent *event);
	void rendering_voxels();

private:
	MarkingProcess					*mark_process;
	double								opacity_image1;
	double								opacity_image2;
	double								opacity_mask1;
	double								opacity_mask2;
	int								image_x;
	int								image_y;
	int								size_from;
	int								size_to;

	QString							f_mask_name;

	QWidget *centralwidget;
	QGroupBox *groupBoxGraph;
	QVBoxLayout *vboxLayout;
	QSplitter *splitter;
	QFrame *tableFrame;
	QVBoxLayout *vboxLayout1;

	QVTKWidget *qVTK1;

	vtkSmartPointer<vtkRenderWindow> renWin;
	vtkSmartPointer<vtkRenderer> ren1;

	vtkSmartPointer<vtkVolume> mask_volume;

};

#endif
