import msilib
import msilib.schema
import pathlib
import base64
import hashlib

DB_FN = '1textures.msi'

# write
# https://docs.microsoft.com/en-us/windows/win32/msi/property-table
# https://github.com/python/cpython/blob/main/PC/_msi.c

#'''
db = msilib.init_database(DB_FN, msilib.schema, 'Resources', 'Resources', '1.0', 'Resources')
for path in pathlib.Path('img').glob('*.gif'):
    with path.open('rb') as f:
        data = f.read()
        data = base64.b64encode(data).decode()
        print(path.stem, len(data))
        msilib.add_data(db, 'UIText', [(path.stem, data)])
db.Commit()
db.Close()
#'''

# check

db = msilib.OpenDatabase(DB_FN, msilib.MSIDBOPEN_READONLY)

def get_key(db, k):
    v = db.OpenView('select * from UIText where `Key`=?')
    args = msilib.CreateRecord(1)
    args.SetString(1, k)
    v.Execute(args)
    rec = v.Fetch()
    ret = rec.GetString(2)
    v.Close()
    return ret

for path in pathlib.Path('img').glob('*.gif'):
    print('checking', path.stem)
    data = get_key(db, path.stem)
    data = base64.b64decode(data.encode())
    with path.open('rb') as f:
        f.read()==data

print('all checked!')

with open(DB_FN, 'rb') as f:
    data = f.read()
    hash = hashlib.sha256(data).digest()
    print('psk is', repr(hash))