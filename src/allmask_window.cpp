#include "allmask_window.h"

AllmaskWindow::AllmaskWindow(int image_w,int image_h,int size_slices,QString fname)
{
	setWindowIcon(QIcon("Resources"));

	vtkObject::GlobalWarningDisplayOff();

	opacity_image1 = 900;
	opacity_image2 = 2000;//3000
	opacity_mask1 = 800;
	opacity_mask2 = 2000;//2000
	image_x = image_w;//IMAGE SIZE
	image_y = image_h;//IMAGE SIZE
	size_from = 0;
	size_to = size_slices-1;//NUMBER OF SLICES -1
	mark_process = NULL;
	f_mask_name = fname;
	//mark_process->write_former_slices_tovolume();

	centralwidget = new QWidget();
	centralwidget->setObjectName(QStringLiteral("centralwidget"));

	qVTK1 = new QVTKWidget(centralwidget);
	qVTK1->setObjectName(QStringLiteral("qVTK1"));

	QGridLayout *layout_main = new QGridLayout;
	layout_main->setContentsMargins(0, 0, 0, 0);
	layout_main->addWidget(qVTK1, 0, 0, -1, -1);

	centralwidget->setLayout(layout_main);

	setCentralWidget(centralwidget);

#if 0
	// image reader;
	vtkSmartPointer<vtkSLCReader> reader = vtkSmartPointer<vtkSLCReader>::New();
	reader->SetFileName("vm_foot.slc");
	reader->Update();
#endif
#if 0
	//image reader2
	vtkSmartPointer<vtkImageReader2> reader = vtkSmartPointer<vtkImageReader2>::New();
	vtkStringArray* files = vtkStringArray::New();
	
	vector<QString> outs_ = mark_process->get_file_names();
	vtkStdString *vtkstrings_ = new vtkStdString[outs_.size()];
	for (int i = 0;i<outs_.size();i++)
	{
		vtkstrings_[i] = (vtkStdString(outs_[i].toStdString()));
	}
	files->SetArray(vtkstrings_, outs_.size(),1);
	std::cout << "input finished" << std::endl;

	reader->SetFileNames(files);
	reader->Update();
	// transfer data from reader to image data;
	imageData->ShallowCopy(reader->GetOutput());
	std::cout << imageData->GetNumberOfPoints() << std::endl;
	delete[] vtkstrings_;
#endif

#if 0
	QString dirname = QFileDialog::getExistingDirectory(this,
		tr("Select a Directory"),QDir::currentPath());
	if (dirname.isNull())
		return;

	QString pattern(dirname += "/pixGT_");

	std::cout << "generateVolume before" << std::endl;
	vtkSmartPointer< vtkVolume > spVolume = generateVolume(pattern);
	std::cout << "generateVolume finished" << std::endl;
#endif

	renWin = vtkSmartPointer<vtkRenderWindow>::New();
	ren1 = vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.0, 0.0, 0.0);

	renWin->AddRenderer(ren1);
	//renWin->SetSize(1000, 650);
	//renWin->DebugOff();

	mask_volume = vtkSmartPointer<vtkVolume>::New();

	qVTK1->SetRenderWindow(renWin);
	qVTK1->GetRenderWindow()->AddRenderer(ren1);

	renWin->Render(); // make sure we have an OpenGL context.

	//iren->Start();

	setWindowTitle("3D Whole Brain Volume Rendering");
}

