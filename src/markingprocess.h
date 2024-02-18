#pragma once
#ifndef MARKING_H
#define MARKING_H

#include "datatype.h"
#include "auxfun.h"

#define MIP_MAXorMIN 1//max:1 min:0
//stable version due to non-CPU-parallel while erasing
//Notes: does not update Mip-mask now

class MarkingProcess :public QObject
{
	Q_OBJECT
public:
	MarkingProcess();
	~MarkingProcess();

	void	load_allimage();
	//bool	read_image(const QString &name);
	void	write_current_slice();
	void	write_all_slices();
	int		get_image_width();
	int		get_image_height();

	void	write_slices_tovolume();

	vector<set<int>> get_former_masks();
	QString			get_current_ground_file_name();
	vector<QString>	&get_file_names();
	set<int>		&get_compound_mask();
	set<int>		&get_current_marked_pixels();
	vector<Pixel> 	&get_current_ground_slice();
	vector<Pixel> 	&get_current_every5_slice();
	
	void	set_current_slice_index(int a);
	void	update_compound_size(int index_);
	void	update_compound_mask(int index_);

	void	add_new_selected_pixels(const vector<int> new_pixels);
	void	remove_selected_pixles(const vector<int> new_pixels);
	
	//get and set model view
	void	GetCameraPosAndOrientation(vector<vector<double>> &pos_orient);
	void	SetCameraPosAndOrientation(vector<vector<double>> pos_orientt);
	bool	&is_fixed_model_view();

signals:
	void	brush_changed(int);

private:

	vector<pair<QString, int>>	mask_files;
	vector<QString>				mask_files_string;
	vector<pair<QString, int>>	compoundslice_files;
	vector<pair<QString, int>>	groundslice_files;

	int							current_slice_index;
	int							image_width;
	int							image_height;
	vector<vector<Pixel>>		ground_slice;

	int							compound_size;
	vector<vector<Pixel>>		every5_group;

	//marking
	vector<set<int>>			masks;
	vector<set<int>>			compound_marked_pixels;

	//model view
	vector<vector<double>>		model_position_orientation;
	bool						is_fixed_modelview;
};


#endif // !MARKING_H
