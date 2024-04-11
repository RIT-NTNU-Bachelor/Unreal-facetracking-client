#include "CoreMinimal.h"
#include "Runtime/Core/Public/Math/IntRect.h"
#include "Engine/LocalPlayer.h"
#include "OffAxisLocalPlayer.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class USERLOCSERVER_API UOffAxisLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

	FSceneView* CalcSceneView(class FSceneViewFamily* ViewFamily,
		FVector& OutViewLocation,
		FRotator& OutViewRotation,
		FViewport* Viewport,
		class FViewElementDrawer* ViewDrawer = NULL,
		int32 StereoViewIndex = -1) override;
};