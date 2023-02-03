#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

class NBTRootObject {
public:
    nbt_node* root = NULL;

    NBTRootObject(nbt_node *root) {
        this->root = root;
    }

    ~NBTRootObject() {
        if (this->root != NULL)
            nbt_free(this->root);
    }
};

class NBTTag;
class NBTListTag;
class NBTCompoundTag;

class NBTRoot {
private:
    std::shared_ptr<NBTRootObject> rootObj;

    NBTRoot(const char *data, int size) {
        nbt_node *root = nbt_parse(data, size);
        if (errno != NBT_OK) {
            throw pybind11::value_error();
        }
        rootObj = std::make_shared<NBTRootObject>(root);
    }

public:
    NBTRoot() {}
    ~NBTRoot() {}

    static NBTRoot from_bytes(pybind11::bytes bytes);

    pybind11::bytes to_bytes();

    int64_t get_int(std::vector<std::string> path);

    void set_existing_int(std::vector<std::string> path, int value);

    NBTListTag get_existing_list(std::vector<std::string> path);

    NBTCompoundTag get_existing_compound(std::vector<std::string> path);

    std::vector<std::string> list_tags();

    NBTCompoundTag compound();

//    bool contains(std::string key);
//    pybind11::object get(std::string key);
//    std::vector<std::string> keys();
};

class NBTTag {
protected:
    NBTRoot rootObj;
    nbt_node* node;
};

class NBTListTag: public NBTTag {
public:
    NBTListTag(NBTRoot rootObj, nbt_node *node);

    std::vector<pybind11::object> iterate();
};

class NBTCompoundTag: public NBTTag {
public:
    NBTCompoundTag(NBTRoot rootObj, nbt_node *node);

    bool contains(std::string key);
    pybind11::object get(std::string key);
    std::vector<std::string> keys();
};

class NBTIntTag: public NBTTag {
public:
    NBTIntTag(NBTRoot rootObj, nbt_node *node);

    int value();
    void set(int32_t value);
};

class NBTStringTag: public NBTTag {
public:
    NBTStringTag(NBTRoot rootObj, nbt_node *node);

    std::string value();
    void set(std::string value);
}
