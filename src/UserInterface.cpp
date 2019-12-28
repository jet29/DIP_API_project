#include "UserInterface.h"


// Global static pointer used to ensure a single instance of the class.
UI * UI::interface = NULL;
// External values of windows size:
extern unsigned int windowWidth, windowHeight;

/**
* Creates an instance of the class
*
* @return the instance of this class
*/
UI * UI::Instance(){
	if (!interface)   // Only allow one instance of class to be generated.
		interface = new UI();

	return interface;
}

UI::UI(){
	tbInterface = TwNewBar("Tweakbar - API");
	
	//Basic config:
	//TwDefine("'Tweakbar - API' refresh = '0.0001f'");
	//TwDefine("'Tweakbar - API' resizable = false");
	//TwDefine("'Tweakbar - API' fontresizable = false");
	//TwDefine("'Tweakbar - API' movable = true");
	//TwDefine("'Tweakbar - API' position = '10 20'");
	TwDefine("'Tweakbar - API' size = '250 400'");
	//TwDefine("'Tweakbar - API' color = '101 57 245'");
	//TwDefine("'Tweakbar - API' valueswidth=120 ");
	//TwDefine("'Tweakbar - API' visible=true ");
	std::string myVar = "hla";
	TwAddVarRW(tbInterface, "NameOfMyVariable", TW_TYPE_STDSTRING, &myVar, "");

	////Type:	
	//TwEnumVal materialType[] = { {BLINN_PHONG, "Blinn-Phong"}, {OREN_NAYAR, "Oren-Nayar"}, {COOK_TORRANCE, "Cook-Torrance"},
	//							 {REFLECTION,"Reflection"}, {BLEND,"Blending"} };

	//TwType DeployMaterialType = TwDefineEnum("Material type", materialType, 5);

	//
	//// Diff type:
	//TwEnumVal diffType[] = { {ALBEDO, "Albedo"}, {COLOR, "Material color"} };
	//TwType DeployDiffType = TwDefineEnum("Diffuse Type", diffType, 2);

	//// Switch type:
	//TwEnumVal switchType[] = {{ false, "OFF" }, { true,"ON" }};
	//TwType DeploySwitchType = TwDefineEnum("ON/OFF", switchType, 2);

	////Swap to camera mode
	//TwAddVarRW(tbInterface, "Camera mode", DeploySwitchType, &cameraMode, "label='Camera Mode'");

	////Model's name:
	//TwAddVarRO(tbInterface, "Model Selected", TW_TYPE_STDSTRING, &modelName, "label='Name' readonly=true");
	//TwAddSeparator(tbInterface, "", NULL);

	////Turn light on/off
	//TwAddVarRW(tbInterface, "Switch", DeploySwitchType, &lightSwitch, "visible=false");

	//// Material type 
	//TwAddVarRW(tbInterface, "Material type", DeployMaterialType, &material.material_type, "visible=false label='Shader'");

	//// Diffuse type
	//TwAddVarRW(tbInterface, "Diffuse type", DeployDiffType, &material.diffuse_type, "visible=false label='Diffuse map'");
	//TwAddVarRW(tbInterface, "Specular map", DeploySwitchType, &material.spec_type, "visible=false");
	//TwAddVarRW(tbInterface, "Normal map", DeploySwitchType, &material.norm_type, "visible=false");
	//TwAddVarRW(tbInterface, "Depth map", DeploySwitchType, &material.depth_type, "visible=false");

	////Model's Attributes
	////Direction 
	//TwAddVarRW(tbInterface, "Direction", TW_TYPE_DIR3F, &direction, " axisz=z ");
	////Position 
	//TwAddVarRW(tbInterface, "X", TW_TYPE_FLOAT, &position[0], " group='Position' step=0.01");
	//TwAddVarRW(tbInterface, "Y", TW_TYPE_FLOAT, &position[1], " group='Position' step=0.01");
	//TwAddVarRW(tbInterface, "Z", TW_TYPE_FLOAT, &position[2], " group='Position' step=0.01");
	//TwDefine("'Tweakbar - API'/'Position' visible = false");
	//// Scale
	//TwAddVarRW(tbInterface, "Scale", TW_TYPE_FLOAT, &scale, " visible=false step=0.001");
	//// Rotation
	//TwAddVarRW(tbInterface, "Rotation", TW_TYPE_QUAT4F, &quat, "visible=false");
	//TwAddSeparator(tbInterface, "sep1", NULL);
	//
	////Color
	//TwAddVarRW(tbInterface, "Ambient", TW_TYPE_COLOR3F, &material.ambient, "group='Material color' label = 'Ambient'");
	//TwAddVarRW(tbInterface, "Diffuse", TW_TYPE_COLOR3F, &material.diffuse, "group='Material color' label = 'Diffuse'");
	//TwAddVarRW(tbInterface, "Specular", TW_TYPE_COLOR3F, &material.specular, "group='Material color' label = 'Specular'");
	//TwAddSeparator(tbInterface, "sep2", NULL);

	////Model attributes

	//TwAddVarRW(tbInterface, "Height scale", TW_TYPE_FLOAT, &material.height_scale, "min=0.0 max=0.7 step=0.001 visible = false");
	//TwAddVarRW(tbInterface, "Shininess", TW_TYPE_FLOAT, &material.shininess, "min=1.0 max=255 step=0.1 visible = false");
	//TwAddVarRW(tbInterface, "Roughness", TW_TYPE_FLOAT, &material.roughness, "min=0.1 max=1.0 step=0.001 visible = false");
	//TwAddVarRW(tbInterface, "Reflection", TW_TYPE_FLOAT, &material.reflection, "min=0.0 max=1.0 step=0.001 visible = false");
	//
	//TwAddVarRW(tbInterface, "RefAmbient", TW_TYPE_FLOAT, &material.ambient_reflective_index, "group='Refractance' label='Ambient' min=1.0 max=3.0 step=0.001");
	//TwAddVarRW(tbInterface, "RefMaterial", TW_TYPE_FLOAT, &material.material_reflective_index, "group='Refractance' label='Material' min=1.0 max=3.0 step=0.001");
	//TwDefine("'Tweakbar - API'/'Refractance' visible = false");

	////Light attributes
	//	//Atenuation's type:
	//	TwAddVarRW(tbInterface, "Constant ", TW_TYPE_FLOAT, &attenuation.constant, "group='Atenuation' min=0.0  step=0.001");
	//	TwAddVarRW(tbInterface, "Linear   ", TW_TYPE_FLOAT, &attenuation.linear, "group='Atenuation'   min=0.0  step=0.001");
	//	TwAddVarRW(tbInterface, "Quadratic", TW_TYPE_FLOAT, &attenuation.quadratic, "group='Atenuation' min=0.0 step=0.001");
	//	//Cutoffs
	//	TwAddVarRW(tbInterface, "Cutoff", TW_TYPE_FLOAT, &cutOff, "group='Cutoffs' step=0.0001 min=0.0 max=1.0");
	//	TwAddVarRW(tbInterface, "Outer cutoff", TW_TYPE_FLOAT, &outerCutOff, "group='Cutoffs' step=0.0001 min=0.1 max =0.99");
	//	TwDefine("'Tweakbar - API'/'Cutoffs' visible = false");

}

