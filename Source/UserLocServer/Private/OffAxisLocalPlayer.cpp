#include "OffAxisLocalPlayer.h"

FSceneView* UOffAxisLocalPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation, FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex)
{

	FSceneView* tmp = ULocalPlayer::CalcSceneView(ViewFamily, OutViewLocation, OutViewRotation, Viewport, ViewDrawer, StereoViewIndex);

	if (tmp)
	{
		FMatrix CurrentMatrix = tmp->ViewMatrices.GetProjectionMatrix();

		float FOV = FMath::DegreesToRadians(60.0f);
		FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(FOV, 16.0f, 9.0f, GNearClippingPlane);

		tmp->UpdateProjectionMatrix(ProjectionMatrix);
	}

	return tmp;
}