#pragma once
#ifndef EVERY5WINDOW_H
#define EVERY5WINDOW_H

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

class Every5Window : public QMainWindow
{
	Q_OBJECT

public:
	Every5Window();
	void set_image_marking(MarkingProcess *data);

signals:
	void window_close();

protected:
	void closeEvent(QCloseEvent *event);

private:
	MarkingProcess					*mark_process;
	ImageViewer						*image_viewer;

	QLabel							*ImagewLabel;
	QLabel							*ImagehLabel;
};

#endif