void UI::reshape(GLFWwindow* window, int width, int height){
	TwWindowSize(width, height);
}

//void UI::swapToPointLightMode() {
//	TwDefine("'Tweakbar - API' visible=true ");
//	//Turn other atributes OFF
//	TwDefine("'Tweakbar - API'/'Direction' visible = false");
//	TwDefine("'Tweakbar - API'/'Rotation' visible = false");
//	TwDefine("'Tweakbar - API'/'Scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Cutoffs' visible = false");
//	TwDefine("'Tweakbar - API'/'Material type' visible = false");
//	TwDefine("'Tweakbar - API'/'Diffuse type' visible = false");
//	TwDefine("'Tweakbar - API'/'Specular map' visible = false");
//	TwDefine("'Tweakbar - API'/'Depth map' visible = false");
//	TwDefine("'Tweakbar - API'/'Normal map' visible = false");
//	TwDefine("'Tweakbar - API'/'Height scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Shininess' visible=false");
//	TwDefine("'Tweakbar - API'/'Roughness' visible=false");
//	TwDefine("'Tweakbar - API'/'Reflection' visible=false");
//	TwDefine("'Tweakbar - API'/'Refractance' visible=false");
//
//	//Turn Point light attributes ON:
//	TwDefine("'Tweakbar - API'/'Position' visible = true");
//	TwDefine("'Tweakbar - API'/'Atenuation' visible = true");
//	TwDefine("'Tweakbar - API'/'Switch' visible = true");
//	TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//
//}
//
//void UI::swaptoDirectionalLightMode() {
//	TwDefine("'Tweakbar - API' visible=true ");
//	//Turn other atributes OFF
//	TwDefine("'Tweakbar - API'/'Atenuation' visible = false");
//	TwDefine("'Tweakbar - API'/'Scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Rotation' visible = false");
//	TwDefine("'Tweakbar - API'/'Position' visible = false");
//	TwDefine("'Tweakbar - API'/'Cutoffs' visible = false");
//	TwDefine("'Tweakbar - API'/'Material type' visible = false");
//	TwDefine("'Tweakbar - API'/'Diffuse type' visible = false");
//	TwDefine("'Tweakbar - API'/'Specular map' visible = false");
//	TwDefine("'Tweakbar - API'/'Normal map' visible = false");
//	TwDefine("'Tweakbar - API'/'Depth map' visible = false");
//	TwDefine("'Tweakbar - API'/'Height scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Shininess' visible=false");
//	TwDefine("'Tweakbar - API'/'Reflection' visible=false");
//	TwDefine("'Tweakbar - API'/'Refractance' visible=false");
//
//	//Turn Directional light attributes ON:
//	TwDefine("'Tweakbar - API'/'Direction' visible = true");
//	TwDefine("'Tweakbar - API'/'Switch' visible = true");
//	TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//}
//
//void UI::swaptoFlashlightMode() {
//	TwDefine("'Tweakbar - API' visible=true ");
//	//Turn other atributes OFF
//	TwDefine("'Tweakbar - API'/'Direction' visible = false");
//	TwDefine("'Tweakbar - API'/'Scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Position' visible = false");
//	TwDefine("'Tweakbar - API'/'Rotation' visible = false");
//	TwDefine("'Tweakbar - API'/'Shininess' visible=false");
//	TwDefine("'Tweakbar - API'/'Roughness' visible=false");
//	TwDefine("'Tweakbar - API'/'Material type' visible = false");
//	TwDefine("'Tweakbar - API'/'Diffuse type' visible = false");
//	TwDefine("'Tweakbar - API'/'Normal map' visible = false");
//	TwDefine("'Tweakbar - API'/'Specular map' visible = false");
//	TwDefine("'Tweakbar - API'/'Depth map' visible = false");
//	TwDefine("'Tweakbar - API'/'Height scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Reflection' visible=false");
//	TwDefine("'Tweakbar - API'/'Refractance' visible=false");
//
//	//Turn flashlight atributes ON
//	TwDefine("'Tweakbar - API'/'Cutoffs' visible = true");
//	TwDefine("'Tweakbar - API'/'Atenuation' visible = true");
//	TwDefine("'Tweakbar - API'/'Switch' visible = true");
//	TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//}
//
//void UI::swaptoRegularMode(MATERIAL_TYPE mat_type) {
//	TwDefine("'Tweakbar - API' visible=true ");
//	//Turn other atributes OFF
//	TwDefine("'Tweakbar - API'/'sep1' visible = false");
//	TwDefine("'Tweakbar - API'/'Atenuation' visible = false");
//	TwDefine("'Tweakbar - API'/'Cutoffs' visible = false");
//	TwDefine("'Tweakbar - API'/'Direction' visible = false");
//	TwDefine("'Tweakbar - API'/'Switch' visible = false");
//	TwDefine("'Tweakbar - API'/'Shininess' visible=false");
//	TwDefine("'Tweakbar - API'/'Roughness' visible=false");
//	TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//
//	//Turn regular model attributes ON:
//	TwDefine("'Tweakbar - API'/'Position' visible = true");
//	TwDefine("'Tweakbar - API'/'Scale' visible = true");
//	TwDefine("'Tweakbar - API'/'Rotation' visible = true");
//	TwDefine("'Tweakbar - API'/'Material type' visible = true");
//	TwDefine("'Tweakbar - API'/'Diffuse type' visible = true");
//	TwDefine("'Tweakbar - API'/'Specular map' visible = true");
//	TwDefine("'Tweakbar - API'/'Normal map' visible = true");
//	TwDefine("'Tweakbar - API'/'Depth map' visible = true");
//	TwDefine("'Tweakbar - API'/'Height scale' visible = false");
//	TwDefine("'Tweakbar - API'/'Shininess' visible=false");
//	TwDefine("'Tweakbar - API'/'Diffuse' visible=false");
//	TwDefine("'Tweakbar - API'/'Reflection' visible=false");
//	TwDefine("'Tweakbar - API'/'Refractance' visible=false");
//	TwDefine("'Tweakbar - API'/'Roughness' visible=false");
//
//	if (mat_type == BLINN_PHONG) {
//		TwDefine("'Tweakbar - API'/'Shininess' visible=true");
//		TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//		TwDefine("'Tweakbar - API'/'Height scale' visible = true");
//	}
//	else if (mat_type == OREN_NAYAR){
//		TwDefine("'Tweakbar - API'/'Diffuse' visible=false");
//		TwDefine("'Tweakbar - API'/'Roughness' visible=true min=0.0");
//	}
//	else if (mat_type == REFLECTION) {
//		TwDefine("'Tweakbar - API'/'Shininess' visible=true");
//		TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//		TwDefine("'Tweakbar - API'/'Reflection' visible=true");
//		TwDefine("'Tweakbar - API'/'Refractance' visible=true");
//	}
//	else {
//		TwDefine("'Tweakbar - API'/'Roughness' visible=true min=0.01");
//		TwDefine("'Tweakbar - API'/'Diffuse' visible=true");
//	}
//
//}
//
//void UI::updateFromUI(int pickedIndex, std::vector<Model *> *model, std::vector<Light *> *light, bool *cameraMode){
//	*cameraMode = this->cameraMode;
//	// If there's no object selected
//	if (pickedIndex == INT_MIN)
//		return;
//
//	switch (pickedIndex) {
//	// Directional light interface
//	case -2:
//		// Set parameters into tweakbar
//		(*light)[0]->direction = this->direction;
//		(*light)[0]->material.ambient = this->material.ambient;
//		(*light)[0]->material.diffuse = this->material.diffuse;
//		(*light)[0]->material.specular = this->material.specular;
//		(*light)[0]->lightSwitch = this->lightSwitch;
//		break;
//	// Flashlight
//	case -1:
//		(*light)[1]->cutOff = this->cutOff;
//		(*light)[1]->outerCutOff = this->outerCutOff;
//		(*light)[1]->material.ambient = this->material.ambient;
//		(*light)[1]->material.diffuse = this->material.diffuse;
//		(*light)[1]->material.specular = this->material.specular;
//		(*light)[1]->attenuation.constant = this->attenuation.constant;
//		(*light)[1]->attenuation.linear = this->attenuation.linear;
//		(*light)[1]->attenuation.quadratic = this->attenuation.quadratic;
//		(*light)[1]->lightSwitch = this->lightSwitch;
//		break;
//	// Pointlight interface
//	case 0: case 1: case 2: case 3:
//		//Set parameters into tweakbar
//		(*light)[(*model)[pickedIndex]->lightIndex]->position = this->position;
//		(*light)[(*model)[pickedIndex]->lightIndex]->material.ambient = this->material.ambient;
//		(*light)[(*model)[pickedIndex]->lightIndex]->material.diffuse = this->material.diffuse;
//		(*light)[(*model)[pickedIndex]->lightIndex]->material.specular = this->material.specular;
//		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.constant = this->attenuation.constant;
//		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.linear = this->attenuation.linear;
//		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.quadratic = this->attenuation.quadratic;
//		(*light)[(*model)[pickedIndex]->lightIndex]->lightSwitch = this->lightSwitch;
//		break;
//	// Regular model interface
//	default:
//		(*model)[pickedIndex]->material.ambient = this->material.ambient;
//		(*model)[pickedIndex]->material.diffuse = this->material.diffuse;
//		(*model)[pickedIndex]->material.specular = this->material.specular;
//		(*model)[pickedIndex]->material.shininess = this->material.shininess;
//		(*model)[pickedIndex]->material.reflection = this->material.reflection;
//		(*model)[pickedIndex]->material.roughness = this->material.roughness;
//		(*model)[pickedIndex]->material.height_scale = this->material.height_scale;
//		(*model)[pickedIndex]->material.material_type = this->material.material_type;
//		(*model)[pickedIndex]->material.diffuse_type = this->material.diffuse_type;
//		(*model)[pickedIndex]->material.spec_type = this->material.spec_type;
//		(*model)[pickedIndex]->material.norm_type = this->material.norm_type;
//		(*model)[pickedIndex]->material.depth_type = this->material.depth_type;
//		(*model)[pickedIndex]->material.ambient_reflective_index = this->material.ambient_reflective_index;
//		(*model)[pickedIndex]->material.material_reflective_index = this->material.material_reflective_index;
//		(*model)[pickedIndex]->transformations.translate = this->position;
//		(*model)[pickedIndex]->transformations.scale = this->scale;
//		(*model)[pickedIndex]->transformations.quat = this->quat;
//		break;
//	}
//}
//
//void UI::setUI(int pickedIndex, std::vector<Model *> model, std::vector<Light *> light, bool cameraMode) {
//	this->cameraMode = cameraMode;
//	// If there's no object selected
//	if (pickedIndex == INT_MIN)
//		return;
//
//	switch (pickedIndex){
//	// Directional light
//	case -2:
//		// Set parameters into tweakbar
//		this->modelName = light[0]->name;
//		this->direction = light[0]->direction;
//		this->material.ambient = light[0]->material.ambient;
//		this->material.diffuse = light[0]->material.diffuse;
//		this->material.specular = light[0]->material.specular;
//		this->lightSwitch = light[0]->lightSwitch;
//		// Swap to appropiate interface
//		this->swaptoDirectionalLightMode();
//		break;
//	// Flashlight
//	case -1:
//		// Set parameters into tweakbar
//		this->modelName = light[1]->name;
//		this->cutOff = light[1]->cutOff;
//		this->outerCutOff = light[1]->outerCutOff;
//		this->material.ambient = light[1]->material.ambient;
//		this->material.diffuse = light[1]->material.diffuse;
//		this->material.specular = light[1]->material.specular;
//		this->attenuation.constant = light[1]->attenuation.constant;
//		this->attenuation.linear = light[1]->attenuation.linear;
//		this->attenuation.quadratic = light[1]->attenuation.quadratic;
//		this->lightSwitch = light[1]->lightSwitch;
//		// Swap to appropiate interface
//		this->swaptoFlashlightMode();
//		break;
//	// Pointlight
//	case 0: case 1: case 2: case 3: 
//		//Set parameters into tweakbar
//		this->modelName = light[model[pickedIndex]->lightIndex]->name;
//		this->position = light[model[pickedIndex]->lightIndex]->position;
//		this->material.ambient = light[model[pickedIndex]->lightIndex]->material.ambient;
//		this->material.diffuse = light[model[pickedIndex]->lightIndex]->material.diffuse;
//		this->material.specular = light[model[pickedIndex]->lightIndex]->material.specular;
//		this->attenuation.constant = light[model[pickedIndex]->lightIndex]->attenuation.constant;
//		this->attenuation.linear = light[model[pickedIndex]->lightIndex]->attenuation.linear;
//		this->attenuation.quadratic = light[model[pickedIndex]->lightIndex]->attenuation.quadratic;
//		this->lightSwitch = light[model[pickedIndex]->lightIndex]->lightSwitch;
//		
//		// Swap to appropiate interface
//		this->swapToPointLightMode();
//		break;
//	// Regular model interface
//	default:
//		this->modelName = model[pickedIndex]->name;
//		this->material.ambient = model[pickedIndex]->material.ambient;
//		this->material.diffuse = model[pickedIndex]->material.diffuse;
//		this->material.specular = model[pickedIndex]->material.specular;
//		this->material.shininess = model[pickedIndex]->material.shininess;
//		this->material.reflection = model[pickedIndex]->material.reflection;
//		this->material.roughness = model[pickedIndex]->material.roughness;
//		this->material.height_scale = model[pickedIndex]->material.height_scale;
//		this->material.material_type = model[pickedIndex]->material.material_type;
//		this->material.diffuse_type = model[pickedIndex]->material.diffuse_type;
//		this->material.spec_type = model[pickedIndex]->material.spec_type;
//		this->material.norm_type = model[pickedIndex]->material.norm_type;
//		this->material.depth_type = model[pickedIndex]->material.depth_type;
//		this->material.ambient_reflective_index = model[pickedIndex]->material.ambient_reflective_index;
//		this->material.material_reflective_index = model[pickedIndex]->material.material_reflective_index;
//		this->position = model[pickedIndex]->transformations.translate;
//		this->scale = model[pickedIndex]->transformations.scale;
//		this->quat = model[pickedIndex]->transformations.quat;
//
//		// Swap to appropiate interface
//		this->swaptoRegularMode(model[pickedIndex]->material.material_type);
//	}
//}
