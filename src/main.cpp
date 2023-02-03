#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "nbt.h"
#include "main.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

struct nbt_list {
    struct nbt_node* data; /* A single node's data. */
    struct list_head entry;
};

NBTRoot NBTRoot::from_bytes(pybind11::bytes bytes) {
    std::string data = bytes;
    return NBTRoot(data.c_str(), data.size());
}

pybind11::bytes NBTRoot::to_bytes() {
    buffer buff = nbt_dump_binary(rootObj->root);
    return std::string((const char*)buff.data, (size_t)buff.len);
}

NBTCompoundTag NBTRoot::compound() {
    return NBTCompoundTag(*this, rootObj->root);
}

int64_t NBTRoot::get_int(std::vector<std::string> path) {
    if (path.size() == 0)
        throw pybind11::value_error();
    std::string pathString = "";
    for(uint i = 0 ; i < path.size() ; i++) {
        pathString += ".";
        pathString += path[i];
    }

    nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type == TAG_INT)
        return node->payload.tag_int;
    if (node->type == TAG_LONG)
        return node->payload.tag_long;
    if (node->type == TAG_SHORT)
        return node->payload.tag_short;
    if (node->type == TAG_FLOAT)
        return (int64_t)node->payload.tag_float;
    if (node->type == TAG_DOUBLE)
        return (int64_t)node->payload.tag_double;
    throw pybind11::value_error("Tag not numerical");
}

void NBTRoot::set_existing_int(std::vector<std::string> path, int value) {
    if (path.size() == 0)
        throw pybind11::value_error();
    std::string pathString = "";
    for(uint i = 0 ; i < path.size() ; i++) {
        pathString += ".";
        pathString += path[i];
    }

    nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type == TAG_INT || node->type == TAG_LONG || node->type == TAG_SHORT || node->type == TAG_FLOAT || node->type == TAG_DOUBLE) {
        node->payload.tag_int = value;
        node->type = TAG_INT;
        return;
    }
    throw pybind11::value_error("Tag not numerical");
}

NBTListTag NBTRoot::get_existing_list(std::vector<std::string> path) {
    if (path.size() == 0)
        throw pybind11::value_error();
    std::string pathString = "";
    for(uint i = 0 ; i < path.size() ; i++) {
        pathString += ".";
        pathString += path[i];
    }

    nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
    return NBTListTag(*this, node);
}

std::vector<std::string> NBTRoot::list_tags() {
    nbt_node *node = rootObj->root;
    std::vector<std::string> tags;
    if (node->type != TAG_COMPOUND)
        throw pybind11::value_error("Tag not compound");

    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
        nbt_node *currentNode = list_entry(pos, struct nbt_list, entry)->data;
        if (currentNode->name == NULL)
            throw pybind11::value_error("Consistency error 32314!");

        tags.push_back(std::string(currentNode->name));
    }

    return tags;
}
/*
NBTListTag NBTRoot::get_existing_compound(std::vector<std::string> path) {
    if (path.size() == 0)
        throw pybind11::value_error();
    std::string pathString = "";
    for(uint i = 0 ; i < path.size() ; i++) {
        pathString += ".";
        pathString += path[i];
    }

    nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
    return NBTListTag(*this, node);
}
*/
NBTListTag::NBTListTag(NBTRoot rootObj, nbt_node *node) {
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type != TAG_LIST)
        throw pybind11::value_error("Tag is not list");

    this->rootObj = rootObj;
    this->node = node;
}

std::vector<pybind11::object> NBTListTag::iterate() {
    std::vector<pybind11::object> tags;

    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
        nbt_node *currentNode = list_entry(pos, struct nbt_list, entry)->data;
//        if (currentNode->name == NULL)
//            throw pybind11::value_error("Consistency error 32314!");

//        tags.push_back(std::string(currentNode->name));
        if (currentNode->type == TAG_INT) {
            printf("Int\n");
        } else if (currentNode->type == TAG_COMPOUND) {
//            printf("Compound\n");
            tags.push_back(pybind11::cast(NBTCompoundTag(rootObj, currentNode)));
        } else if (currentNode->type == TAG_STRING) {
            tags.push_back(pybind11::cast(std::string(currentNode->payload.tag_string)));
//            printf("String\n");
//            tags.push_back(pybind11::cast(NBTCompoundTag(rootObj, currentNode)));
        } else {
            printf("Unknown\n");
        }
    }

    return tags;
}

NBTCompoundTag::NBTCompoundTag(NBTRoot rootObj, nbt_node *node) {
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type != TAG_COMPOUND)
        throw pybind11::value_error("Tag is not compound");

    this->rootObj = rootObj;
    this->node = node;
}

std::vector<std::string> NBTCompoundTag::keys() {
    std::vector<std::string> tags;
    if (node->type != TAG_COMPOUND)
        throw pybind11::value_error("Tag not compound");

    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
        nbt_node *currentNode = list_entry(pos, struct nbt_list, entry)->data;
        if (currentNode->name == NULL)
            throw pybind11::value_error("Consistency error 32314!");

        tags.push_back(std::string(currentNode->name));
    }

    return tags;
}

bool NBTCompoundTag::contains(std::string key) {
    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
        nbt_node *currentNode = list_entry(pos, struct nbt_list, entry)->data;
        if (key == currentNode->name)
            return true;
    }
    return false;
}

