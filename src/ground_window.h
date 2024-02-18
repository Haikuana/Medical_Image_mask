#pragma once
#ifndef REFERENCEWINDOW_H
#define REFERENCEWINDOW_H

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

class QLabel;
class QPushButton;
class QCheckBox;
class QGroupBox;
class QComboBox;

class GroundWindow : public QMainWindow
{
	Q_OBJECT

public:
	GroundWindow();
	void set_image_marking(MarkingProcess *data);

signals:
	void window_close();


protected:
	void closeEvent(QCloseEvent *event);

//private:
	//void CreateLayout();
	//void CreateStatusBar();

	//private slots:
	//void updateStatusBar();

	//void updateDrawImage(bool);
	//void updateDrawImageGrid(bool);
	//void updateDrawImageFeature(bool);

private:
	MarkingProcess					*mark_process;
	ImageViewer						*image_viewer;

	QLabel							*ImagewLabel;
	QLabel							*ImagehLabel;

	QGroupBox						*image_groupBox;
};

#endif
