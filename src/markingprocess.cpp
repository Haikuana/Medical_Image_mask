#include "markingprocess.h"
#include <QtWidgets/QMessageBox>

MarkingProcess::MarkingProcess()
{
	image_width = 0;
	image_height = 0;
	current_slice_index = 9;
	compound_size = 5;
	is_fixed_modelview = true;
}

MarkingProcess::~MarkingProcess()
{
}

//bool MarkingProcess::read_image(const QString &name)
//{
//	fileName = name;
//	QString filename = name;
//	if (!filename.isNull())
//	{
//		input_image = new QImage;
//		if (input_image->load(filename))
//		{
//			cout << "image open down - size: " << input_image->width() << ' ' << input_image->height() << endl;
//		}
//		else
//		{
//			cout << "image open wrong" << endl;
//			return false;
//		}
//	}
//
//	//construct file
//	out_file_name = "Results/";
//	out_file_name.append(fileName.split("/").last().split(".").at(0));
//	generate_output_directory(out_file_name);
//
//	return true;
//}

void MarkingProcess::load_allimage()
{
	vector<string> ground_slice_file;
	getFiles("./Input/Slice/*.png", ground_slice_file);
	for (int i = 0; i < ground_slice_file.size(); i++)
	{
		QString filename_ = QString::fromStdString(ground_slice_file[i]);
		int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
		QString fullname("Input/Slice/");
		fullname.append(filename_);
		groundslice_files.push_back(pair<QString, int>(fullname, index));
	}
	sort(groundslice_files.begin(), groundslice_files.end(), sort_qstringpair_secondgreater);

	vector<string> every5_MaxIP_file;
	bool had_compound_slice = getFiles("./Input/Generated/Compound/*.png", every5_MaxIP_file);
	if (!had_compound_slice)
	{
		QString out_file = "./Input/Generated";
		generate_output_directory(out_file);
		out_file = "./Input/Generated/Compound";
		generate_output_directory(out_file);
		out_file = "./Input/Generated/Volumevtk";
		generate_output_directory(out_file);
		std::cout <<__FUNCTION__<< ": require re-computing compound slices\n";
	}	
	else
	{
		std::cout << __FUNCTION__ << ": compound slices are already existing\n";
	}
	for (int i = 0; i < every5_MaxIP_file.size(); i++)
	{
		QString filename_ = QString::fromStdString(every5_MaxIP_file[i]);
		int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
		QString fullname("Input/Generated/Compound/");
		fullname.append(filename_);
		compoundslice_files.push_back(pair<QString, int>(fullname, index));
	}
	sort(compoundslice_files.begin(), compoundslice_files.end(), sort_qstringpair_secondgreater);

	vector<string> operation_premax_file;
	getFiles("./Input/Mask/*.png", operation_premax_file);
	for (int i = 0; i < operation_premax_file.size(); i++)
	{
		QString filename_ = QString::fromStdString(operation_premax_file[i]);
		int index = filename_.split("/").last().split(".").at(0).split("_").last().toInt();
		QString fullname("Input/Mask/");
		fullname.append(filename_);
		mask_files.push_back(pair<QString, int>(fullname, index));
	}
	sort(mask_files.begin(), mask_files.end(), sort_qstringpair_secondgreater);

	for (int i = 0;i<mask_files.size();i++)
	{
		mask_files_string.push_back(mask_files[i].first);
	}

	ground_slice.resize(groundslice_files.size());
	masks.resize(groundslice_files.size());
	every5_group.resize(groundslice_files.size());

	vector<int> max_gray;
	max_gray.resize(groundslice_files.size());
#pragma omp parallel for
	for (int i = 0;i<groundslice_files.size();i++)
	{
		int max_g = 0;
		QImage im_;
		vector<Pixel> sparse_im;
		im_.load(groundslice_files[i].first);
		for (int wid = 0;wid<im_.width();wid++)
		{
			for (int hei = 0;hei<im_.height();hei++)
			{
				int gray_ = qGray(im_.pixel(wid, im_.height()-1-hei));
				if (gray_ != 0)
				{
					if (gray_>max_g)
					{
						max_g = gray_;
					}

					Pixel p; p.row = wid; p.col = hei; p.gray_value = gray_;
					sparse_im.push_back(p);
				}
			}
		}
		ground_slice[i]=(sparse_im);
		max_gray[i] = max_g;

		if (image_height == 0 || image_width == 0)
		{
			image_height = im_.height();
			image_width = im_.width();
		}

		QImage im_1;
		set<int> sparse_im1;
		im_1.load(mask_files[i].first);
		int check_value;
		if (qGray(im_1.pixel(0, 0)) == 0)
		{
			check_value = 0;
		}
		else
		{
			check_value = 255;
		}
		for (int wid = 0; wid < im_1.width(); wid++)
		{
			for (int hei = 0; hei < im_1.height(); hei++)
			{
				int gray_ = qGray(im_1.pixel(wid, im_.height() - 1 - hei));
				if (gray_ != check_value)
				{
					int index_ = wid*im_.height() +  hei;
					sparse_im1.insert(index_);
				}
			}
		}
		masks[i] = sparse_im1;
		if (had_compound_slice)
		{
			QImage im_2;
			vector<Pixel> sparse_im2;
			im_2.load(compoundslice_files[i].first);
			for (int wid = 0; wid < im_2.width(); wid++)
			{
				for (int hei = 0; hei < im_2.height(); hei++)
				{
					int gray_ = qGray(im_2.pixel(wid, im_.height() - 1 - hei));
					if (gray_ != 0)
					{
						Pixel p; p.row = wid; p.col = hei; p.gray_value = gray_;
						sparse_im2.push_back(p);
					}
				}
			}
			every5_group[i] = (sparse_im2);
		}
	}

	double threshold_ = 0.0;
	for (int i = 0;i<max_gray.size();i++)
	{
		threshold_ += max_gray[i];
	}
	threshold_ = threshold_ / max_gray.size();

#pragma omp parallel for
	for (int i = 0;i<ground_slice.size();i++)
	{
		for (int j = 0;j<ground_slice[i].size();j++)
		{
			double temp = ground_slice[i][j].gray_value*1.0*255.0 / threshold_;
			ground_slice[i][j].gray_value = std::min(int(temp),255);
		}
	}

	std::cout << __FUNCTION__ << ": load images done\n";

	QImage im_t;
	if (!mask_files.empty())
	{
		im_t.load(mask_files[0].first);
		if (qGray(im_t.pixel(0, 0)) == 0) {
			write_all_slices();
		}
	}
	write_slices_tovolume();
	std::cout << __FUNCTION__ << ": compound slices done\n";

	if (!had_compound_slice)
	{
		update_compound_size(compound_size);
		std::cout << __FUNCTION__ << ": compound slices done\n";
	}

	//update compound mask
	compound_marked_pixels.resize(masks.size());
	update_compound_mask(compound_size);
	std::cout << __FUNCTION__ << ": compound masks done\n";
}

