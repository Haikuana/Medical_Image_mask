#include "auxfun.h"

bool sort_qstringpair_secondgreater(pair<QString, int> p1, pair<QString, int> p2)
{
	return p1.second < p2.second;
}

bool getFiles(std::string file_path, std::vector<std::string>& file_names)
{
	intptr_t hFile = 0;
	_finddata_t fileInfo;
	hFile = _findfirst(file_path.c_str(), &fileInfo);
	if (hFile != -1) {
		do {
			if ((fileInfo.attrib &  _A_SUBDIR)) {
				continue;
			}
			else {
				file_names.push_back(fileInfo.name);
				//cout << fileInfo.name << endl;
			}

		} while (_findnext(hFile, &fileInfo) == 0);

		_findclose(hFile);
	}
	else
		return false;

	return true;
}

bool generate_output_directory(QString allDir)
{
	QStringList dirList = allDir.split("/");
	QString dir = ".";

	for (unsigned i = 0; i < dirList.size(); i++)
	{
		bool ret = false;

		QDir outputDir(dir);
		dir.append("/");
		dir.append(dirList.at(i));

		QString dirName = dirList.at(i);

		if (outputDir.exists(dirName))
			ret = true;
		else
			ret = outputDir.mkdir(dirName);

		if (!ret)
			return false;
	}

	return true;
}