import rabbitnbt as m
import rabbitnbt
import pyzstd
import time
import os

assert m.__version__ == '1.0.0'

TEST_CHUNK_FILE = "/tmp/0.raw"
TEST_CHUNK_FILE_COMPRESSED = "tests/0.raw.zst"

if not os.path.exists(TEST_CHUNK_FILE):
    region = pyzstd.decompress(open(TEST_CHUNK_FILE_COMPRESSED, "rb").read())
    open(TEST_CHUNK_FILE, "wb").write(region)
nbt = open(TEST_CHUNK_FILE, "rb").read()
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

print(obj.compound())

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
#    print(tag)
#    print(tag.get("x").value())
    if tag.contains("Items"):
        items = tag.get("Items")
        for item in items.iterate():
#            print(item.list())
#            print(item.get("id"))
            if item.get("id").count("shulker_box"):
                tag = item.get("tag")
#                print(tag.list())
                print(tag.get("display").get("Name"))
                BlockEntityTag = tag.get("BlockEntityTag")
                print(BlockEntityTag.list())
#        print(items.iterate())
#    print(tag.list())
#    print(rabbitnbt.NBTCompoundTag(tag))
#    print(issubclass(tag, rabbitnbt.NBTCompoundTag))

# TODO NBTTagString test