void AllmaskWindow::rendering_voxels()
{
#if 0
	int image_wid = mark_process->get_image_width();
	int image_hei = mark_process->get_image_height();
	double m_dx = IMAGEWIDTHSIZE;
	double pixel_l = 2.0*IMAGEWIDTHSIZE / image_wid;
	double m_dy = pixel_l * image_hei / 2.0;

	vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
	cube->SetXLength(1.0);
	cube->SetYLength(1.0);
	cube->SetZLength(1.0);

	vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cubeMapper->SetInputConnection(cube->GetOutputPort());

	vector<list<int>> former_masks = mark_process->get_former_masks();
	for (int i = 0;i<former_masks.size();i++)
	{
		for (auto it = former_masks[i].begin();it != former_masks[i].end();it++)
		{
			vtkSmartPointer<vtkActor> cubea = vtkSmartPointer<vtkActor>::New();
			cubea->SetMapper(cubeMapper);
			cubea->GetProperty()->SetColor(0, 0, 1);
			cubea->GetProperty()->SetOpacity(1.0);
			cubea->AddPosition(*it/image_hei + 1, *it % image_hei +1, i+1);

			ren1->AddActor(cubea);
		}
	}
#endif
	
	vtkSmartPointer<vtkPNGReader> pngreader =
		vtkSmartPointer<vtkPNGReader>::New();
	pngreader->SetFilePrefix(f_mask_name.toLatin1());
	pngreader->SetFilePattern("%s%d.png");
	pngreader->SetFileNameSliceOffset(1);
	pngreader->SetFileNameSliceSpacing(1);
	pngreader->SetDataSpacing(0.5, 0.5, 2.0);
	pngreader->SetDataOrigin(0, 0, 0);
	pngreader->SetDataExtent(0, image_x-1, 0, image_y-1, size_from, size_to);
	pngreader->SetDataScalarTypeToUnsignedChar();
	pngreader->Update();

	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	imageData->ShallowCopy(pngreader->GetOutput());
	std::cout << imageData->GetNumberOfPoints() << std::endl;

	// The volume will be displayed by ray-cast alpha compositing.
	// A ray-cast mapper is needed to do the ray-casting.
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	//volumeMapper->SetInputConnection(pngreader->GetOutputPort());
	volumeMapper->SetInputData(imageData);

	////////////////////////////////////////////////////////////////////
	//property
	// The color transfer function maps voxel intensities to colors.
	// It is modality-specific, and often anatomy-specific as well.
	// The goal is to one color for flesh (between 500 and 1000)
	// and another color for bone (1150 and over).
	// The opacity transfer function is used to control the opacity
	// of different tissue types.
	vtkSmartPointer<vtkColorTransferFunction>volumeColor =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	volumeColor->AddRGBPoint(0, 1.0, 0.0, 0.0);
	volumeColor->AddRGBPoint(opacity_mask1, 1.0, 0.0, 0.0);
	volumeColor->AddRGBPoint(opacity_mask2, 1.0, 0.0, 0.0);

	vtkSmartPointer<vtkPiecewiseFunction> volumeScalarOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeScalarOpacity->AddPoint(0, 0.2);
	volumeScalarOpacity->AddPoint(opacity_mask1, 0.4);
	volumeScalarOpacity->AddPoint(opacity_mask2, 0.8);

	// The gradient opacity function is used to decrease the opacity
	// in the "flat" regions of the volume while maintaining the opacity
	// at the boundaries between tissue types.  The gradient is measured
	// as the amount by which the intensity changes over unit distance.
	// For most medical data, the unit distance is 1mm.
	vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeGradientOpacity->AddPoint(0, 0.0);
	volumeGradientOpacity->AddPoint(opacity_mask1, 0.2);
	volumeGradientOpacity->AddPoint(opacity_mask2, 0.45);

	// The VolumeProperty attaches the color and opacity functions to the
	// volume, and sets other volume properties.  The interpolation should
	// be set to linear to do a high-quality rendering.  The ShadeOn option
	// turns on directional lighting, which will usually enhance the
	// appearance of the volume and make it look more "3D".  However,
	// the quality of the shading depends on how accurately the gradient
	// of the volume can be calculated, and for noisy data the gradient
	// estimation will be very poor.  The impact of the shading can be
	// decreased by increasing the Ambient coefficient while decreasing
	// the Diffuse and Specular coefficient.  To increase the impact
	// of shading, decrease the Ambient and increase the Diffuse and Specular.
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(volumeColor);
	volumeProperty->SetScalarOpacity(volumeScalarOpacity);
	volumeProperty->SetGradientOpacity(volumeGradientOpacity);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(0.8);
	volumeProperty->SetDiffuse(0.3);
	volumeProperty->SetSpecular(0.1);
	volumeProperty->SetSpecularPower(0);
	volumeProperty->SetScalarOpacityUnitDistance(1.0/opacity_mask2);

	// The vtkVolume is a vtkProp3D (like a vtkActor) and controls the position
	// and orientation of the volume in world coordinates.
	mask_volume->SetMapper(volumeMapper);
	mask_volume->SetProperty(volumeProperty);
	//mask_volume->AddPosition(768, 1024, 0);
	std::cout << "bounds: " << mask_volume->GetMinXBound() << "-" << mask_volume->GetMaxXBound() << ","
		<< mask_volume->GetMinYBound() << "-" << mask_volume->GetMaxYBound() << ","
		<< mask_volume->GetMinZBound() << "-" << mask_volume->GetMaxZBound() << "\n";

	ren1->AddActor(mask_volume);
}