vector<QString> &MarkingProcess::get_file_names()
{
	return mask_files_string;
}

QString MarkingProcess::get_current_ground_file_name()
{
	return groundslice_files[current_slice_index].first;
}

void MarkingProcess::write_current_slice()
{
	QImage out_(image_width,image_height,QImage::Format_RGB16);
	out_.fill(qRgb(255, 255, 255));
	for (auto i = masks[current_slice_index].begin();i!= masks[current_slice_index].end();i++)
	{
		int x_ = *i / image_height;
		int y_ =  *i % image_height;
		out_.setPixel(x_, image_height - 1 - y_, qRgb(0,0,0));
	}
	out_.save(mask_files[current_slice_index].first);
}

void MarkingProcess::write_all_slices()
{
	for (int i = 0;i<masks.size();i++)
	{
		QImage out_(image_width, image_height, QImage::Format_RGB16);
		out_.fill(qRgb(255, 255, 255));
		for (auto it = masks[i].begin(); it != masks[i].end(); it++)
		{
			int x_ = *it / image_height;
			int y_ = *it % image_height;
			out_.setPixel(x_, image_height - 1 - y_, qRgb(0,0,0));
		}
		out_.save(mask_files[i].first);
	}
}

vector<set<int>> MarkingProcess::get_former_masks()
{
	vector<set<int>> former_masks;
	if (current_slice_index >= 0 && current_slice_index < masks.size())
	{
		for (int i = 0; i < current_slice_index+1; i++)
		{
			former_masks.push_back(masks[i]);
		}
	}

	return former_masks;
}

