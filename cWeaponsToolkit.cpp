#include "cWeaponsToolkit.h"
#include <Windows.h>
#include <string>
#include <vector> 
#include <locale>
#include <codecvt>
#include <wx/msgdlg.h>
#include "vendor/rapidxml/rapidxml.hpp"
#include <fstream>

int cWeaponsToolkit::getWeaponCount() {
	int count = 0;

	for (const char* i : generatedWeapon->nativeWeapons) {
		if (i)
			count += 1;
	}
	return count;
}

int cWeaponsToolkit::getWeaponComponentCount() {
	int count = 0;

	for (const char* i : generatedWeapon->nativeComponents) {
		if (i)
			count += 1;
	}
	return count;

}

int cWeaponsToolkit::getAmmoInfoCount() {
	int count = 0;

	for (const char* i : generatedWeapon->nativeAmmoInfos) {
		if (i)
			count += 1;
	}
	return count;
}

int cWeaponsToolkit::getAudioItemsCount()
{
	int count = 0;

	for (const char* i : generatedWeapon->audioItems) {
		if (i)
			count += 1;
	}
	return count;
}

int cWeaponsToolkit::getAmmoTypesCount()
{
	int count = 0;

	for (const char* i : generatedWeapon->ammoTypes) {
		if (i)
			count += 1;
	}
	return count;
}

int cWeaponsToolkit::getDamageTypesCount()
{
	int count = 0;

	for (const char* i : generatedWeapon->damageTypes) {
		if (i)
			count += 1;
	}
	return count;
}

void cWeaponsToolkit::onImportDirectoryChanged(wxCommandEvent& evt) {
	const std::string tmpDir = importerDirectoryPicker->GetPath().ToStdString();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(tmpDir);

	searchForWeaponAssets(wide);
	validateWeaponAssets();
}

void cWeaponsToolkit::onWeaponTemplateChanged(wxCommandEvent& evt)
{
	generatedWeapon->setWeaponTemplate(std::string(evt.GetString()));
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node;
	std::ifstream theFile(std::string( "templates/weapons/" + generatedWeapon->getWeaponTemplate() + "/weapons.meta"));
	std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);
	
	root_node = doc.first_node("CWeaponInfoBlob");

	for (rapidxml::xml_node<>* infos_1 = root_node->first_node("Infos"); infos_1; infos_1 = infos_1->next_sibling())
	{
		for (rapidxml::xml_node<>* item_1 = infos_1->first_node("Item"); item_1; item_1 = item_1->next_sibling())
		{
			for (rapidxml::xml_node<>* infos_2 = item_1->first_node("Infos"); infos_2; infos_2 = infos_2->next_sibling())
			{
				for (rapidxml::xml_node<>* item_2 = infos_2->first_node("Item"); item_2; item_2 = item_2->next_sibling())
				{
					for (rapidxml::xml_node<>* weapon_node = item_2->first_node("Name"); weapon_node; weapon_node = weapon_node->next_sibling())
					{
						if (std::string(weapon_node->name()) == "Model") {
							generatedWeapon->setWeaponModel(weapon_node->value());
							weaponModelTextCtrl->SetValue(weapon_node->value());
						}
						else if (std::string(weapon_node->name()) == "Audio") {
							//wxString s1;
							//s1.Printf(wxT("Audio Item: %s"), weapon_node->value());
							//wxMessageBox(s1, wxT("vWeaponToolkit"), wxICON_INFORMATION);
							generatedWeapon->setAudioItem(weapon_node->value());
							audioItemComboBox->SetValue(wxString(weapon_node->value()));
						}
						else if (std::string(weapon_node->name()) == "DamageType") {
							generatedWeapon->setWeaponDamageType(weapon_node->value());
							damageTypesComboxBox->SetValue(wxString(weapon_node->value()));
						}
						else if (std::string(weapon_node->name()) == "Damage") {
							if (std::string(weapon_node->first_attribute()->name()) == "value") {
								generatedWeapon->setWeaponDamageType(weapon_node->first_attribute()->value());
								weaponDamageTextCtrl->SetValue(wxString(weapon_node->first_attribute()->value()));
							}
						}
						else if (std::string(weapon_node->name()) == "WeaponRange") {
							if (std::string(weapon_node->first_attribute()->name()) == "value") {
								generatedWeapon->setWeaponRange(std::stof(weapon_node->first_attribute()->value()));
								weaponRangeTextCtrl->SetValue(wxString(weapon_node->first_attribute()->value()));
							}
						}
						else if (std::string(weapon_node->name()) == "AmmoInfo") {
							if (std::string(weapon_node->first_attribute()->name()) == "ref") {
								generatedWeapon->setAmmoType(weapon_node->first_attribute()->value());
								ammoTypesComboBox->SetValue(wxString(weapon_node->first_attribute()->value()));
							}
						}
						else if (std::string(weapon_node->name()) == "AnimReloadRate") {
							if (std::string(weapon_node->first_attribute()->name()) == "value") {
								generatedWeapon->setWeaponReloadSpeedMultiplier(std::stof(weapon_node->first_attribute()->value()));
								weaponReloadModifierTextCtrl->SetValue(wxString(weapon_node->first_attribute()->value()));
							}
						}
						//todo weaponLOD in weaponarchtypes.meta
						//todo weapon fire rate in weaponanimations.meta
					}
				}
			}
		}
	}
}

