#include "LevelSystem.h"
#include "misc.h"

#include <fstream>


vector<vector<unique_ptr<Sprite>>> LevelSystem::_tiles;
vector<RectangleShape> LevelSystem::_objects;

vector<unique_ptr<Texture>> tileSet;

vector<string> tokenizeString(string str, char delim) {
    vector<string> temp;

    string tempStr;

    for (int i = 0; i < str.length(); i++) {
        if (str.at(i) == delim) {
            temp.push_back(tempStr);
            tempStr = "";
        } else {
            tempStr.push_back(str.at(i));
        }
    }

    return temp;
}

void LevelSystem::loadLevelFile(const char* path) {
    string buffer;

    if (ifstream f(path); f.good()) {
        f.seekg(0, ios::end);
        buffer.resize(f.tellg());
        f.seekg(0);
        f.read(&buffer[0], buffer.size());
        f.close();
    } else {
        throw string("Couldn't open level file: ") + path;
    }

    tileSet.clear();

    for (int i = 0; i < 87; i++) {
        auto t = make_unique<Texture>();
        t->loadFromFile("res/tiles/" + to_string(i) + ".png");
        tileSet.push_back(move(t));
    }

    XMLDocument doc;
    doc.LoadFile(path);

    int map_width = doc.FirstChildElement("map")->FindAttribute("width")->Int64Value();

    XMLNode* layer = doc.FirstChildElement("map")->FirstChildElement("layer");
    XMLNode* objects = doc.FirstChildElement("map")->FirstChildElement("objectgroup");

    while (layer != nullptr) {
        vector<string> map_data = tokenizeString(layer->FirstChildElement("data")->GetText(), ',');

        vector<unique_ptr<Sprite>> temp_tiles;

        float x = 0;
        float y = 0;

        for (int i = 0; i < map_data.size(); ++i) {
            if (i % map_width == 0) {
                x = 0;
                y++;
            }

            int tile = atoi(map_data.at(i).c_str());

            if (tile == 0) {
                auto s = make_unique<Sprite>();
                temp_tiles.push_back(move(s));
                x++;
                continue;
            }

            auto s = make_unique<Sprite>();
            s->setTexture(*tileSet.at(tile-1));
            s->setPosition(misc::getIsometric(x, y, map_width));
            s->setScale({sprite_scale, sprite_scale});
            temp_tiles.push_back(move(s));
            x++;
        }

        _tiles.push_back(move(temp_tiles));

        layer = layer->NextSiblingElement("layer");
    }

    while (objects != nullptr) {
        auto object = objects->FirstChildElement("object");

        while (object != nullptr) {
            auto x = stoi(object->Attribute("x")) / 17;
            auto y = stoi(object->Attribute("y")) / 17;

            float newX = (x - y) * (32 / 2) * sprite_scale;
            float newY = (y + x) * (17 / 2) * sprite_scale;

            auto o = new RectangleShape();
            o->setPosition(newX, newY);
            o->setSize({static_cast<float>(stoi(object->Attribute("width")) * sprite_scale + 12),
                    static_cast<float>(stoi(object->Attribute("height")) * sprite_scale + 12)});

            _objects.push_back(*o);

            object = object->NextSiblingElement("object");
        }

        objects = objects->NextSiblingElement("objectgroup");
    }
}

RectangleShape LevelSystem::getObject(int index) {
    int temp = 0;

    for (const auto & _object : _objects) {
        if (temp < index) {
            temp++;
            continue;
        }
        return _object;
    }

    throw length_error("There is no object at that index");
}

void LevelSystem::Render(RenderWindow &window) {
    for (auto & _tile : _tiles) {
        for (const auto & _sub_tile : _tile) {
            window.draw(*_sub_tile);
        }
    }
}
