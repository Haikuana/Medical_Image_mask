#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

int main(int argc, char *argv[])
{
#if 0
	//test
#if 1
	std::string out = "E:/Projects/mask-image/teaser/normal/";
	std::string normal_mask = "E:/Projects/mask-image/teaser/normal/ours_wsu_030_seg/micro/";
	std::string normal_imageHR = "E:/Projects/mask-image/teaser/normal/images/wsu_030_HR_ori/";
	std::string normal_imageLR = "E:/Projects/mask-image/teaser/normal/images/wsu_030_LR_ori/";
	std::string normal_hl = "E:/Projects/mask-image/teaser/normal/highlights/hipMask_030/";
	std::string normal_box = "E:/Projects/mask-image/teaser/normal/box/";
	std::string normal_gt = "E:/Projects/mask-image/teaser/normal/GT_HR/";
#else
	std::string out = "E:/Projects/mask-image/teaser/tumor/";
	std::string tumor_mask = "E:/Projects/mask-image/teaser/tumor/Results/Seg_results/";
	std::string tumor_imageHR = "E:/Projects/mask-image/teaser/tumor/HR images/HR_images_ori/";
	std::string tumor_hl1 = "E:/Projects/mask-image/teaser/tumor/highlights/lesion_mask_acc/";
	std::string tumor_hl2 = "E:/Projects/mask-image/teaser/tumor/highlights/lesion_mask_acc_2/";
	std::string tumor_box = "E:/Projects/mask-image/teaser/tumor/box/";

	std::map<int, bool> hldata;
	{
		std::string hlr = tumor_hl1;
		vector<pair<QString, int>>	groundslice_files;
		std::vector<string> ground_slice_file;
		getFiles(hlr + "*.png", ground_slice_file);
		for (int i = 0; i < ground_slice_file.size(); i++)
		{
			QString filename_ = QString::fromStdString(ground_slice_file[i]);
			int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
			QString fullname = QString::fromStdString(hlr);
			fullname.append(filename_);
			groundslice_files.push_back(pair<QString, int>(fullname, index));
		}
		sort(groundslice_files.begin(), groundslice_files.end(), sort_qstringpair_secondgreater);

		for (int i = 0; i < groundslice_files.size(); i++)
		{
			QImage im_;
			std::vector<Pixel> sparse_im;
			im_.load(groundslice_files[i].first);

			int check_value;
			if (qGray(im_.pixel(0, 0)) == 0)
			{
				check_value = 0;
			}
			else
			{
				check_value = 255;
			}
			for (int wid = 0; wid < im_.width(); wid++)
			{
				for (int hei = 0; hei < im_.height(); hei++)
				{
					int gray_ = qGray(im_.pixel(wid, im_.height() - 1 - hei));

					int id_ = i * im_.width()*im_.height() + wid * im_.height() + hei;

					if (gray_ != check_value)
					{
						hldata[id_] = true;
					}
				}
			}
		}
	}

#endif

	std::string target = normal_gt;

	vector<pair<QString, int>>	groundslice_files;
	std::vector<string> ground_slice_file;
	getFiles(target +"*.png", ground_slice_file);
	for (int i = 0; i < ground_slice_file.size(); i++)
	{
		QString filename_ = QString::fromStdString(ground_slice_file[i]);
		int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
		QString fullname = QString::fromStdString(target);
		fullname.append(filename_);
		groundslice_files.push_back(pair<QString, int>(fullname, index));
	}
	sort(groundslice_files.begin(), groundslice_files.end(), sort_qstringpair_secondgreater);

	QImage im_;
	std::vector<Pixel> sparse_im;
	im_.load(groundslice_files[0].first);
	int w = im_.width(), h = im_.height(), d = groundslice_files.size();

	std::string filen_ = out + "data.vtk";
	std::ofstream fo(filen_);
	fo << "# vtk DataFile Version 5.1 \n"
		<< "disField\n"
		<< "ASCII\n"
		<< "DATASET STRUCTURED_POINTS\n"
		<< "DIMENSIONS " << h << " " << w << " " << d << "\n"
		<< "SPACING  1 1 3\n" 
		<< "ORIGIN 0 0 0\n" 
		<< "POINT_DATA " << w*h*d << "\n"
		<< "SCALARS dis float\n"
		<< "LOOKUP_TABLE default\n";

	//to match the surface rendering
	//<< "SPACING 0.001642 0.001642 0.004926\n"  //1.0/new_width; ; *3;
	//<< "ORIGIN -0.662579 -0.815482 -0.133790\n"
	//0.97*{ -(old-width/2)*spacing-x; -(old-height/2)*spacing-y; }
	//-(56/2)*spacing-z

	for (int i = 0; i < groundslice_files.size(); i++)
	{		
		QImage im_;
		std::vector<Pixel> sparse_im;
		im_.load(groundslice_files[i].first);

		int check_value;
		if (qGray(im_.pixel(0, 0)) == 0)
		{
			check_value = 0;
		}
		else
		{
			check_value = 255;
		}

		for (int wid = 0; wid < im_.width(); wid++)
		{
			for (int hei = 0; hei < im_.height(); hei++)
			{
				int gray_ = qGray(im_.pixel(wid, im_.height() - 1 - hei));

				int id_ = i * im_.width()*im_.height() + wid * im_.height() + hei;

				//highlight
				/*if (hldata.find(id_) != hldata.end() && gray_ != check_value)
					gray_ = 255;
				else
					gray_ = 0;*/

				//layer
				//if ( i > 21)
				//if (i < 22 || i > 38)
				//if (i < 39 )
				{
					//gray_ = 0;
				}

				//half
				//if (wid > w / 2)
				//	gray_ = 0;			

				//inverse for background images
				//gray_ = 255 - gray_;			

				fo << gray_ << " ";
			}
		}
	}
	fo << "\n";

	std::cout << "done\n";
#else

	QApplication a(argc, argv);
	QFont font = a.font();
	font.setPointSize(10);
	a.setFont(font);
	QApplication::setStyle("fusion");
	MainWindow w;
	w.show();
	return a.exec();

#endif
}
