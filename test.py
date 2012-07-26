from perper import PersistentDict

p = PersistentDict()
for x in xrange(10000):
    for y in xrange(1000):
        p = p.setitem(y, x)

