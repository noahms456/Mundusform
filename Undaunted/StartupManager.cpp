#include <Undaunted\ConfigUtils.h>
#include "StartupManager.h"
#include "RSJparser.tcc"
#include <filesystem>
#include <algorithm>
#include <string>
#include <Undaunted\FormRefList.h>
#include <Undaunted\LocationUtils.h>
#include <Undaunted\NavmeshTool.h>

namespace Undaunted {
	RSJresource currentfile;
	void LoadJson(const char* filepath)
	{
		_MESSAGE("Loading %s", filepath);
		std::ifstream t(filepath);
		t.seekg(0, std::ios::end);
		size_t size = t.tellg();
		std::string buffer(size, ' ');
		t.seekg(0);
		t.read(&buffer[0], size);
		RSJresource my_json(buffer.c_str());
		currentfile = my_json;
	}

	void LoadSettings()
	{
		LoadJson("Data/Undaunted/Settings.json");
		RSJresource settings = currentfile; 

		auto data = settings.as_array();
		_MESSAGE("size: %i", data.size());
		for (int i = 0; i < data.size(); i++)
		{
			auto inner = data[i].as_array();
			std::string key = inner[0].as<std::string>("default string");
			std::string value = inner[1].as<std::string>("default string");
			AddConfigValue(key.c_str(), value.c_str());
		}
	}

	void LoadRifts()
	{
		DataHandler* dataHandler = GetDataHandler();
		_MESSAGE("Loading Rifts...");
		std::string path = "Data/Undaunted/Rifts";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			auto filename = entry.path().u8string();
			_MESSAGE("file: %s", filename.c_str());
			if (entry.is_regular_file())
			{
				LoadJson(filename.c_str());
				RSJresource settings = currentfile;
				auto data = settings.as_array();
				_MESSAGE("size: %i", data.size());
				FormRefList reflist = FormRefList();
				for (int i = 0; i < data.size(); i++)
				{
					std::string formid = data[i][0].as<std::string>("formid");

					//_MESSAGE("formid: %s", formid);
					double xpos = data[i][1].as<double>();
					double ypos = data[i][2].as<double>();
					double zpos = data[i][3].as<double>();
					double xrot = data[i][4].as<double>();
					double yrot = data[i][5].as<double>();
					double zrot = data[i][6].as<double>();
					int scale = data[i][7].as<int>();

					FormRef formref = FormRef();
					if (formid.compare("RiftExit") == 0) {
						const ModInfo* modInfo = dataHandler->LookupModByName("Undaunted.esp");
						formref.formId = (modInfo->modIndex << 24) + 745728;//RiftDoor
					}
					else
					{
						formref.formId = std::stoul(formid.c_str(), nullptr, 16);
					}
					formref.pos = NiPoint3(xpos, ypos, zpos);
					formref.rot = NiPoint3(xrot, yrot, zrot);
					formref.scale = scale;
					reflist.AddItem(formref);
				}
				AddRift(reflist);
			}
		}
	}

	void LoadBlocks()
	{
		DataHandler* dataHandler = GetDataHandler();
		_MESSAGE("Loading Blocks...");
		std::string path = "Data/Undaunted/Blocks";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			auto filename = entry.path().u8string();
			_MESSAGE("file: %s", filename.c_str());
			if (entry.is_regular_file())
			{
				LoadJson(filename.c_str());
				RSJresource settings = currentfile;
				auto data = settings.as_array();
				_MESSAGE("size: %i", data.size());
				auto forms = data[0].as_array();
				_MESSAGE("size: %i", forms.size());
				FormRefList reflist = FormRefList();
				for (int j = 0; j < forms.size(); j++)
				{
					std::string esp = forms[j][0].as<std::string>("esp");
					const ModInfo* modInfo = dataHandler->LookupModByName(esp.c_str());
					int form = forms[j][1].as<int>(0);
					if (modInfo != NULL)
					{
						form = (modInfo->modIndex << 24) + form;
					}
					double xpos = forms[j][2].as<double>();
					double ypos = forms[j][3].as<double>();
					double zpos = forms[j][4].as<double>();
					double xrot = forms[j][5].as<double>();
					double yrot = forms[j][6].as<double>();
					double zrot = forms[j][7].as<double>();
					int scale = forms[j][8].as<int>();
					_MESSAGE("Block: %s, %i", esp.c_str(), form);
					FormRef ref = FormRef();
					ref.formId = form;
					ref.pos = NiPoint3(xpos, ypos, zpos);
					ref.rot = NiPoint3(xrot, yrot, zrot);
					ref.scale = scale;
				}
				auto nav = data[1].as_array();
				TileList navlist = TileList();
				for (int j = 0; j < nav.size(); j++)
				{
					int xpos = forms[j][0].as<int>();
					int ypos = forms[j][1].as<int>();
					int zpos = forms[j][2].as<int>();
					Tile tile = Tile(xpos, ypos, zpos, 1);
					navlist.AddItem(tile);
				}
				auto exits = data[2].as_array();
				TileList exitslist = TileList();
				for (int j = 0; j < exits.size(); j++)
				{
					int xpos = forms[j][0].as<int>();
					int ypos = forms[j][1].as<int>();
					int zpos = forms[j][2].as<int>();
					Tile tile = Tile(xpos, ypos, zpos, 1);
					exitslist.AddItem(tile);
				}

			}
		}
	}
}