from distutils.core import setup, Extension
from Cython.Distutils import build_ext

permap = Extension(
    'perper',
    sources = ['hashmap.c', 'object.c', 'pyext.c', 'perper.pyx']) 

setup (name = 'perper',
       version = '1.0',
       description = 'This is a persistent hashmap package.',
       cmdclass = {'build_ext': build_ext},
       ext_modules = [permap])