void cWeaponsToolkit::onWeaponNameChanged(wxCommandEvent& evt)
{
	generatedWeapon->setWeaponName(std::string(evt.GetString()));
}

void cWeaponsToolkit::onWeaponIdChanged(wxCommandEvent& evt)
{
	generatedWeapon->setWeaponId(std::string(evt.GetString()));
}

void cWeaponsToolkit::onWeaponModelChanged(wxCommandEvent& evt)
{
	generatedWeapon->setWeaponModel(std::string(evt.GetString()));
	validateWeaponAssets();
}

void cWeaponsToolkit::onCreateWeaponNextButtonChanged(wxCommandEvent& evt)
{
	if (!generatedWeapon->getValidWeaponModelFound()) {
		wxMessageBox(wxT("No valid weapon model found."), wxT("vWeaponToolkit"), wxICON_ERROR);
	}
	else {
		menuTabs->SetSelection(1);
	}
}

void cWeaponsToolkit::validateWeaponAssets()
{
	for (int i = 0; i < filesFoundListCtrl->GetItemCount(); i++) {
		filesFoundListCtrl->SetItemTextColour(i, wxColour(*wxBLACK));
	}

	for (int i = 0; i < filesFoundListCtrl->GetItemCount(); i++) {
		wxString s = filesFoundListCtrl->GetItemText(i);
		if (s == generatedWeapon->getWeaponModel() + ".ydr") {
			//Found ydr model, now look for _hi ydr & textures.
			filesFoundListCtrl->SetItemTextColour(i, wxColour(70,200,0));
			generatedWeapon->setValidWeaponModelFound(true);

			for (int i = 0; i < filesFoundListCtrl->GetItemCount(); i++) {
				wxString s = filesFoundListCtrl->GetItemText(i);
				if (s == generatedWeapon->getWeaponModel() + "_hi.ydr") {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(70, 200, 0));
					generatedWeapon->addWeaponAsset(std::string(generatedWeapon->getWeaponModel() + "_hi.ydr"));
				}
				else if (s == generatedWeapon->getWeaponModel() + ".ytd") {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(70, 200, 0));
					generatedWeapon->addWeaponAsset(std::string(generatedWeapon->getWeaponModel() + ".ytd"));
				}
				else if (s == generatedWeapon->getWeaponModel() + "+hi.ytd") {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(70, 200, 0));
					generatedWeapon->addWeaponAsset(std::string(generatedWeapon->getWeaponModel() + "+hi.ytd"));
				}
				//Addon Mods/Components
				else if (s.find("_mag1") != std::string::npos) {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(0, 70, 200));
					generatedWeapon->addWeaponAsset(std::string(s));
				}
				else if (s.find("_mag2") != std::string::npos) {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(0, 70, 200));
					generatedWeapon->addWeaponAsset(std::string(s));
				}
				else if (s.find("_afgrip") != std::string::npos) {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(0, 70, 200));
					generatedWeapon->addWeaponAsset(std::string(s));
				}
				else if (s.find("_supp") != std::string::npos) {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(0, 70, 200));
					generatedWeapon->addWeaponAsset(std::string(s));
				}
				else if (s.find("_medium") != std::string::npos) {
					filesFoundListCtrl->SetItemTextColour(i, wxColour(0, 70, 200));
					generatedWeapon->addWeaponAsset(std::string(s));
				}
				else if(s != generatedWeapon->getWeaponModel() + ".ydr")
				{
					filesFoundListCtrl->SetItemTextColour(i, wxColour(200, 40, 0));
				}
			}
		}
	}
}