void AllmaskWindow::set_image_marking(MarkingProcess *data)
{
	mark_process = data;

	int index_from_, index_to;
	//vector<Pixel>

	vtkSmartPointer<vtkPNGReader> pngreader =
		vtkSmartPointer<vtkPNGReader>::New();
	pngreader->SetFilePrefix("Input/Generated/Volumevtk/Slice_");
	pngreader->SetFilePattern("%s%d.png");
	pngreader->SetFileNameSliceOffset(1);
	pngreader->SetFileNameSliceSpacing(1);
	pngreader->SetDataSpacing(0.5, 0.5, 2.0);
	pngreader->SetDataOrigin(0, 0, 0);
	pngreader->SetDataExtent(0, image_x, 0, image_y, size_from, size_to);
	pngreader->SetDataScalarTypeToUnsignedChar();
	pngreader->Update();

	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	imageData->ShallowCopy(pngreader->GetOutput());
	std::cout << imageData->GetNumberOfPoints() << std::endl;

	// The volume will be displayed by ray-cast alpha compositing.
	// A ray-cast mapper is needed to do the ray-casting.
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	//volumeMapper->SetInputConnection(pngreader->GetOutputPort());
	volumeMapper->SetInputData(imageData);
	volumeMapper->SetBlendModeToComposite();
	//vtk::GenerateValues();

	////////////////////////////////////////////////////////////////////
	//property
	// The color transfer function maps voxel intensities to colors.
	// It is modality-specific, and often anatomy-specific as well.
	// The goal is to one color for flesh (between 500 and 1000)
	// and another color for bone (1150 and over).
	// The opacity transfer function is used to control the opacity
	// of different tissue types.
	vtkSmartPointer<vtkColorTransferFunction>volumeColor =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	volumeColor->AddRGBPoint(0, 0.0, 0.0, 0.0);
	volumeColor->AddRGBPoint(opacity_image1, 0.1, 0.1, 0.1);
	volumeColor->AddRGBPoint(opacity_image2, 0.2, 0.2, 0.2);

	vtkSmartPointer<vtkPiecewiseFunction> volumeScalarOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeScalarOpacity->AddPoint(0, 0.0);
	volumeScalarOpacity->AddPoint(opacity_image1, 0.1);
	volumeScalarOpacity->AddPoint(opacity_image2, 0.2);

	// The gradient opacity function is used to decrease the opacity
	// in the "flat" regions of the volume while maintaining the opacity
	// at the boundaries between tissue types.  The gradient is measured
	// as the amount by which the intensity changes over unit distance.
	// For most medical data, the unit distance is 1mm.
	vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeGradientOpacity->AddPoint(0, 0.0);
	volumeGradientOpacity->AddPoint(opacity_image1, 0.1);
	volumeGradientOpacity->AddPoint(opacity_image2, 0.2);

	// The VolumeProperty attaches the color and opacity functions to the
	// volume, and sets other volume properties.  The interpolation should
	// be set to linear to do a high-quality rendering.  The ShadeOn option
	// turns on directional lighting, which will usually enhance the
	// appearance of the volume and make it look more "3D".  However,
	// the quality of the shading depends on how accurately the gradient
	// of the volume can be calculated, and for noisy data the gradient
	// estimation will be very poor.  The impact of the shading can be
	// decreased by increasing the Ambient coefficient while decreasing
	// the Diffuse and Specular coefficient.  To increase the impact
	// of shading, decrease the Ambient and increase the Diffuse and Specular.
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(volumeColor);
	volumeProperty->SetScalarOpacity(volumeScalarOpacity);
	volumeProperty->SetGradientOpacity(volumeGradientOpacity);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(0.8);
	volumeProperty->SetDiffuse(0.3);
	volumeProperty->SetSpecular(0.01);
	volumeProperty->SetSpecularPower(0);
	volumeProperty->SetScalarOpacityUnitDistance(1.0/opacity_image2);

	// The vtkVolume is a vtkProp3D (like a vtkActor) and controls the position
	// and orientation of the volume in world coordinates.
	vtkSmartPointer<vtkVolume> volume =
		vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	//volume->AddPosition(0, 0, 0);
	std::cout << "bounds: " << volume->GetMinXBound() << "-" << volume->GetMaxXBound() << ","
		<< volume->GetMinYBound() << "-" << volume->GetMaxYBound() << ","
		<< volume->GetMinZBound() << "-" << volume->GetMaxZBound() << "\n";

	//vtkSmartPointer<vtkRenderWindowInteractor> iren =
	//vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//iren->SetRenderWindow(renWin);

	//volumeMapper->SetRequestedRenderModeToRayCast();

	rendering_voxels();
	ren1->AddActor(volume);

	ren1->ResetCamera();

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	light->SetFocalPoint(1.875, 0.6125, 0);
	light->SetPosition(0.875, 1.6125, 1.7);

	vtkSmartPointer<vtkLight> light1 = vtkSmartPointer<vtkLight>::New();
	light1->SetLightTypeToCameraLight();
	//light1->SetPosition(0.875, 1.6125, 1);

	vtkSmartPointer<vtkLight> light2 = vtkSmartPointer<vtkLight>::New();
	light2->SetLightTypeToSceneLight();
	//light2->SetPosition(0.875, 1.6125, 1);

	vtkSmartPointer<vtkLight> light3 = vtkSmartPointer<vtkLight>::New();
	light3->SetFocalPoint(-0.6125, -0.875, 1.875);
	light3->SetPosition(1.875, 0.6125, -2.5);

	vtkSmartPointer<vtkLight> light4 = vtkSmartPointer<vtkLight>::New();
	light4->SetFocalPoint(0.6125, -0.875, 0);
	light4->SetPosition(-10.875, 0.6125, 0);

	ren1->AddLight(light);
	ren1->AddLight(light1);
	ren1->AddLight(light2);
	ren1->AddLight(light3);
	ren1->AddLight(light4);

	// 3D texture mode. For coverage.
#if !defined(VTK_LEGACY_REMOVE) && !defined(VTK_OPENGL2)
	volumeMapper->SetRequestedRenderModeToRayCastAndTexture();
#endif // VTK_LEGACY_REMOVE

	//ren1->SetAllocatedRenderTime(100.0);
	//std::cout << "rendering" << std::endl;
	renWin->Render();
	//image_viewer->set_image_marking(data);
}

