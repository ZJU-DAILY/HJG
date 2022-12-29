#include <stdio.h>
#include <memory.h>


#include <sstream>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "bptree.h"

extern RAF* Obj_f;
extern RAF* Index_f;
extern Cache* Obj_c;
extern Cache* Index_c;

void bptree::flush() {
	char* buf;
	//	cout << "This Mtree is dirty, rewrite! " << endl;
		//		outnode("Dirty ");

	buf = new char[Index_f->file->get_blocklength()];
	write_to_buffer(buf);
	//	cout << "Write fin " << endl;
	dirty = 0;
	if (Index_c == NULL) // no cache
		Index_f->file->write_block(buf, block);
	else
		Index_c->write_block(buf, block, Index_f);
	//outnode("flush ");
	delete[] buf;
}


int bptree::getsize()
{
	//return size * sizeof(float) + 2 * sizeof(int);
	//outnode("test" );
	int ii = 0;
	ii = 3 * sizeof(int);
	for (int i = 0; i < leafnum; i++) {
		//		son[i]->outnode("getsize ");
				//if (son[i]->metric < 0) outnode("!!error Mtree ");
		ii += son[i]->getsize();
	}
	//cout << "leafnum " << leafnum << " size " << ii << endl;
	return ii;
}


int bptree::read_from_buffer(char* buffer)
{
	int i;
	memcpy(&fa, buffer, sizeof(int));
	i = sizeof(int);

	memcpy(&isleaf, &buffer[i], sizeof(int));
	i += sizeof(int);
	
	memcpy(&leafnum, &buffer[i], sizeof(int));
	i += sizeof(int);

	
	int pos;
	son = new bpnode * [leafnum];
	for (int j = 0; j < leafnum; j++) {
		son[j] = new bpnode();
		pos = son[j]->read_from_buffer(&buffer[i]);
		i += pos;
	}
	return i;
}

int bptree::write_to_buffer(char* buffer)
{

	int i;

	memcpy(buffer, &fa, sizeof(int));
	i = sizeof(int);

	memcpy(&buffer[i], &isleaf, sizeof(int));
	i += sizeof(int);

	memcpy(&buffer[i], &leafnum, sizeof(int));
	i += sizeof(int);


	int pos;
	
	//cout << "========= Mtree writing " << i << endl;
	
	for (int j = 0; j < leafnum; j++) {
		//	cout <<"======i is "<<i<< " this is son " << j << " ";
		//	son[j]->outnode("try ");
		//if (isleaf == 0) son[j]->len = -1; 
		pos = son[j]->write_to_buffer(&buffer[i]);
		i += pos;
		//		cout << "====== Finish son " << j << " leafnum " << leafnum << " ===== check len " << pos << endl;
	}

	//outnode();
	//cout << "total size " << i<<endl;
	//cout << " tst " << getsize()<<endl;
	return i;
}

void bptree::outnode(string str)
{
	cout << str << "Block " << block << " isleaf " << isleaf << " fa " << fa << " leafnum " << leafnum << endl;
	for (int i = 0; i < leafnum; i++)son[i]->outnode("    " + str);
	cout << endl;
}

void bptree::loadroot(int  page) {
	//cout << "page is " << page << endl;
	char* buf = new char[Index_f->file->get_blocklength()];
	if (Index_c == NULL) {
		Index_f->file->read_block(buf, page);
	}
	else {
		//	cout << "read from cache" << endl;
		Index_c->read_block(buf, page, Index_f);
	}
	read_from_buffer(buf);
//	outnode("load root ");
	block = page;
	delete[] buf;
}

void bptree::bfs(string str) {

	outnode(str);		
	for (int i = 0; i < leafnum; i++) {
		if(isleaf==0){
			bptree* p = new bptree();
			p->loadroot(son[i]->son);
			p->bfs("   "+str);
			delete p;
		}
	}

}


bptree::bptree() {
	dirty = 0;
	dis = 0;	
	son = NULL;
	fa = -1;
	leafnum = 0;
	isleaf = 1;
}


