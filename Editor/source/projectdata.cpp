#include "projectdata.h"

ProjectData::ProjectData() {}

// Asset stuff
bool ProjectData::asset_name_exists(std::string name) {
  return name_to_asset_map.count(name) > 0;
}

void ProjectData::rename_asset(std::string old_name, std::string new_name) {
  AssetEntry* entry = name_to_asset_map.at(old_name);
  name_to_asset_map.erase(old_name);
  name_to_asset_map.insert({new_name, entry});
}

bool ProjectData::asset_id_exists(int id) {
  return asset_db.count(id) > 0;
}

AssetEntry* ProjectData::get_asset(int id) {
  if(!asset_id_exists(id)) return nullptr;
  return asset_db.at(id);
}



// Database stuff
void ProjectData::clear_database() {
  // delete all heap-allocated asset entries
    for(std::pair<int, AssetEntry*> kv : asset_db)
        delete kv.second;

  // clear maps
  asset_db.clear();
  name_to_asset_map.clear();

  // clear held project config node
  config_node = pugi::xml_node();
}

void ProjectData::load_entries_into_db(pugi::xml_node &root, std::string subset_name, ASSET_TYPE type) {
  for(pugi::xml_node& node : root.child(subset_name.c_str()).children()){
      load_entry_into_db(node, type);
  }
}

void ProjectData::load_entry_into_db(pugi::xml_node &node, ASSET_TYPE type) {
  AssetEntry* asset_ptr = new AssetEntry;
  asset_ptr->id = generate_new_unique_id();
  asset_ptr->node = node;
  asset_ptr->type = type;
  asset_ptr->name = asset_ptr->name = node.attribute("name").as_string();
  asset_db.insert({asset_ptr->id, asset_ptr});
  name_to_asset_map.insert({asset_ptr->name, asset_ptr});
}

bool ProjectData::load_project_file_into_database(std::string path) {
  clear_database();

  pugi::xml_parse_result result = project_xml_document.load_file(path.c_str());
  if (result) {
    printf("Parsed project %s without errors!\n", path.c_str());
  }else{
    printf("Error: %s at %lld", result.description(), result.offset);
    return false;
  }

  pugi::xml_node root_node = project_xml_document.child("project");

  // load project name
  game_name = root_node.child("name").text().as_string();

  // Load project into databases
  load_entries_into_db(root_node, "objects",     ASSET_TYPE::OBJECT);
  load_entries_into_db(root_node, "sounds",      ASSET_TYPE::SOUND);
  load_entries_into_db(root_node, "sprites",     ASSET_TYPE::SPRITE);
  load_entries_into_db(root_node, "tilesets",    ASSET_TYPE::TILESET);
  load_entries_into_db(root_node, "backgrounds", ASSET_TYPE::BACKGROUND);
  load_entries_into_db(root_node, "rooms",       ASSET_TYPE::ROOM);

  printf("Loaded database. Size: %lld\n", asset_db.size());

  // load config
  config_node = root_node.child("window");

  return true;
}

std::unordered_map<int, AssetEntry*>* ProjectData::get_map() {
  return &asset_db;
}



// Utilities
int ProjectData::generate_new_unique_id() {
  int id = static_cast<int>(asset_db.size());
  do id++; while (asset_db.count(id) > 0);
  return id;
}

pugi::xml_node* ProjectData::get_config_node() {
  return &config_node;
}



// PROJECT LOADING AND SAVING
bool ProjectData::load_project_from_file_into_database(std::string& filename) {
  // does nothing if user escapes the file dialog
  if(filename.compare("") == 0) return false;
  QString qfilename = QString(filename.c_str());

  /* If the .lumi file is invalid,
   * throw a message box displaying the error. */
  if(!load_project_file_into_database(filename)) {
    return false;
  } else {
    current_loaded_file_path = filename;
    return true;
  }
}

/*
 * Saving:
 * - check if projectdata's save file location string is not empty
 * - check if projectdata's save file location exists on disk
 * if yes:
 * - overwrite the save file's location with new document (moves all temp files into save directory)
 * if not:
 * - show dialog asking for a new save location
 */
bool ProjectData::save_current_project() {
  // not implemented
  return false;
}