pybind11::object NBTCompoundTag::get(std::string key) {
    const struct list_head* pos;
    list_for_each(pos, &node->payload.tag_list->entry) {
        nbt_node *currentNode = list_entry(pos, struct nbt_list, entry)->data;
        if (key == currentNode->name) {
            if (currentNode->type == TAG_INT) return pybind11::cast(NBTIntTag(rootObj, currentNode));
            else if (currentNode->type == TAG_COMPOUND) return pybind11::cast(NBTCompoundTag(rootObj, currentNode));
            else if (currentNode->type == TAG_LIST) return pybind11::cast(NBTListTag(rootObj, currentNode));
            else if (currentNode->type == TAG_STRING) return pybind11::cast(std::string(currentNode->payload.tag_string));
            else throw pybind11::key_error("Unimplemented tag " + std::to_string(currentNode->type)); // TODO
        }
/*
        if (currentNode->type == TAG_INT) {
            return NBTInt
        }*/
    }
    throw pybind11::key_error("Compound doesn't contain tag " + key);
}

NBTIntTag::NBTIntTag(NBTRoot rootObj, nbt_node *node) {
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type != TAG_INT)
        throw pybind11::value_error("Tag is not int");

    this->rootObj = rootObj;
    this->node = node;
}

int NBTIntTag::value() {
    return this->node->payload.tag_int;
}

void NBTIntTag::set(int32_t value) {
    this->node->payload.tag_int = value;
}

NBTStringTag::NBTStringTag(NBTRoot rootObj, nbt_node *node) {
    if (node == NULL)
        throw pybind11::value_error("Tag doesn't exist");
    if (node->type != TAG_INT)
        throw pybind11::value_error("Tag is not string");

    this->rootObj = rootObj;
    this->node = node;
}

string NBTStringTag::value() {
    return std::string(this->node->payload.tag_string);
}

void NBTStringTag::set(string value) {
    this->node->payload.tag_string = value.c_str();
}

/*
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

    static NBTRoot from_bytes(py::bytes bytes) {
        std::string data = bytes;
        return NBTRoot(data.c_str(), data.size());
    }

    py::bytes to_bytes() {
        buffer buff = nbt_dump_binary(rootObj->root);
        return std::string((const char*)buff.data, (size_t)buff.len);
    }

    int64_t get_int(std::vector<std::string> path) {
        if (path.size() == 0)
            throw pybind11::value_error();
        std::string pathString = "";
        for(int i = 0 ; i < path.size() ; i++) {
            pathString += ".";
            pathString += path[i];
        }

        nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
        if (node == NULL)
            throw pybind11::value_error("Tag doesn't exist");
        if (node->type == TAG_INT)
            return node->payload.tag_int;
        if (node->type == TAG_LONG)
            return node->payload.tag_long;
        if (node->type == TAG_SHORT)
            return node->payload.tag_short;
        if (node->type == TAG_FLOAT)
            return (int64_t)node->payload.tag_float;
        if (node->type == TAG_DOUBLE)
            return (int64_t)node->payload.tag_double;
        throw pybind11::value_error("Tag not numerical");
    }

    void set_existing_int(std::vector<std::string> path, int value) {
        if (path.size() == 0)
            throw pybind11::value_error();
        std::string pathString = "";
        for(int i = 0 ; i < path.size() ; i++) {
            pathString += ".";
            pathString += path[i];
        }

        nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
        if (node == NULL)
            throw pybind11::value_error("Tag doesn't exist");
        if (node->type == TAG_INT || node->type == TAG_LONG || node->type == TAG_SHORT || node->type == TAG_FLOAT || node->type == TAG_DOUBLE) {
            node->payload.tag_int = value;
            node->type = TAG_INT;
            return;
        }
        throw pybind11::value_error("Tag not numerical");
    }

    NBTListTag get_existing_list(std::vector<std::string> path) {
        if (path.size() == 0)
            throw pybind11::value_error();
        std::string pathString = "";
        for(int i = 0 ; i < path.size() ; i++) {
            pathString += ".";
            pathString += path[i];
        }

        nbt_node *node = nbt_find_by_path(rootObj->root, pathString.c_str());
        if (node == NULL)
            throw pybind11::value_error("Tag doesn't exist");
        if (node->type != TAG_LIST)
            throw pybind11::value_error("Tag is not list");

        return NBTListTag(rootObj, node);
    }
};*/
/*
NBTListTag::NBTListTag(NBTRoot rootObj, nbt_node *node) {
    this->rootObj = rootObj;
    this->node = node;
}
*/
PYBIND11_MODULE(rabbitnbt, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: rabbitnbt

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    pybind11::class_<NBTRoot>(m, "NBTRoot")
        .def(pybind11::init(&NBTRoot::from_bytes))
        .def("get_int", &NBTRoot::get_int)
        .def("to_bytes", &NBTRoot::to_bytes)
        .def("set_existing_int", &NBTRoot::set_existing_int)
        .def("get_existing_list", &NBTRoot::get_existing_list)
        .def("list_tags", &NBTRoot::list_tags)

//        .def("contains", &NBTRoot::contains)
//        .def("get", &NBTRoot::get)
        .def("compound", &NBTRoot::compound);

    pybind11::class_<NBTTag>(m, "NBTTag");

    pybind11::class_<NBTListTag, NBTTag>(m, "NBTListTag")
        .def("iterate", &NBTListTag::iterate);

    pybind11::class_<NBTCompoundTag, NBTTag>(m, "NBTCompoundTag")
        .def("contains", &NBTCompoundTag::contains)
        .def("list", &NBTCompoundTag::keys)
        .def("get", &NBTCompoundTag::get);

    pybind11::class_<NBTIntTag, NBTTag>(m, "NBTIntTag")
        .def("value", &NBTIntTag::value)
        .def("set", &NBTIntTag::set);


    m.def("from_bytes", &NBTRoot::from_bytes);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
