// (c) 2023 Pururum LLC. All rights reserved.


#include "Scene/FineScene.h"


// Sets default values
AFineScene::AFineScene(): Super()
{
	// Lifecycle is controlled by UFineSceneLoop, not world partition.
	SetIsSpatiallyLoaded(false);
}
