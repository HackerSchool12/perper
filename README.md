Perper
======

_Efficient persistent hashtrie implemented in C_

This project implements a persistent hash map as seen in Clojure. It is implemented in C, but provides Python bindings for convenience.

Check [this page][1] for more info about how persistent hash tries are implemented.

ctags annotated source is avaliable in the [gh-pages branch][3]. I recommend starting in perper.pyx or hashmap.c, and clicking your way through.

Usage
-----

```python
from perper import PersistentDict

p = PersistentDict()
q = p.setitem("foo", "bar")
r = q.delitem("foo")

p["foo"] # None
q["foo"] # "bar"
r["foo"] # None
```

Performance
-----------

It's fast.

```python
from perper import PersistentDict

p = PersistentDict()
for x in xrange(10000):
    for y in xrange(1000):
        p = p.setitem(y, x)
```
```
real	0m20.730s
user	0m20.689s
sys	0m0.020s
```
```c
#include "hashmap.h"

int main(int argc, char *argv[]) {
	int x;
	int y;
	OInt *key;
	OInt *val;
	BasicNode *p = new_empty_node();
	BasicNode *q;
	for(x=0;x<10000;x++) {
		for(y=0;y<1000;y++) {
			key = new_oint(y);
			val = new_oint(x);
			q = INSERT(p, key, val);
			release((Object*)key);
			release((Object*)val);
			release((Object*)p);
			p = q;
		}
	}
	release((Object*)p);
	return 0;
}
```
```
real	0m14.430s
user	0m14.413s
sys	0m0.000s
```
```python
from pysistence import make_dict
p = make_dict()

for x in xrange(10000):
    for y in xrange(1000):
        p = p.using(**{str(y):x})
```
```
real	27m18.539s
user	27m15.954s
sys	0m0.540s
```
```python
from perseus import frozendict

p = frozendict()
for x in xrange(10000):
    for y in xrange(1000):
        p = p.withPair(y, x)
```
```
real	2m58.169s
user	2m57.791s
sys	0m0.024s
```
```clojure
user=> (time (reduce conj {} (for [x (range 10000) y (range 1000)] [y x])))
"Elapsed time: 13728.298915 msecs"
```
Wow.

TODO
----

 * Make it faster, Java beats us.
 * Use C primitives instead of Python objects where possible
 * Implement hash and equality to support nested maps

[1]: http://blog.higher-order.net/2009/09/08/understanding-clojures-persistenthashmap-deftwice/
[3]: http://pepijndevos.nl/perper/
