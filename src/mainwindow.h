#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "ui_mainwindow.h"

#include "operation_window.h"
#include "ground_window.h"
#include "every5_window.h"
#include "allmask_window.h"
#include "maskvoxels_window.h"

#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>

class vtkQtTableView;
class QLabel;
class QPushButton;
class QCheckBox;
class QGroupBox;
class QComboBox;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();
private:
	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateLayout();

	private slots:
	void read_image();
	void write_current_slice();
	void write_all_slices();

	void set_window_visible(bool bVisible);
	void operation_window_close();
	void ground_window_close();
	void every5_window_close();
	void allmask_window_close();
	void maskvoxels_window_close();

	private slots:
	void setLineEditValue(int);
	void setLineEditValueInv(int);
	void setSliceLineEditValue(int);
	void rebuild_mask_volume();

	void setCompoundLineEditValue(int);
	void updateCompoundNumber();

private:
	Ui::mainwindowClass ui;

	QMenu							*fileMenu;
	QMenu							*filefMenu;
	QMenu							*viewMenu;

	QToolBar						*viewTool;
	QGroupBox						*image_groupBox;

	QToolBar						*brushTool;
	QGroupBox						*brush_groupBox;
	QLineEdit						*lineEdit;
	QSlider							*slider;

	QToolBar						*compoundTool;
	QGroupBox						*compound_groupBox;
	QLineEdit						*compound_lineEdit;
	QSlider							*compound_slider;
	int								former_compoundsize;

	QToolBar						*sliceTool;
	QGroupBox						*slice_groupBox;
	QLineEdit						*slice_lineEdit;
	QSlider							*slice_slider;
	int								slice_size;

	QAction							*openimageAction;
	QAction							*writeCsliceAction;
	QAction							*writesliceAction;

	QAction							*operation_window_Action;
	QAction							*ground_window_Action;
	QAction							*allmask_window_Action;
	QAction							*every5_window_Action;
	QAction							*maskvoxels_window_Action;

	QMdiArea						*mdi;
	OperationWindow					*operation_window;
	GroundWindow					*ground_window;
	AllmaskWindow					*allmask_window;
	Every5Window					*every5_window;
	MaskVoxelsWindow				*maskvoxels_window;

	MarkingProcess					*mark_process;
};

#endif // MAINWINDOW_H

