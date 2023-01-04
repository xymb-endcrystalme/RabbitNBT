import rabbitnbt as m
import rabbitnbt
import time

assert m.__version__ == '1.0.0'
nbt = open("/tmp/extracted/0.raw", "rb").read()
'''
start = time.time()
m.parsenbt(nbt)
print(time.time() - start)
'''
try:
    m.from_bytes(b'aaaaa')
    assert False
except ValueError: pass

start = time.time()
obj = m.from_bytes(nbt)
print(time.time() - start)
print(obj)
print(obj.get_int(["xPos"]))
print(obj.get_int(["zPos"]))
obj.set_existing_int(["xPos"], 1)
print(obj.get_int(["xPos"]))
byt = obj.to_bytes()
#print(byt)
obj = m.from_bytes(byt)
print(obj.get_int(["xPos"]))
#print(obj.get_int(["xPos", 7]))
print(obj.list_tags())
listtag = obj.get_existing_list(["block_entities"])
#import inspect
#print(inspect.getmro(listtag))
for tag in listtag.iterate():
    print(tag)
    print(tag.get("x").value())
#    print(rabbitnbt.NBTCompoundTag(tag))
#    print(issubclass(tag, rabbitnbt.NBTCompoundTag))
