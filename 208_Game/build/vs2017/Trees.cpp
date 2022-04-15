#include "Trees.h"
#include"Trees.h"
gef::Scene* Trees::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* Trees::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}


void Trees::render(gef::Renderer3D* render)
{
	for (int i = 0; i < 20; i++)
	{
		render->DrawMesh(trees[i]);
	}
}

//initialising trees in a certain pattern 
void Trees::InitTrees(gef::Platform& platform_)
{
	const char* scene_asset_filename = "cone_tree.scn";
	scene_assets_ = LoadSceneAssets(platform_, scene_asset_filename);
	
	int i = 0;
	int offset = 0;
		for (float z_up = -100; z_up < 0; z_up += 20)
		{
			for (float x_up = 100; x_up > 0; x_up -= 20)
			{
				if (scene_assets_)
				{	
					
					trees[i].set_mesh(GetMeshFromSceneAssets(scene_assets_));
					gef::Matrix44 t;
					t.SetIdentity();
					t.SetTranslation({(-x_up+60.f),0.0f,(z_up+50.0f)});
					trees[i].set_transform(t);

				
				}
				else
				{
					gef::DebugOut("Scene file %s failed to load\n", scene_asset_filename);
				}
				i++;
			}
		
		}


}