wxString cWeaponsToolkit::removeWeaponFileExtension(wxString s)
{
	std::string ext_ydr = ".ydr";

	size_t pos_ydr = s.find(ext_ydr);
	if (pos_ydr != std::string::npos)
	{
		// If found then erase it from string
		s.erase(pos_ydr, ext_ydr.length());
	}

	std::string ext_ytd = ".ytd";

	size_t pos_ytd = s.find(ext_ytd);
	if (pos_ytd != std::string::npos)
	{
		// If found then erase it from string
		s.erase(pos_ytd, ext_ytd.length());
	}
	return s;
}

void cWeaponsToolkit::onAddComponent(wxCommandEvent& evt)
{
	cWeaponComponent* component = new cWeaponComponent;
	std::string componentName = std::string(weaponComponentsComboBox->GetStringSelection());

	if (componentName == "")
		componentName = "COMPONENT_AT_RAILCOVER_01";

	component->setComponentTemplate(std::string(weaponComponentsComboBox->GetStringSelection()));
	component->setComponentName(componentName);
	component->setModelName(std::string(componentModelNameTextCtrl->GetValue()));
	try {
		component->setClipSize(std::stoi(std::string(componentClipSizeTextCtrl->GetValue())));
	}
	catch (const std::invalid_argument& e) {
		component->setClipSize(0);
	}
	catch (const std::out_of_range& e) {
		component->setClipSize(0);
	}	
	component->setAmmoInfo(std::string(weaponAmmoInfoComboBox->GetStringSelection()));

	generatedWeapon->components->push_back(component);
	weaponComponentsListCtrl->InsertItem(0, component->getComponentName());
}
	
void cWeaponsToolkit::onRemoveComponent(wxCommandEvent& evt)
{
	weaponComponentsListCtrl->DeleteItem(selectedComponent);

	cWeaponComponent* component = nullptr;

	try {
		component = generatedWeapon->components->at(selectedComponent);

		if (component)
			generatedWeapon->components->erase(generatedWeapon->components->begin() + selectedComponent);
	}
	catch (const std::out_of_range& e) {
		wxMessageBox("No weapon component to remove.", wxT("vWeaponToolkit"), wxICON_EXCLAMATION);
	}
}

void cWeaponsToolkit::onSelectComponent(wxCommandEvent& evt)
{
	selectedComponent = weaponComponentsListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent) {
		componentNameTextCtrl->SetValue(currentComponent->getComponentName());
		componentModelNameTextCtrl->SetValue(currentComponent->getModelName());
		componentLODTextCtrl->SetValue(std::to_string(currentComponent->getModelLOD()));

		if (currentComponent->getClipSize()) {
			componentClipSizeTextCtrl->SetValue(std::to_string(currentComponent->getClipSize()));
			componentClipSizeTextCtrl->Enable(true);
			componentClipSizeTextCtrl->SetEditable(true);

			weaponAmmoInfoComboBox->Enable(true);
			weaponAmmoInfoComboBox->SetEditable(true);
		}
		else {
			componentClipSizeTextCtrl->SetValue("");
			componentClipSizeTextCtrl->Enable(false);
			componentClipSizeTextCtrl->SetEditable(false);

			weaponAmmoInfoComboBox->Enable(false);
			weaponAmmoInfoComboBox->SetEditable(false);
		}

		weaponAmmoInfoComboBox->SetValue(currentComponent->getAmmoInfo());
	}
}

