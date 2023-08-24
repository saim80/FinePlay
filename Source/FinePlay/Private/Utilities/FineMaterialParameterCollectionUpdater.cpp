// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FineMaterialParameterCollectionUpdater.h"

#include "FinePlayLog.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"


// Sets default values for this component's properties
UFineMaterialParameterCollectionUpdater::UFineMaterialParameterCollectionUpdater()
{
}

void UFineMaterialParameterCollectionUpdater::BeginUpdating()
{
	// start timer to update material parameter collection.
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this,
	                                       &UFineMaterialParameterCollectionUpdater::UpdateMaterialParameterCollection,
	                                       UpdateInterval, true);
}

void UFineMaterialParameterCollectionUpdater::EndUpdating()
{
	// stop timer to update material parameter collection.
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
}

void UFineMaterialParameterCollectionUpdater::UpdateMaterialParameterCollection_Implementation()
{
	if (!IsValid(MaterialParameterCollection))
	{
		// Try to load synchronously.
		MaterialParameterCollection = Cast<UMaterialParameterCollection>(
			MaterialParameterCollectionAsset.LoadSynchronous());
		// if it' still invalid, warn and return.
		if (!IsValid(MaterialParameterCollection))
		{
			FP_WARNING("Loading material parameter collection asset failed: %s",
			           *MaterialParameterCollectionAsset.ToSoftObjectPath().ToString());
			return;
		}
	}
	if (FloatParameters.IsEmpty() && VectorParameters.IsEmpty())
	{
		// if there is no parameters, warn and return.
		FP_LOG("There is no parameters to update.");
		return;
	}
	// Get material parameter collection instance.
	const auto Instance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);

	// for each float parameters, set the values to the collection.
	for (const auto& Pair : FloatParameters)
	{
		Instance->SetScalarParameterValue(FName(*Pair.Key), Pair.Value);
	}
	// for each vector parameters, set the values to the collection.
	for (const auto& Pair : VectorParameters)
	{
		Instance->SetVectorParameterValue(FName(*Pair.Key), Pair.Value);
	}
}

void UFineMaterialParameterCollectionUpdater::BeginPlay()
{
	Super::BeginPlay();
	const auto ObjectPath = MaterialParameterCollectionAsset.ToSoftObjectPath();
	// Get streamable manager.
	const auto& WeakThis = TWeakObjectPtr<UFineMaterialParameterCollectionUpdater>(this);
	StreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		{ObjectPath},
		FStreamableDelegate::CreateLambda([=]()
		{
			if (!WeakThis.IsValid())
			{
				return;
			}
			const auto& StreamableManager = UAssetManager::GetStreamableManager();
			if (StreamableManager.IsAsyncLoadComplete(ObjectPath))
			{
				if (const auto Asset = Cast<UMaterialParameterCollection>(StreamableHandle->GetLoadedAsset()))
				{
					MaterialParameterCollection = Asset;
					FP_LOG("Loaded material parameter collection asset: %s", *ObjectPath.ToString());
					WeakThis->BeginUpdating();
				}
				else
				{
					FP_LOG("Loading material parameter collection asset failed: %s", *ObjectPath.ToString());
				}
			}
			else
			{
				FP_LOG("Loading material parameter collection asset failed: %s", *ObjectPath.ToString());
			}
		}));
}

void UFineMaterialParameterCollectionUpdater::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndUpdating();
	StreamableHandle = nullptr;
	MaterialParameterCollection = nullptr;
	Super::EndPlay(EndPlayReason);
}