void MarkingProcess::write_slices_tovolume()
{
#pragma omp parallel for
	for (int i = 0; i < ground_slice.size(); i++)
	{
		QString filenameout = "Input/Generated/Volumevtk/Slice_";
		filenameout.append(QString("%1.png").arg(i + 1));

		QImage out_(image_width, image_height, QImage::Format_RGB16);
		out_.fill(qRgb(0, 0, 0));
		for (auto it = ground_slice[i].begin(); it != ground_slice[i].end(); it++)
		{
			out_.setPixel(it->row, image_height -1- it->col,
				qRgb(it->gray_value, it->gray_value, it->gray_value));
		}
		out_.save(filenameout);

		//QImage im_;
		//im_.load(filenamein);
		//
		//QImage im_out = im_.convertToFormat(QImage::Format_RGB32);

		//for (auto it = masks[i].begin(); it != masks[i].end(); it++)
		//{
		//	int x = *it / im_.height();
		//	int y = *it % im_.height();

		//	im_out.setPixelColor(x, im_.height() - 1 - y, QColor(255, 255, 255));
		//}		
	}
}

int MarkingProcess::get_image_width()
{
	return image_width;
}

int MarkingProcess::get_image_height()
{
	return image_height;
}

set<int>	&MarkingProcess::get_compound_mask()
{
	if (current_slice_index >= 0 && current_slice_index < compound_marked_pixels.size())
	{
		return compound_marked_pixels[current_slice_index];
	}
	return compound_marked_pixels[0];
}

void MarkingProcess::set_current_slice_index(int a)
{
	current_slice_index = a;
}

void MarkingProcess::update_compound_size(int size_)
{
	int former, later;
	if (size_ %2 == 0)
	{
		former = size_ / 2 - 1;
		later = size_ / 2;
	}
	else
	{
		former = size_ / 2;
		later = size_ / 2;
	}

#pragma omp parallel for
	for (int i = 0; i < ground_slice.size(); i++)
	{
		map<int, Pixel> sparse_im2;
		for (int k = 0; k < ground_slice[i].size(); k++)
		{
			sparse_im2.insert(pair<int, Pixel>(ground_slice[i][k].row*image_height +
				ground_slice[i][k].col,
				ground_slice[i][k]));
		}
		for (int it = i - former; it <= i + later; it++)
		{
			if (it == i)
			{
				continue;
			}
			if (it<0 || it>ground_slice.size() - 1)
			{
				continue;
			}
			//std::cout << it << ",";
			for (int k = 0; k < ground_slice[it].size(); k++)
			{
				int index_ = ground_slice[it][k].row*image_height + ground_slice[it][k].col;
				map<int, Pixel>::iterator res = sparse_im2.find(index_);
				if (res != sparse_im2.end())
				{
#if MIP_MAXorMIN
					if (ground_slice[it][k].gray_value > res->second.gray_value)
					{
						res->second.gray_value = ground_slice[it][k].gray_value;
					}
#else
					if (ground_slice[it][k].gray_value < res->second.gray_value)
					{
						res->second.gray_value = ground_slice[it][k].gray_value;
					}
#endif
				}
				else
				{
					sparse_im2.insert(pair<int, Pixel>(index_, ground_slice[it][k]));
				}
			}
		}
		//std::cout << "\n";
		vector<Pixel> sparse_im3;
		for (auto it = sparse_im2.begin(); it != sparse_im2.end(); it++)
		{
			sparse_im3.push_back(it->second);
		}
		every5_group[i] = sparse_im3;
	}


	for (int i = 0; i < every5_group.size(); i++)
	{
		QString filenameout = "Input/Generated/Compound/Slice_";
		filenameout.append(QString("%1.png").arg(i + 1));

		QImage out_(image_width, image_height, QImage::Format_RGB16);
		out_.fill(qRgb(0, 0, 0));
		for (auto it = every5_group[i].begin(); it != every5_group[i].end(); it++)
		{
			out_.setPixel(it->row, image_height - 1 - it->col,
				qRgb(it->gray_value, it->gray_value, it->gray_value));
		}
		out_.save(filenameout);
	}
}