void cWeaponsToolkit::onComponentTemplateChanged(wxCommandEvent& evt)
{	
	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent)
		currentComponent->setComponentTemplate(std::string(evt.GetString()));

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node;
	std::ifstream theFile(std::string("templates/components/" + evt.GetString() + "/weaponcomponents.meta"));
	std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("CWeaponComponentInfoBlob");

	componentClipSizeTextCtrl->SetValue("");
	componentClipSizeTextCtrl->Enable(false);
	componentClipSizeTextCtrl->SetEditable(false);

	weaponAmmoInfoComboBox->SetValue("");
	weaponAmmoInfoComboBox->Enable(false);
	weaponAmmoInfoComboBox->SetEditable(false);

	for (rapidxml::xml_node<>* infos_1 = root_node->first_node("Infos"); infos_1; infos_1 = infos_1->next_sibling())
	{
		for (rapidxml::xml_node<>* item_1 = infos_1->first_node("Item"); item_1; item_1 = item_1->next_sibling()) {
			for (rapidxml::xml_node<>* component_node = item_1->first_node("Name"); component_node; component_node = component_node->next_sibling()) {
				if (std::string(component_node->name()) == "Name") {
					if (currentComponent)
						currentComponent->setComponentName(component_node->value());

					componentNameTextCtrl->SetValue(component_node->value());
				}
				else if (std::string(component_node->name()) == "Model") {
					if (currentComponent)
						currentComponent->setModelName(component_node->value());

					componentModelNameTextCtrl->SetValue(component_node->value());
				}

				if (std::string(component_node->name()) == "ClipSize") {
					if (std::string(component_node->first_attribute()->name()) == "value") {
						if (currentComponent)
							currentComponent->setClipSize((int)component_node->value());

						componentClipSizeTextCtrl->SetValue(component_node->first_attribute()->value());
						componentClipSizeTextCtrl->Enable(true);
						componentClipSizeTextCtrl->SetEditable(true);

						weaponAmmoInfoComboBox->Enable(true);
						weaponAmmoInfoComboBox->SetEditable(true);
					}
				}

				if (std::string(component_node->name()) == "AmmoInfo") {
					if (currentComponent)
						currentComponent->setAmmoInfo(component_node->value());

					weaponAmmoInfoComboBox->SetValue(component_node->value());
				}

				//todo modelLOD
			}
		}
	}
}

void cWeaponsToolkit::onComponentNameChanged(wxCommandEvent& evt)
{
	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent) {
		currentComponent->setComponentName(std::string(evt.GetString()));

		weaponComponentsListCtrl->SetItemText(selectedComponent, evt.GetString());
	}
}

void cWeaponsToolkit::onComponentModelNameChanged(wxCommandEvent& evt)
{
	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent) {
		currentComponent->setModelName(std::string(evt.GetString()));
	}
}

void cWeaponsToolkit::onComponentLODChanged(wxCommandEvent& evt)
{
	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent) {
		try {
			int intLOD = std::stoi(std::string(evt.GetString()));
			currentComponent->setModelLOD(intLOD);
		}
		catch (const std::invalid_argument& e) {
			currentComponent->setModelLOD(0.0);
		}
		catch (const std::out_of_range& e) {
			currentComponent->setModelLOD(0.0);
		}
	}
}

void cWeaponsToolkit::onComponentClipSizeChanged(wxCommandEvent& evt)
{
	cWeaponComponent* currentComponent = nullptr;

	try {
		currentComponent = generatedWeapon->components->at(selectedComponent);
	}
	catch (const std::out_of_range& e) {

	}

	if (currentComponent) {
		try {
			currentComponent->setClipSize(std::stoi(std::string(evt.GetString())));
		}
		catch (const std::invalid_argument& e) {
			currentComponent->setClipSize(0);
		}
		catch (const std::out_of_range& e) {
			currentComponent->setClipSize(0);
		}
	}
}

