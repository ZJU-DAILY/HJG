#ifndef __LINLIST
#define __LINLIST

#include <stdio.h>
#include <string>
using namespace std;
////////////////////////////////////////////////////////////////////////
// LinList  (SLink)
////////////////////////////////////////////////////////////////////////

class RTree;

struct Linkable
{
public:
	int son;
	int dimension;
	int level;
	int ptr;
	double *bounces;
	double distanz;

	Linkable(int dim)
	{ dimension = dim;
	  bounces = new double[2 * dim];
    }

	~Linkable()
	{ 
		delete [] bounces;
	}
};

struct SLink
{
    Linkable *d;          // Zeiger auf Element-Daten
    SLink *next;          // Zeiger auf naechstes Element
    SLink *prev;          // Zeiger auf vorhergehendes Element

    SLink();
    ~SLink();
};

////////////////////////////////////////////////////////////////////////
// LinList
////////////////////////////////////////////////////////////////////////


class LinList
{
protected:
    SLink *first;         // Rootzeiger des Datenbestands
    SLink *last;          // Zeiger auf letztes Element
    int anz;                    // Anzahl der belegten Elemente in der Liste
    SLink *akt;           // zeigt auf aktuelles Element
    int akt_index;              // Index des zuletzt mit get geholten Elements
public:
    LinList();
    virtual ~LinList();
    int get_num()               // gibt Anzahl der im Index belegten Elements
        { return anz; }         // zurueck

    void check();               // ueberprueft Konsistenz der Liste
    void print(int k);

    void insert(Linkable *f);       // haengt ein Element vorne an die Liste an
    bool erase();               // loescht aktuelles Element aus der Liste

    Linkable * get(int i);          // liefert i-tes Element
    Linkable * get_first();         // liefert erstes Element im Index
    Linkable * get_last();          // liefert erstes Element im Index
    Linkable * get_next();          // liefert naechstes Element im Index
    Linkable * get_prev();          // liefert vorhergehendes Element im Index
};


////////////////////////////////////////////////////////////////////////
// SortedLinList
////////////////////////////////////////////////////////////////////////


class SortedLinList : public LinList
{
    bool increasing;
public:
    SortedLinList();

    void set_sorting(bool _increasing); // wenn increasing gleich TRUE, wird
                                // aufsteigend einsortiert
                                // DIESE FUNKTION MUSS VOR DEM ERSTEN EINFUEGEN
                                // GERUFEN WERDEN !!!!!!!!!!!
    void insert(Linkable *f);       // fuegt ein Element durch direktes Einfuegen ein

    void sort(bool _increasing);// sortiert die Liste
};

#endif  // __LINLIST