void MarkingProcess::update_compound_mask(int size_)
{
	int former, later;
	if (size_ % 2 == 0)
	{
		former = size_ / 2 - 1;
		later = size_ / 2;
	}
	else
	{
		former = size_ / 2;
		later = size_ / 2;
	}
#pragma omp parallel for
	for (int i = 0; i < masks.size(); i++)
	{
		set<int> sparse_im2;
		for (int it = i - former; it <= i + later; it++)
		{
			if (it<0 || it>ground_slice.size() - 1)
			{
				continue;
			}
			for (auto k = masks[it].begin(); k != masks[it].end(); k++)
			{
				int id_ = *k + it*image_height*image_width;
				sparse_im2.insert(id_);
			}
		}
		compound_marked_pixels[i] = sparse_im2;
	}
	std::cout << __FUNCTION__ << ": update compound-masks done\n";
}


vector<Pixel> 	& MarkingProcess::get_current_ground_slice()
{
	if (current_slice_index>=0&& current_slice_index<ground_slice.size())
	{
		return ground_slice[current_slice_index];
	}
	return ground_slice[0];
}

vector<Pixel> 	& MarkingProcess::get_current_every5_slice()
{
	if (current_slice_index >= 0 && current_slice_index < every5_group.size())
	{
		return every5_group[current_slice_index];
	}
	return every5_group[0];
}

set<int> & MarkingProcess::get_current_marked_pixels()
{
	if (current_slice_index >= 0 && current_slice_index < masks.size())
	{
		return masks[current_slice_index];
	}
	return masks[0];
}

void MarkingProcess::add_new_selected_pixels(const vector<int> new_pixels)
{
	if (new_pixels.empty()||current_slice_index < 0 || current_slice_index >= masks.size())
	{
		return;
	}
	
	int former, later;
	if (compound_size% 2 == 0)
	{
		former = compound_size / 2 - 1;
		later = compound_size / 2;
	}
	else
	{
		former = compound_size / 2;
		later = compound_size / 2;
	}
//#pragma omp parallel for
	for (int i = 0;i<new_pixels.size();i++)
	{
		masks[current_slice_index].insert(new_pixels[i]);
//#pragma omp parallel for
		for (int it = current_slice_index - former; it <= current_slice_index + later; it++)
		{
			if (it<0 || it>masks.size() - 1)
			{
				continue;
			}
			int id_ = new_pixels[i] + current_slice_index*image_width*image_height;
			compound_marked_pixels[it].insert(id_);
		}
	}
}

void MarkingProcess::remove_selected_pixles(const vector<int> new_pixels)
{
	if (new_pixels.empty() || current_slice_index < 0 || current_slice_index >= masks.size())
	{
		return;
	}
	int former, later;
	if (compound_size % 2 == 0)
	{
		former = compound_size / 2 - 1;
		later = compound_size / 2;
	}
	else
	{
		former = compound_size / 2;
		later = compound_size / 2;
	}

	if (masks[current_slice_index].empty())
	{
		return;
	}
	for (int i = 0; i < new_pixels.size(); i++)
	{		
		masks[current_slice_index].erase(new_pixels[i]);
//#pragma omp parallel for
		for (int it = current_slice_index - former; it <= current_slice_index + later; it++)
		{
			if (it<0 || it>masks.size() - 1)
			{
				continue;
			}
			int id_ = new_pixels[i] + current_slice_index*image_width*image_height;
			compound_marked_pixels[it].erase(id_);
		}
	}
}



//model view
void  MarkingProcess::GetCameraPosAndOrientation(vector<vector<double>> &pos_orient)
{
	if (model_position_orientation.size() != 0)
	{
		pos_orient = model_position_orientation;
	}
	//else
	//{
	//	//std::cout << "read from txt" << std::endl;
	//	pos_orient.resize(2);
	//	pos_orient[0].resize(3);
	//	pos_orient[1].reserve(4);
	//	std::ifstream fin;
	//	QString orientation_f = out_file_name;
	//	orientation_f.append("/camera_pos_orientation.txt");
	//	fin.open(orientation_f.toStdString());
	//	if (fin.is_open())
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			fin >> pos_orient[0][i];
	//		}
	//		for (int i = 0; i < 4; i++)
	//		{
	//			fin >> pos_orient[1][i];
	//		}
	//	}
	//	fin.close();
	//}
}

void  MarkingProcess::SetCameraPosAndOrientation(vector<vector<double>> pos_orient)
{
	model_position_orientation = pos_orient;
}

bool & MarkingProcess::is_fixed_model_view()
{
	return is_fixed_modelview;
}