bptree::~bptree() {
	
	if (dirty && block > -1) {
		//cout << "flushing" << endl;
		flush();
	}
	else if (dirty) {
		//cout << "this Mtree written failed" << endl;
	}
	else {
		//cout << "No need to write this Node" << endl;
	}
	//outnode("");
	for (int i = 0; i < leafnum; i++) {
		
		delete son[i];
		son[i] = NULL;
		//		cout << "deleting "<<i << endl;
	}
	//cout << "deleting  node" << endl;
	if (son != NULL) delete[] son;
	//cout << "deleting  finish" << endl;
}

void bptree::entere(bpnode* q) {
	bpnode** tmp = new bpnode * [leafnum + 1];
	//	cout << "======= Entere" << endl;
	for (int i = 0; i < leafnum; i++) {
		tmp[i] = son[i];
		//		son[i]->outnode("");
		son[i] = NULL;
	}
	delete[] son;
	tmp[leafnum] = q;
	son = tmp;
	leafnum++;	
	dirty = 1;
}

void bptree::insert(bpnode* q) {
//	if (f == 1) { cout << "==============inserting "; q->outnode(" insert "); }
//	if (q->dis < disl) disl = q->dis;
//	if (q->dis > disr) disr = q->dis;
	if (son == NULL || isleaf == 1) {
		entere(q);
		int k=leafnum;
		for (int i = 0; i < leafnum; i++) if (son[i]->disl + eps >= q->disl) {
			k = i;
			break;
		}
		//cout << "k is " << k << endl;
		for (int i = leafnum - 1; i > k; i--) {
			son[i] = son[i - 1];
		}
		son[k] = q;
		if (getsize() >= Index_f->file->get_blocklength()) {
		//	if (f == 1) { cout << "==============going split "; outnode(""); }
			this->split(0, NULL);			
		}
		//bfs("====== after inserting in leaf ");
		
	}
	else {
		int k = 0;		
		while (k < leafnum-1 && son[k]->disr < q->disl) k++;
		
		if (son[k]->disl > q->dis) son[k]->disl = q->dis;
		if (son[k]->disr < q->dis) son[k]->disr = q->dis;
		flush();
		bptree* p = new bptree();
		p->loadroot(son[k]->son);
		p->fa = block;
		p->insert(q);			
		delete p;		
	}
	//if (f == 1) { cout << "==============finish inserting "<<endl;}
}


