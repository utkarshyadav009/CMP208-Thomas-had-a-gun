#pragma once
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <graphics/mesh_instance.h>
#include "graphics/scene.h"
#include <system/debug_log.h>
#include<iostream>
using std::cout;
using std::endl;

class Trees
{	
private:
	gef::MeshInstance trees[25];//tree meshes 
	gef::Scene* scene_assets_;//scene assests 
public:
	void render(gef::Renderer3D* render); //rendering all the trees 
	void InitTrees(gef::Platform& platform); //initialising the trees
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);//loading models
};

