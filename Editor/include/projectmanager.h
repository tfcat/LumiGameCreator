#pragma once
#include <ui_mainwindow.h>
#include <external/pugixml.hpp>
#include <projectdata.h>

struct xml_string_writer: pugi::xml_writer {
    std::string result;
    virtual void write(const void* data, size_t size) {
        result.append(static_cast<const char*>(data), size);
    }
};

class ProjectManager
{
public:
  // singleton stuff
  static ProjectManager& fetch() {
    static ProjectManager instance;
    return instance;
  }

  ProjectManager() {}
  ProjectManager(ProjectManager const&);

  Ui::MainWindow* get_ui();
  void pass_ui(Ui::MainWindow* ui);
  void show_error_message(QString text);

  void open_asset_in_tab(AssetEntry* asset);
  void open_project_configuration_tab();

private:
  Ui::MainWindow* ui;
};

