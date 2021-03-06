#ifndef FSI_DUMMY_COMMUNICATOR_H
#define FSI_DUMMY_COMMUNICATOR_H

#include <vector>
#include <string>
#include <hash_map>
namespace fsi{
	class FSIDummyCommunicator;
	class FSIData;
}

class fsi::FSIDummyCommunicator{
private:

	std::vector<int> _pointIds;
	std::vector<double> _data2Transfer;

	std::string _hostname;
	double* _data;
	fsi::FSIData* _comm;
	__gnu_cxx::hash_map<int,int>* _globalToLocalPointMapping;
public:
	FSIDummyCommunicator(std::string hostname);
	~FSIDummyCommunicator();
	void setData(double* data);
	void addPointId(const int pointId);
	void setPointMapping(__gnu_cxx::hash_map<int,int>*);
	void flush();
	void connect();
	void disconnect();
};

#endif