void AllmaskWindow::rebuild_mask_volume()
{
	mark_process->write_all_slices();
	//mark_process->write_slices_tovolume();
	ren1->RemoveActor(mask_volume);

	vector<string> operation_premax_file;
	getFiles("./Input/SubMask/*.png", operation_premax_file);
	if (!operation_premax_file.empty()) {
		QString filename_ = QString::fromStdString(operation_premax_file[0]);
		QString new_ = filename_.split("/").last().split(".").at(0).split("_").first();
		QString fullname("Input/SubMask/");
		fullname.append(new_);
		fullname.append("_");
		vector<pair<QString, int>>	mask_files;
		for (int i = 0; i < operation_premax_file.size(); i++)
		{
			filename_ = QString::fromStdString(operation_premax_file[i]);
			int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
			QString fullname("Input/SubMask/");
			fullname.append(filename_);
			mask_files.push_back(pair<QString, int>(fullname, index));
		}
		sort(mask_files.begin(), mask_files.end(), sort_qstringpair_secondgreater);

		int size_slices = operation_premax_file.size();
		size_from = mask_files.begin()->second;
		size_to = mask_files.rbegin()->second-1;//NUMBER OF SLICES -1
		f_mask_name = fullname;
	}

	rendering_voxels();

	ren1->AddActor(mask_volume);
	ren1->ResetCamera();
	renWin->Render();
}

void AllmaskWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
	emit window_close();
}
