#include "bpnode.h"

bpnode::bpnode() {	
	disl = 0;
	disr = 0;
	son = 0;
	len = -1;
	objloc = -1;
	metric = -1;

	datas = NULL;
	datad = NULL;
}
void bpnode::copy(bpnode* m) {	
	objloc = m->objloc;
	len = m->len;
	metric = m->metric;	
	objloc = m->objloc;
	if (metric < 0||len<0) return;
	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		datad = new float[len];
		for (int i = 0; i < len; i++)
			datad[i] = m->datad[i];
		break;
	case 6:
		datas = new char[len];
		for (int i = 0; i < len; i++)
			datas[i] = m->datas[i];
		break;
	}
}

float bpnode::distance(bpnode* m)
{
	if (m == NULL) return 0;
	return baseme(datad, m->datad, datas, m->datas, metric, len);
}

bpnode::bpnode(int m, Object* o) {
	
	disl = 0;
	disr = 0;
	son = -1;
	len = 0;
	metric = m;
	datas = NULL;
	datad = NULL;
	switch (metricm[m]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		//		cout<<"Object size " << o->size[m * 2 + 1] - o->size[m * 2] << endl;
		datad = new float[o->size[m * 2 + 1] - o->size[m * 2]];
		len = o->size[m * 2 + 1] - o->size[m * 2];
		//cout << "Creating Mnode 0/1 " << sizeof(datad[0]) << " " << sizeof(float)  << endl;
		for (int i = o->size[m * 2]; i < o->size[m * 2 + 1]; i++)datad[i - o->size[m * 2]] = o->datad[i];
		//cout << "Creating Mnode 0/1 " << sizeof(datad) << " " << sizeof(float) << " " << datad[0] << endl;
		break;
	case 6:
		datas = new char[o->size[m * 2 + 1] - o->size[m * 2]];

		len = o->size[m * 2 + 1] - o->size[m * 2];
		for (int i = o->size[m * 2]; i < o->size[m * 2 + 1]; i++)datas[i - o->size[m * 2]] = o->datas[i];

		break;
	}
}


bpnode::~bpnode() {
	if (metric < -1 || metric>m) {
		outnode("error ");
	}
	//outnode("trying ");
	if (len > 0) {
		switch (metricm[metric]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			if (datad != NULL) delete[] datad;
			datad = NULL;
			break;
		case 6:
			if (datas != NULL) delete[] datas;
			datas = NULL;
			break;
		}
	}
}

int bpnode::getsize()
{
	//return size * sizeof(float) + 2 * sizeof(int);
	if (metric < -1 || metric>m) {
		outnode("error ");
	}
	int i = 0;
	i = 4 * sizeof(int) + 2 * sizeof(float);
	if (metric < 0||len<0) return i;
	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		i += len * sizeof(float);
		break;
	case 6:
		i += len * sizeof(char);
		break;
	}
	return i;
}


int bpnode::read_from_buffer(char* buffer)
{
	int i = 0;
	memcpy(&len, &buffer[i], sizeof(int));
	i += sizeof(int);

	memcpy(&objloc, &buffer[i], sizeof(int));
	i += sizeof(int);

	memcpy(&son, &buffer[i], sizeof(int));
	i += sizeof(int);

	memcpy(&metric, &buffer[i], sizeof(int));
	i += sizeof(int);
	//cout << "node total size " << i << endl;
	memcpy(&disl, &buffer[i], sizeof(float));
	i += sizeof(float);

	memcpy(&disr, &buffer[i], sizeof(float));
	i += sizeof(float);
	//cout << "node total size " << i << endl;
	if (len < 1) return i;
	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		datad = new float[len];
		memcpy(datad, &buffer[i], len * sizeof(float));
		i += len * sizeof(float);
		break;
	case 6:
		datas = new char[len];
		memcpy(datas, &buffer[i], len * sizeof(char));
		i += len * sizeof(char);
		break;
	}
	//cout << "node total size " << i << endl;
	//cout << "Mnode tst " << getsize() << endl;
	return i;
}

int bpnode::write_to_buffer(char* buffer)
{
	int i = 0;

	memcpy(&buffer[i], &len, sizeof(int));
	i += sizeof(int);

	memcpy(&buffer[i], &objloc, sizeof(int));
	i += sizeof(int);

	memcpy(&buffer[i], &son, sizeof(int));
	i += sizeof(int);

	memcpy(&buffer[i], &metric, sizeof(int));
	i += sizeof(int);

	memcpy(&buffer[i], &disl, sizeof(float));
	i += sizeof(float);

	memcpy(&buffer[i], &disr, sizeof(float));
	i += sizeof(float);

	if (len < 1) return i;
	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5: {
		memcpy(&buffer[i], datad, len * sizeof(float));
		i += len * sizeof(float);
		break;
	}
	case 6: {
		memcpy(&buffer[i], datas, len * sizeof(char));
		i += len * sizeof(char);
		break;
	}

	}

	//outnode();
	//cout << "Mnode total size " << i<<endl;
	//cout << "Mnode tst " << getsize()<<endl;
	return i;
}

void bpnode::outnode(string str)
{

	cout << str << "son " << son << " disl " << disl << " disr " << disr << " metric " << metric << " len " << len;
	if (objloc > 0) {
		Object* q = Obj_f->get_object(objloc);
		cout << " id " << q->id;
	}
	cout << " data obj " << objloc << " data:";
	if (len < 1) {
		cout << endl;
	//	cout << " outnode failed " << endl;
		return;
	}

	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		for (int j = 0; j < len; j++)
			cout << " " << datad[j];
		break;
	case 6:
		for (int j = 0; j < len; j++)
			cout << datas[j];
		break;
	}
	cout << endl;
}
