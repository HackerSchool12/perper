Perper
======

_Efficient persistent hashtrie implemented in C_

This project implements a persistent hash map as seen in Clojure. It is implemented in C, but provides Python bindings for convenience.

Check [this page][1] for more info about how persistent hash tries are implemented.

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
real0m20.730s
user0m20.689s
sys0m0.020s
```

```python
from pysistence import make_dict
p = make_dict()

for x in xrange(10000):
    for y in xrange(1000):
        p = p.using(**{str(y):x})
```
Still running...

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