void bptree::split(int flag, bptree* rt) {
	//cout<<"blocksize " << Index_f->file->get_blocklength()<<" real size:"<<getsize()<<" leaf: "<<leafnum << endl;
	dirty = 1;
	if (fa == -1) {
	//	if (f == 1) outnode("==== splitting fa=-1 " + to_string(flag) + " ");
		bptree* n1 = new bptree();
		bptree* n2 = new bptree();
		n1->fa = block;
		n2->fa = block;
		// ================================ son need to change, together with block
		n1->isleaf = isleaf;
		n2->isleaf = isleaf;
		isleaf = 0;

		char* buf = new char[Index_f->file->get_blocklength()];
		n1->write_to_buffer(buf);
		n1->block = Index_f->file->append_block(buf);
		n1->dirty = 1;
		n2->write_to_buffer(buf);
		n2->block = Index_f->file->append_block(buf);
		//write_to_buffer(buf);
		//block = Index_f->file->append_block(buf);
		delete[] buf;
		n2->dirty = 1;
		
		n1->leafnum = leafnum / 2;
		n2->leafnum = leafnum - leafnum / 2;
		n1->son = new bpnode*[leafnum / 2];
		n2->son = new bpnode * [leafnum-leafnum / 2];
		for (int i = 0; i < leafnum; i++) 
		if(i<leafnum/2){
			n1->son[i] = son[i];
			son[i] = NULL;
		}
		else {
			n2->son[i - leafnum / 2] = son[i];
			son[i] = NULL;
		}
		delete[]son;
		son = new bpnode * [2];
		bpnode* sonl = new bpnode();
		bpnode* sonr = new bpnode();
		sonl->son = n1->block;
		sonr->son = n2->block;
		sonl->disl = n1->son[0]->disl;
		sonl->disr = n1->son[n1->leafnum-1]->disr;
		sonr->disl = n2->son[0]->disl;
		sonr->disr = n2->son[n2->leafnum - 1]->disr;
		leafnum = 2;
		son[0] = sonl;
		son[1] = sonr;
	//	cout<<"blocksize " << Index_f->file->get_blocklength() << endl;
		if (n1->getsize() >= Index_f->file->get_blocklength() ) n1->split(1, this);
		//cout << "prep 1" << endl;
		if (n2->getsize() >= Index_f->file->get_blocklength() ) n2->split(1, this);

		//cout << "prep 4" << endl;
		if (getsize() >= Index_f->file->get_blocklength() ) split(0, NULL);
		delete n1;
		delete n2;
		//bfs("split finish");
		//outnode("splut fin ");
	}
	else {
		//if (f == 1) outnode("==== splitting normal "+to_string(flag)+" ");
		bptree* root;
		if (flag == 0) {
			root = new bptree();
			root->loadroot(fa);//	if(f==1)		cout << "              insert in normal node" << endl << endl;
		}
		else {
			root = rt;//if (f == 1)		cout << "              float insert in normal node" << endl << endl;
		}
		//root->bfs(" before split ");
		bptree* n2 = new bptree();
		n2->fa = root->block;
		n2->isleaf = isleaf;
		char* buf = new char[Index_f->file->get_blocklength()];
		n2->write_to_buffer(buf);
		n2->block = Index_f->file->append_block(buf);
		delete[] buf;
		dirty = 1;
		root->dirty = 1;
		n2->dirty = 1;
		//		cout << "0002             insert in normal node" << endl << endl;
		bpnode** tmp = son;

		n2->leafnum = leafnum - leafnum / 2;
		n2->son = new bpnode * [leafnum - leafnum / 2];
		son= new bpnode * [leafnum / 2];
		for (int i = 0; i < leafnum; i++)
			if (i < leafnum / 2) {
				son[i] = tmp[i];
				tmp[i] = NULL;
			}
			else {
				n2->son[i - leafnum / 2] = tmp[i];
				tmp[i] = NULL;
			}
		delete[]tmp;		
		leafnum = leafnum / 2;
		int k = -1;
		for (int i = 0; i < root->leafnum; i++) if (root->son[i]->son == block) {
			k = i;
			break;
		}
		//if (k == -1) cout << "===== Fatail Error Split finding root faile" << endl;
		bpnode* sonr = new bpnode();		
		sonr->son = n2->block;
		root->son[k]->disl = son[0]->disl;
		root->son[k]->disr = son[leafnum - 1]->disr;
		sonr->disl = n2->son[0]->disl;
		sonr->disr = n2->son[n2->leafnum - 1]->disr;

		root->entere(sonr);
		for (int i = root->leafnum - 1; i > k+1; i--) {
			root->son[i] =root->son[i - 1];
		}
		root->son[k+1] = sonr;

		//cout << "0003              insert in normal node" << endl << endl;
		fa = root->block;
		/*
		if (f == 1) {
			cout << "split finish " << endl;
			root->outnode(" split root ");
			outnode("this ");
			n2->outnode("n2 ");
		}
		*/
		if (getsize() >= Index_f->file->get_blocklength())	split(1, root);
		if (n2->getsize() >= Index_f->file->get_blocklength()) n2->split(1, root);

		if (flag == 0 && root->getsize() >= Index_f->file->get_blocklength()) root->split(0, NULL);

		//cout << "prep 2 4" << endl;
		
		delete n2;
		if (flag == 0) delete root;
	//	root = new bptree();
	//	root->loadroot(fa);
	//	root->outnode("written new root ");
	//	delete root;
	}
	//	cout<<"===================here is "<<kj<<endl;

}