void cWeaponsToolkit::searchForWeaponAssets(const std::wstring& directory)
{
	std::wstring fullDir = directory + L"\\*";
	WIN32_FIND_DATAW file;
	HANDLE search_handle = FindFirstFileW(fullDir.c_str(), &file);
	if (search_handle != INVALID_HANDLE_VALUE)
	{
		std::vector<std::wstring> directories;
		int filesFound = 0;
		filesFoundListCtrl->DeleteAllItems();
		do
		{
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((!lstrcmpW(file.cFileName, L".")) || (!lstrcmpW(file.cFileName, L"..")))
					continue;
			}

			fullDir = directory + L"\\" + std::wstring(file.cFileName);

			if (std::wstring(file.cFileName).find(L'.ytd') != std::string::npos || std::wstring(file.cFileName).find(L'.ydr') != std::string::npos) {
				filesFoundListCtrl->InsertItem(0, file.cFileName);
				filesFound += 1;
			}

			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				directories.push_back(fullDir);
		} while (FindNextFileW(search_handle, &file));
		FindClose(search_handle);

		if (filesFound == 0) {			
			int index = filesFoundListCtrl->InsertItem(0, "No files found.");
			filesFoundListCtrl->SetItemTextColour(index, wxColour(200, 40, 0));
		}
	}
}

