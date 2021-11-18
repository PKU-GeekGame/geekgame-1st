from distutils.core import setup
import py2exe

setup(
    windows=['hidden_treasure.pyw'],
    options={'py2exe': {'bundle_files': 1, 'compressed': True}},
    zipfile=None,
    data_files=[
        ('', ['textures.msi'])
    ],
)