bptree* bptree::prenode(int loc) {
	//outnode("prenode ");
	if (son[0]->objloc == loc) {
		//cout << "this is prenode 1 "<<loc << endl;
		if (fa == -1) return NULL;
		bptree* pe = new bptree();
		pe->loadroot(fa);
		int temp = block;
		while (pe->fa != -1 && pe->son[0]->son == temp) {
			//pe->outnode("in while fa is ");
			//cout << "temp " << temp << endl;
			temp = pe->block;
			pe->loadroot(pe->fa);
		}
		//pe->outnode("fa is ");
		//cout << "temp " << temp << endl;
		if (pe->son[0]->son == temp) return NULL;
		int k = 0;
		while (pe->son[k + 1]->son != temp) k++;
		bptree* ps = new bptree();
		ps->loadroot(pe->son[k]->son);
		if (ps->fa != pe->block) {
			ps->fa = pe->block;
			ps->dirty = 1;
		}
		while (ps->isleaf == 0) {
			delete pe;
			pe = ps;
			ps = new bptree();
			ps->loadroot(pe->son[leafnum-1]->son);
			if (ps->fa != pe->block) {
				ps->fa = pe->block;
				ps->dirty = 1;
			}
		}
		delete pe;
		return ps;
	}
	else {
	//	cout << "this is prenode 2" << endl;
		return this;
	}
}

bptree* bptree::lastnode() {	
	if (isleaf == 1) return this;
	bptree* pe = new bptree();
	pe->loadroot(son[leafnum-1]->son);
	if (pe->fa != block) {
		pe->fa = block;
		pe->dirty = 1;
	}
	if (pe->isleaf == 1) return pe;
	else {
		bptree* pq = pe->lastnode();
		delete pe;
		return pq;
	}
}



void bptree::delnode(int loc) {
	//if (f == 1)cout<<"==============deleting "<<loc<<" isleaf "<<isleaf<<endl;
	int k = 0;
	dirty = 1;
	for(int i=0;i<leafnum;i++) 
		if (isleaf == 1) {
			if (son[i]->objloc == loc) {
				k = i;
				break;
			}
		}
		else if (son[i]->son == loc) {
			k = i;
			break;
		}
	//cout << "k lefnum " << k << " " << leafnum << endl;
	//outnode("this ");
	if(leafnum>1){
		//cout << "deleteing 1" << endl;
		
		bpnode** sontmp = new bpnode * [leafnum-1];
		
		for (int i = 0; i < leafnum; i++) 
		if(i<k){
			sontmp[i] = son[i];
			son[i] = NULL;
		}
		else if(i>k){
			sontmp[i-1] = son[i];
			son[i] = NULL;
		}
		else {
			delete son[i];
			son[i] = NULL;
		}
		leafnum--;		
		delete[] son;
		//cout << "deleteing 5" << endl;
		son = sontmp;
		sontmp = NULL;

	//outnode("del 1 ");
		update();
	//	outnode("del 2 ");
		//cout << "deleteing 7" << endl;
	}
	else {
		//cout << "deleteing 2" << endl;
		if(fa>-1){
		bptree* pe = new bptree();
		pe->loadroot(fa);
		pe->delnode(block);
		delete pe;
		}
	}

}

void bptree::update() {
	if (fa < 0) return;
	//outnode("update this ");
	bptree* pe = new bptree();
	pe->loadroot(fa);	
	//outnode("update this ");
	//pe->outnode("update fa ");

	int k = 0;	
	int flg = 0;
	for (int i = 0; i < leafnum; i++)
		if (pe->son[i]->son == block) {
			k = i;
			break;
		}
	if (pe->son[k]->disl > son[0]->disl) {
		pe->son[k]->disl = son[0]->disl;
		flg = 1;
	}
	if (pe->son[k]->disr < son[leafnum - 1]->disr) {
		pe->son[k]->disr = son[leafnum - 1]->disr;
		flg = 1;
	}
	if (flg == 1) {
		pe->update();
		pe->dirty = 1;
	}	
	delete pe;

}