cWeaponsToolkit::cWeaponsToolkit() : wxFrame(nullptr, wxID_ANY, "vWeaponsToolkit", wxPoint((wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / 2) - (800/2), (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / 2) - (500 /2)), wxSize(windowWidth, windowHeight))
{
	// Initialize wxWidgets Menu.

	// MenuBar
	wxMenuBar* menuBar = new wxMenuBar;
	
	wxMenu* fileMenu = new wxMenu;
	fileMenu->Append(wxID_ANY, wxT("&New Add-on Weapon"));
	fileMenu->Append(wxID_EXIT);
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_ANY, wxT("&Created by Robbster"));

	wxMenu* emptyMenu = new wxMenu;

	menuBar->Append(fileMenu, "&File");;
	SetMenuBar(menuBar);

	// Top-level panel to hold all the contents of the frame
	wxPanel* panel = new wxPanel(this, wxID_ANY);	

	// Tabs
	menuTabs = new wxAuiNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
	createWeaponPanel = new wxPanel(menuTabs, wxID_ANY);
	menuTabs->AddPage(createWeaponPanel, L"Create Add-on Weapon");

	configTab = new wxPanel(menuTabs, wxID_ANY);
	menuTabs->AddPage(configTab, L"Configuration");

	componentsTab = new wxPanel(menuTabs, wxID_ANY);
	menuTabs->AddPage(componentsTab, L"Components");

	exportTab = new wxPanel(menuTabs, wxID_ANY);
	menuTabs->AddPage(exportTab, L"Export");

	// Menu Sizers
	wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
	panelSizer->Add(menuTabs, 1, wxEXPAND);
	panel->SetSizer(panelSizer);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(panel, 1, wxEXPAND);
	SetSizerAndFit(topSizer);
	SetMinSize(wxSize(windowWidth, windowHeight));
	SetMaxSize(wxSize(windowWidth, windowHeight));


	//TAB 1 - Create Add-on Weapon
	filesFoundListCtrl = new wxListCtrl(createWeaponPanel, wxID_ANY, wxPoint(20, 40), wxSize(200, 320), wxLC_REPORT, wxDefaultValidator);
	wxListItem col0;
	col0.SetId(0);
	col0.SetText(_("Files Found"));
	col0.SetWidth(200);
	filesFoundListCtrl->InsertColumn(0, col0);

	importerDirectoryPicker = new wxDirPickerCtrl(createWeaponPanel, wxID_ANY, "Import Folder", "", wxPoint(20, 370), wxSize(625, 25));
	wxButton* nextButton = new wxButton(createWeaponPanel, wxID_ANY, "Next", wxPoint(675, 370));

	wxStaticText* weaponTemplateStaticText = new wxStaticText(createWeaponPanel, wxID_ANY, "Select Weapon Template", wxPoint(250, 40));
	wxComboBox* weaponTemplate = new wxComboBox(createWeaponPanel, wxID_ANY, "WEAPON_ASSAULTRIFLE", wxPoint(250, 60), wxSize(175, 25));
	weaponTemplate->Append(wxArrayString(cWeaponsToolkit::getWeaponCount(), generatedWeapon->nativeWeapons));

	wxStaticText* weaponNameStaticText = new wxStaticText(createWeaponPanel, wxID_ANY, "Weapon Name:", wxPoint(250, 100));
	weaponNameTextCtrl = new wxTextCtrl(createWeaponPanel, wxID_ANY, "AK-47", wxPoint(250,120), wxSize(175, 25));

	wxStaticText* weaponIdStaticText = new wxStaticText(createWeaponPanel, wxID_ANY, "Weapon ID:", wxPoint(250, 160));
	weaponIdTextCtrl = new wxTextCtrl(createWeaponPanel, wxID_ANY, "WEAPON_AK47", wxPoint(250,180), wxSize(175, 25));

	wxStaticText* weaponModelStaticText = new wxStaticText(createWeaponPanel, wxID_ANY, "Weapon Model:", wxPoint(250, 220));
	weaponModelTextCtrl = new wxTextCtrl(createWeaponPanel, wxID_ANY, "w_ar_assaultrifle", wxPoint(250,240), wxSize(175, 25));

	//Event Handlers
	importerDirectoryPicker->Bind(wxEVT_COMMAND_DIRPICKER_CHANGED, &cWeaponsToolkit::onImportDirectoryChanged, this);
	weaponTemplate->Bind(wxEVT_COMBOBOX, &cWeaponsToolkit::onWeaponTemplateChanged, this);
	nextButton->Bind(wxEVT_BUTTON, &cWeaponsToolkit::onCreateWeaponNextButtonChanged, this);
	weaponNameTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onWeaponNameChanged, this);
	weaponIdTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onWeaponIdChanged, this);
	weaponModelTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onWeaponModelChanged, this);


	//TAB 2 - Configuration
	wxStaticText* audioItemStaticText = new wxStaticText(configTab, wxID_ANY, "Select Audio Item", wxPoint(20, 30));
	audioItemComboBox = new wxComboBox(configTab, wxID_ANY, "AUDIO_ITEM_PISTOL", wxPoint(20, 50), wxSize(175, 25));
	audioItemComboBox->Append(wxArrayString(cWeaponsToolkit::getAudioItemsCount(), generatedWeapon->audioItems));

	wxStaticText* weaponDamageStaticText = new wxStaticText(configTab, wxID_ANY, "Weapon Damage:", wxPoint(20, 90));
	weaponDamageTextCtrl = new wxTextCtrl(configTab, wxID_ANY, "0.0", wxPoint(20, 110), wxSize(175, 25));

	wxStaticText* weaponRangeStaticText = new wxStaticText(configTab, wxID_ANY, "Weapon Range:", wxPoint(20, 150));
	weaponRangeTextCtrl = new wxTextCtrl(configTab, wxID_ANY, "120.0", wxPoint(20, 170), wxSize(175, 25));

	wxStaticText* ammoTypesStaticText = new wxStaticText(configTab, wxID_ANY, "Ammo Type", wxPoint(20, 210));
	ammoTypesComboBox = new wxComboBox(configTab, wxID_ANY, "AMMO_PISTOL", wxPoint(20, 230), wxSize(175, 25));
	ammoTypesComboBox->Append(wxArrayString(cWeaponsToolkit::getAmmoTypesCount(), generatedWeapon->ammoTypes));

	wxStaticText* weaponLODStaticText = new wxStaticText(configTab, wxID_ANY, "LOD:", wxPoint(20, 270));
	weaponLODTextCtrl = new wxTextCtrl(configTab, wxID_ANY, "400.0", wxPoint(20, 290), wxSize(175, 25));

	wxStaticText* weaponReloadModifierStaticText = new wxStaticText(configTab, wxID_ANY, "Reload Speed Modifier:", wxPoint(20, 330));
	weaponReloadModifierTextCtrl = new wxTextCtrl(configTab, wxID_ANY, "1.0", wxPoint(20, 350), wxSize(175, 25));

	wxStaticText* weaponFireRateModifierStaticText = new wxStaticText(configTab, wxID_ANY, "Fire Rate Modifier:", wxPoint(220, 30));
	weaponFireRateModifierTextCtrl = new wxTextCtrl(configTab, wxID_ANY, "1.0", wxPoint(220, 50), wxSize(175, 25));

	wxStaticText* damageTypesStaticText = new wxStaticText(configTab, wxID_ANY, "Damage Type", wxPoint(220, 90));
	damageTypesComboxBox = new wxComboBox(configTab, wxID_ANY, "BULLET", wxPoint(220, 110), wxSize(175, 25));
	damageTypesComboxBox->Append(wxArrayString(cWeaponsToolkit::getDamageTypesCount(), generatedWeapon->damageTypes));

	//todo add HeadShotDamageModifierPlayer

	//TAB 3 - Components
	weaponComponentsListCtrl = new wxListCtrl(componentsTab, wxID_ANY, wxPoint(20, 20), wxSize(260, 340), wxLC_REPORT, wxDefaultValidator);
	wxListItem colComps;
	colComps.SetId(0);
	colComps.SetText(_("Components"));
	colComps.SetWidth(260);
	weaponComponentsListCtrl->InsertColumn(0, colComps);

	wxButton* addComponentButton = new wxButton(componentsTab, wxID_ANY, "Add Component", wxPoint(20, 370), wxSize(125,25));
	wxButton* removeComponentButton = new wxButton(componentsTab, wxID_ANY, "Remove Component", wxPoint(155, 370),wxSize(125, 25));

	wxStaticText* weaponComponentsStaticText = new wxStaticText(componentsTab, wxID_ANY, "Select Component Template", wxPoint(300, 20));
	weaponComponentsComboBox = new wxComboBox(componentsTab, wxID_ANY, "COMPONENT_AT_RAILCOVER_01", wxPoint(300, 40), wxSize(225, 25));
	weaponComponentsComboBox->Append(wxArrayString(cWeaponsToolkit::getWeaponComponentCount(), generatedWeapon->nativeComponents));

	wxStaticText* componentNameStaticText = new wxStaticText(componentsTab, wxID_ANY, "Component Name:", wxPoint(300, 80));
	componentNameTextCtrl = new wxTextCtrl(componentsTab, wxID_ANY, "COMPONENT_AT_RAILCOVER_01", wxPoint(300, 100), wxSize(225, 25));

	wxStaticText* componentModelNameStaticText = new wxStaticText(componentsTab, wxID_ANY, "Model Name:", wxPoint(300, 140));
	componentModelNameTextCtrl = new wxTextCtrl(componentsTab, wxID_ANY, "w_at_railcover_01", wxPoint(300, 160), wxSize(225, 25));

	wxStaticText* componentLODStaticText = new wxStaticText(componentsTab, wxID_ANY, "Model LOD:", wxPoint(300, 200));
	componentLODTextCtrl = new wxTextCtrl(componentsTab, wxID_ANY, "300.0", wxPoint(300, 220), wxSize(225, 25));

	//todo gray out box if attachment template is not a mag.
	wxStaticText* componentClipSizeStaticText = new wxStaticText(componentsTab, wxID_ANY, "Clip Size:", wxPoint(300, 260));
	componentClipSizeTextCtrl = new wxTextCtrl(componentsTab, wxID_ANY, "", wxPoint(300, 280), wxSize(225, 25));

	wxStaticText* weaponAmmoInfoStaticText = new wxStaticText(componentsTab, wxID_ANY, "Ammo Info", wxPoint(300, 320));
	weaponAmmoInfoComboBox = new wxComboBox(componentsTab, wxID_ANY, "", wxPoint(300, 340), wxSize(225, 25));
	weaponAmmoInfoComboBox->Append(wxArrayString(cWeaponsToolkit::getAmmoInfoCount(), generatedWeapon->nativeAmmoInfos));

	//Event Handlers
	addComponentButton->Bind(wxEVT_BUTTON, &cWeaponsToolkit::onAddComponent, this);
	removeComponentButton->Bind(wxEVT_BUTTON, &cWeaponsToolkit::onRemoveComponent, this);
	weaponComponentsListCtrl->Bind(wxEVT_LIST_ITEM_SELECTED, &cWeaponsToolkit::onSelectComponent, this);
	weaponComponentsComboBox->Bind(wxEVT_COMBOBOX, &cWeaponsToolkit::onComponentTemplateChanged, this);
	componentNameTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onComponentNameChanged, this);
	componentModelNameTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onComponentModelNameChanged, this);
	componentLODTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onComponentLODChanged, this);
	componentClipSizeTextCtrl->Bind(wxEVT_TEXT, &cWeaponsToolkit::onComponentClipSizeChanged, this);

	//TAB 4 - Export
	//todo
}